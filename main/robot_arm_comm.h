#ifndef ROBOT_ARM_COMM_H
#define ROBOT_ARM_COMM_H

#include <stdbool.h>

// Robot arm communication status
typedef enum {
    ROBOT_ARM_COMM_OK,
    ROBOT_ARM_COMM_ERROR,
    ROBOT_ARM_COMM_TIMEOUT,
    ROBOT_ARM_COMM_NOT_CONNECTED
} robot_arm_comm_status_t;

// Robot arm joint identifiers (1-4 for RoArm-M2)
typedef enum {
    ROBOT_ARM_JOINT_BASE = 1,      // Joint 1 - Base rotation
    ROBOT_ARM_JOINT_SHOULDER = 2,  // Joint 2 - Shoulder
    ROBOT_ARM_JOINT_ELBOW = 3,     // Joint 3 - Elbow
    ROBOT_ARM_JOINT_GRIPPER = 4    // Joint 4 - Gripper tilt
} robot_arm_joint_t;

// Function declarations
robot_arm_comm_status_t robot_arm_init(const char* robot_ip);
robot_arm_comm_status_t robot_arm_get_status(void);

// Basic control commands
robot_arm_comm_status_t robot_arm_enable_torque(void);
robot_arm_comm_status_t robot_arm_disable_torque(void);
robot_arm_comm_status_t robot_arm_home(void);

// Joint movement commands
robot_arm_comm_status_t robot_arm_move_joint(robot_arm_joint_t joint, float radians, int speed, int acceleration);
robot_arm_comm_status_t robot_arm_move_base(float radians, int speed, int acceleration);
robot_arm_comm_status_t robot_arm_move_shoulder(float radians, int speed, int acceleration);
robot_arm_comm_status_t robot_arm_move_elbow(float radians, int speed, int acceleration);
robot_arm_comm_status_t robot_arm_move_gripper(float radians, int speed, int acceleration);

// LED control
robot_arm_comm_status_t robot_arm_led_on(void);
robot_arm_comm_status_t robot_arm_led_off(void);
robot_arm_comm_status_t robot_arm_led_set(int brightness); // 0-255

// Connection status
bool robot_arm_is_connected(void);

#endif // ROBOT_ARM_COMM_H 