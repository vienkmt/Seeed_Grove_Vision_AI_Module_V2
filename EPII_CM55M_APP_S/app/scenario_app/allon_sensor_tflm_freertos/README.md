# allon_sensor_tflm_freertos

## Overview

Person detection application with FreeRTOS multitasking and advanced power management for battery-powered applications.

## Features

- **Chức năng**: Person detection với **FreeRTOS multitasking**
- **Ứng dụng**: Smart power management - chụp 10 ảnh rồi sleep 1 giây, lặp lại
- **Tính năng**:
  - Task-based architecture
  - Power management (sleep/wake cycles)
  - Configurable capture count
  - Inter-task communication

## Technical Details

### FreeRTOS Architecture
- **Multi-task system**: Separate tasks for AI processing, data handling, and communication
- **Task scheduling**: Priority-based preemptive scheduling
- **Memory management**: Heap and stack management for tasks
- **Synchronization**: Queues, semaphores, and mutexes for task coordination

### Power Management
- **Smart sleep cycles**: Configurable sleep/wake patterns
- **Battery optimization**: Reduced power consumption during idle periods
- **Wake-up triggers**: Timer-based and event-driven wake-up
- **Power states**: Active, sleep, and deep sleep modes

## Configuration

You can customize the behavior by modifying settings in `common_config.h`:

```c
#define SUPPORT_FATFS               0       // 0 : send images via SPI, 1 : save images to SD card
#define ENTER_SLEEP_MODE			0		// 0 : always on, 1 : enter Sleep mode
#define SENSOR_AE_STABLE_CNT		10      // Number of images to capture before sleep
#define ENTER_PMU_MODE_FRAME_CNT	3       // Images to capture after wake-up
```

### Configuration Parameters

| Parameter | Values | Description |
|-----------|--------|-------------|
| **SUPPORT_FATFS** | 0 / 1 | (0) send images via SPI, (1) save images to SD card |
| **ENTER_SLEEP_MODE** | 0 / 1 | (0) always on, (1) enter Sleep mode |
| **SENSOR_AE_STABLE_CNT** | 1-255 | How many images to capture before entering sleep mode |
| **ENTER_PMU_MODE_FRAME_CNT** | 1-255 | After waking up, capture how many images before sleep |

## Output

### Serial Console (UART)
- **Person detection results**: `person_score:X` where X is confidence level
  - Positive values (+): Person detected with high confidence
  - Negative values (-): No person detected
  - Range: -128 to +127 (int8)
- **Task status**: FreeRTOS task execution and scheduling information
- **Power events**: Sleep/wake cycle notifications
- **System health**: Memory usage, task performance metrics

### Power Management Modes

#### Always On Mode (`ENTER_SLEEP_MODE = 0`)
- Continuous operation without sleep
- Maximum responsiveness
- Higher power consumption
- Suitable for AC-powered applications

#### Sleep Mode (`ENTER_SLEEP_MODE = 1`)
- **Capture Phase**: Take `SENSOR_AE_STABLE_CNT` images
- **Sleep Phase**: Enter low-power mode for 1 second
- **Wake-up Phase**: Resume and take `ENTER_PMU_MODE_FRAME_CNT` images
- **Cycle**: Repeat indefinitely

## Build Configuration

This application uses FreeRTOS and requires specific OS selection:

```makefile
# In main makefile
OS_SEL = freertos
APP_TYPE = allon_sensor_tflm_freertos
```

## Usage

### Setup
1. **Build**: Set `APP_TYPE = allon_sensor_tflm_freertos` in main makefile
2. **Configure**: Modify settings in `common_config.h` as needed
3. **Flash**: Follow standard firmware flashing procedure
4. **Monitor**: Connect serial terminal at 921600 baud

### Power Optimization
- **Battery Mode**: Enable `ENTER_SLEEP_MODE = 1` for maximum battery life
- **Capture Count**: Adjust `SENSOR_AE_STABLE_CNT` based on application needs
- **Wake Frequency**: Balance between responsiveness and power consumption

## Camera Support

Supports multiple camera sensors (configure in makefile):
- IMX219: `CIS_SUPPORT_INAPP_MODEL = cis_imx219`
- IMX477: `CIS_SUPPORT_INAPP_MODEL = cis_imx477`
- OV5647: `CIS_SUPPORT_INAPP_MODEL = cis_ov5647`
- HM0360: `CIS_SUPPORT_INAPP_MODEL = cis_hm0360`

## Applications

### Battery-Powered Systems
- **Security Cameras**: Long-term surveillance with power optimization
- **Wildlife Monitoring**: Remote deployment with battery constraints
- **IoT Sensors**: Smart building occupancy detection
- **Portable Devices**: Handheld or wearable AI cameras

### Smart Home Integration
- **Presence Detection**: Automated lighting and HVAC control
- **Security Alerts**: Motion-triggered notifications
- **Energy Management**: Occupancy-based power saving
- **Privacy Protection**: Local processing without cloud dependency

## Performance Characteristics

- **Power Consumption**: 
  - Active: ~200-300mA
  - Sleep: ~10-50mA (depending on sleep depth)
- **Response Time**: 
  - Wake-up latency: ~100-200ms
  - Detection latency: ~50-100ms per frame
- **Battery Life**: 
  - Depends on sleep/wake ratio and battery capacity
  - Example: 2000mAh battery, 10 images/minute → ~20-30 hours

## Troubleshooting

### Common Issues
- **Task stack overflow**: Increase task stack sizes in FreeRTOS config
- **Memory allocation**: Monitor heap usage and adjust as needed
- **Sleep/wake issues**: Check power management configuration
- **Performance degradation**: Balance task priorities and CPU usage