#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_wifi.h"
#include "esp_event.h"

// WiFi credentials for the robot arm
#define WIFI_SSID "RoArm-M2"
#define WIFI_PASS "12345678"
#define WIFI_MAXIMUM_RETRY 10

// WiFi connection status
typedef enum {
    WIFI_STATUS_DISCONNECTED,
    WIFI_STATUS_CONNECTING,
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_FAILED
} wifi_status_t;

// Function declarations
void wifi_init_sta(void);
wifi_status_t wifi_get_status(void);
bool wifi_is_connected(void);
char* wifi_get_ip_address(void);
void wifi_disconnect(void);
void wifi_reconnect(void);

#endif // WIFI_MANAGER_H 