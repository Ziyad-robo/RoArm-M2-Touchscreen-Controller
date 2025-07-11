#include <string.h>
#include <stdio.h>
#include "esp_log.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "robot_arm_comm.h"
#include "wifi_manager.h"

static const char *ROBOT_TAG = "ROBOT_ARM";

// Robot arm communication settings
static char robot_ip[16] = {0};
static bool robot_initialized = false;
static robot_arm_comm_status_t comm_status = ROBOT_ARM_COMM_NOT_CONNECTED;

// HTTP response buffer
#define HTTP_BUFFER_SIZE 1024
static char http_response_buffer[HTTP_BUFFER_SIZE];
static int http_response_len = 0;

// HTTP event handler
static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(ROBOT_TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(ROBOT_TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(ROBOT_TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(ROBOT_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_HEADERS_COMPLETE:
            ESP_LOGD(ROBOT_TAG, "HTTP_EVENT_ON_HEADERS_COMPLETE");
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(ROBOT_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (evt->data_len < HTTP_BUFFER_SIZE - http_response_len) {
                memcpy(http_response_buffer + http_response_len, evt->data, evt->data_len);
                http_response_len += evt->data_len;
                http_response_buffer[http_response_len] = '\0';
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(ROBOT_TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(ROBOT_TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(ROBOT_TAG, "HTTP_EVENT_REDIRECT");
            break;
    }
    return ESP_OK;
}

// Send HTTP GET request with JSON data to robot arm (equivalent to curl --get --data-urlencode)
static robot_arm_comm_status_t send_robot_command_json(const char* json_data)
{
    if (!wifi_is_connected()) {
        ESP_LOGW(ROBOT_TAG, "WiFi not connected");
        return ROBOT_ARM_COMM_NOT_CONNECTED;
    }

    if (!robot_initialized) {
        ESP_LOGW(ROBOT_TAG, "Robot arm not initialized");
        return ROBOT_ARM_COMM_NOT_CONNECTED;
    }

    // URL encode the JSON data
    char encoded_json[512];
    char url[640];
    
    // Simple URL encoding for the JSON string
    int j = 0;
    for (int i = 0; json_data[i] != '\0' && j < sizeof(encoded_json) - 4; i++) {
        if (json_data[i] == ' ') {
            encoded_json[j++] = '+';
        } else if (json_data[i] == '{' || json_data[i] == '}' || json_data[i] == '"' || 
                   json_data[i] == ':' || json_data[i] == ',') {
            sprintf(&encoded_json[j], "%%%02X", (unsigned char)json_data[i]);
            j += 3;
        } else {
            encoded_json[j++] = json_data[i];
        }
    }
    encoded_json[j] = '\0';

    // Build the complete URL with encoded JSON parameter
    snprintf(url, sizeof(url), "http://%s/js?json=%s", robot_ip, encoded_json);
    
    ESP_LOGI(ROBOT_TAG, "Sending command: %s", json_data);
    ESP_LOGD(ROBOT_TAG, "Full URL: %s", url);

    // Reset response buffer
    http_response_len = 0;
    memset(http_response_buffer, 0, HTTP_BUFFER_SIZE);

    // Configure HTTP client
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_event_handler,
        .timeout_ms = 5000,
        .buffer_size = HTTP_BUFFER_SIZE,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        ESP_LOGE(ROBOT_TAG, "Failed to initialize HTTP client");
        return ROBOT_ARM_COMM_ERROR;
    }

    // Perform HTTP GET request
    esp_err_t err = esp_http_client_perform(client);
    int status_code = esp_http_client_get_status_code(client);

    esp_http_client_cleanup(client);

    if (err != ESP_OK) {
        ESP_LOGE(ROBOT_TAG, "HTTP request failed: %s", esp_err_to_name(err));
        return ROBOT_ARM_COMM_ERROR;
    }

    if (status_code != 200) {
        ESP_LOGE(ROBOT_TAG, "HTTP request failed with status code: %d", status_code);
        return ROBOT_ARM_COMM_ERROR;
    }

    ESP_LOGI(ROBOT_TAG, "Robot command sent successfully");
    return ROBOT_ARM_COMM_OK;
}

robot_arm_comm_status_t robot_arm_init(const char* robot_ip_addr)
{
    if (!robot_ip_addr) {
        ESP_LOGE(ROBOT_TAG, "Invalid robot IP address");
        return ROBOT_ARM_COMM_ERROR;
    }

    strncpy(robot_ip, robot_ip_addr, sizeof(robot_ip) - 1);
    robot_ip[sizeof(robot_ip) - 1] = '\0';
    
    robot_initialized = true;
    comm_status = ROBOT_ARM_COMM_OK;
    
    ESP_LOGI(ROBOT_TAG, "Robot arm communication initialized for IP: %s", robot_ip);
    return ROBOT_ARM_COMM_OK;
}

robot_arm_comm_status_t robot_arm_get_status(void)
{
    return comm_status;
}

robot_arm_comm_status_t robot_arm_enable_torque(void)
{
    char json_cmd[] = "{\"T\":210,\"cmd\":1}";
    return send_robot_command_json(json_cmd);
}

robot_arm_comm_status_t robot_arm_disable_torque(void)
{
    char json_cmd[] = "{\"T\":210,\"cmd\":0}";
    return send_robot_command_json(json_cmd);
}

robot_arm_comm_status_t robot_arm_home(void)
{
    char json_cmd[] = "{\"T\":100}";
    return send_robot_command_json(json_cmd);
}

robot_arm_comm_status_t robot_arm_move_joint(robot_arm_joint_t joint, float radians, int speed, int acceleration)
{
    char json_cmd[128];
    snprintf(json_cmd, sizeof(json_cmd), 
             "{\"T\":101,\"joint\":%d,\"rad\":%.2f,\"spd\":%d,\"acc\":%d}", 
             joint, radians, speed, acceleration);
    return send_robot_command_json(json_cmd);
}

robot_arm_comm_status_t robot_arm_move_base(float radians, int speed, int acceleration)
{
    return robot_arm_move_joint(ROBOT_ARM_JOINT_BASE, radians, speed, acceleration);
}

robot_arm_comm_status_t robot_arm_move_shoulder(float radians, int speed, int acceleration)
{
    return robot_arm_move_joint(ROBOT_ARM_JOINT_SHOULDER, radians, speed, acceleration);
}

robot_arm_comm_status_t robot_arm_move_elbow(float radians, int speed, int acceleration)
{
    return robot_arm_move_joint(ROBOT_ARM_JOINT_ELBOW, radians, speed, acceleration);
}

robot_arm_comm_status_t robot_arm_move_gripper(float radians, int speed, int acceleration)
{
    return robot_arm_move_joint(ROBOT_ARM_JOINT_GRIPPER, radians, speed, acceleration);
}

robot_arm_comm_status_t robot_arm_led_on(void)
{
    char json_cmd[] = "{\"T\":114,\"led\":255}";
    return send_robot_command_json(json_cmd);
}

robot_arm_comm_status_t robot_arm_led_off(void)
{
    char json_cmd[] = "{\"T\":114,\"led\":0}";
    return send_robot_command_json(json_cmd);
}

robot_arm_comm_status_t robot_arm_led_set(int brightness)
{
    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;
    
    char json_cmd[64];
    snprintf(json_cmd, sizeof(json_cmd), "{\"T\":114,\"led\":%d}", brightness);
    return send_robot_command_json(json_cmd);
}

bool robot_arm_is_connected(void)
{
    return robot_initialized && wifi_is_connected();
} 