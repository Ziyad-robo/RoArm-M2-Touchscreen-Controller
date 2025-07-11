# RoArm-M2 Touchscreen Controller

A touch-based robot arm controller built on ESP32-S3 with RGB LCD display, designed to control the RoArm-M2 robot arm via WiFi.

## Features

- **Real-time Control**: Interactive sliders for 4 robot joints + LED control
- **WiFi Connectivity**: Auto-connects to RoArm-M2 network
- **Touch Interface**: 800x480 RGB LCD with capacitive touch
- **Immediate Response**: 50ms UI updates with instant robot commands
- **Safety Limits**: Software-enforced joint angle restrictions

## Hardware Requirements

| Component | Specification |
|-----------|---------------|
| **MCU** | ESP32-S3R8 |
| **LCD Controller** | ST7701 (RGB interface) |
| **Touch Controller** | GT911 |
| **Display** | 800x480 RGB LCD |
| **Target Robot** | RoArm-M2 |

## Robot Control Mapping

| Control | Range | Robot Joint | Physical Range |
|---------|--------|-------------|----------------|
| **Base Slider** | 0-100% | Joint 1 | ±90° (-1.57 to +1.57 rad) |
| **Shoulder Slider** | 0-100% | Joint 2 | -11° to +80° (-0.2 to +1.4 rad) |
| **Arm Slider** | 0-100% | Joint 3 | -57° to +86° (-1.0 to +1.5 rad) |
| **Gripper Slider** | 0-100% | Joint 4 | Open to Close (1.08 to 3.14 rad) |
| **Light Switch** | ON/OFF | LED | 0-255 brightness |

## Hardware Connection

```
       ESP32-S3                            RGB LCD Panel
+-----------------------+              +-------------------+
|                   GND +--------------+GND                |
|                   3V3 +--------------+VCC                |
|                  PCLK +--------------+PCLK               |
|            DATA[15:0] +--------------+DATA[15:0]         |
|                 HSYNC +--------------+HSYNC              |
|                 VSYNC +--------------+VSYNC              |
|                    DE +--------------+DE                 |
|              BK_LIGHT +--------------+BLK                |
+-----------------------+              +-------------------+
```

## WiFi Setup

The controller automatically connects to:
- **Network**: "RoArm-M2"
- **Password**: "12345678"
- **Robot IP**: 192.168.4.1

## Software Architecture

```
┌─────────────────┐    ┌──────────────┐    ┌────────────────┐
│   Touch Input   │───▶│  LVGL UI     │───▶│  Robot Commands│
│   (Sliders)     │    │  (EEZ Flow)  │    │  (HTTP/JSON)   │
└─────────────────┘    └──────────────┘    └────────────────┘
                              │
                              ▼
                       ┌──────────────┐
                       │ WiFi Manager │
                       └──────────────┘
```

## API Commands

The robot uses HTTP GET requests with URL-encoded JSON:

```bash
# Enable torque
GET /js?json={"T":210,"cmd":1}

# Move base joint to 45°
GET /js?json={"T":101,"joint":1,"rad":0.785,"spd":50,"acc":50}

# LED control (0-255)
GET /js?json={"T":114,"led":128}
```

## Build and Flash

### Prerequisites

- ESP-IDF v5.0 or later
- CMake
- Git

### Setup

```bash
# Clone repository
git clone <your-repo-url>
cd robot_arm_screen

# Set ESP32-S3 target
idf.py set-target esp32s3

# Build project
idf.py build

# Flash and monitor
idf.py -p /dev/ttyUSB0 flash monitor
```

### First Build

The first build downloads dependencies (~101MB) into `components/`:
- LVGL graphics library
- LCD touch drivers
- ESP32-S3 components

## Project Structure

```
robot_arm_screen/
├── main/
│   ├── main.c                 # Application entry point
│   ├── wifi_manager.c/.h      # WiFi connection management
│   ├── robot_arm_comm.c/.h    # Robot HTTP API communication
│   ├── ui_robot_interface.c/.h # UI event handlers
│   ├── screens.c/.h           # LVGL UI screens (EEZ Flow)
│   └── lvgl_port.c/.h         # LVGL porting layer
├── components/                # ESP-IDF components
├── partitions.csv             # Custom 2MB app partition
└── CMakeLists.txt            # Build configuration
```

## Key Features

### Real-time Control
- 50ms UI refresh rate
- Immediate robot response to slider changes
- Percentage-based control (0-100%) mapped to radians

### Connection Management
- Auto-reconnection to RoArm-M2 network
- Connection status monitoring
- UI feedback for network state

### Safety Features
- Software joint limits
- Safe starting positions
- Torque enable/disable control

## Troubleshooting

### Build Issues

**Binary too large**: Uses custom 2MB partition table
**Missing components**: Run `idf.py build` to auto-download

### Runtime Issues

**WiFi connection failed**: Check RoArm-M2 network availability
**Robot not responding**: Verify 192.168.4.1 accessibility
**UI unresponsive**: Check LVGL task and timers



## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

