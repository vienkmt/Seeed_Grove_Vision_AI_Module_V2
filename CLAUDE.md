# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is the Himax examples repository for the Seeed Grove Vision AI Module V2, featuring ARM Cortex-M55 microcontroller with embedded AI acceleration. The project includes TensorFlow Lite Micro (TFLM) examples, computer vision applications, Edge Impulse integration, and various sensor interfaces.

## Build System

### Primary Build Commands

The project uses GNU Make as the build system. Always build from the `EPII_CM55M_APP_S` directory:

```bash
cd EPII_CM55M_APP_S
make clean
make
```

### Testing and Validation

No automated test suite is available. Validation is done through:
- Manual firmware compilation and flashing
- Runtime testing on hardware via serial console
- Visual verification of AI model outputs

### Build Configuration

The main build configuration is controlled via `EPII_CM55M_APP_S/makefile`:
- `APP_TYPE`: Select which example application to build (e.g., `allon_sensor_tflm`, `tflm_fd_fm`, `kws_pdm_record`)
- `TOOLCHAIN`: Set to `gnu` (default) or `arm`
- `BOARD`: Set to `epii_evb` (default)
- `IC_PACKAGE_SEL`: Set to `WLCSP65` (default)
- `LIB_CMSIS_NN_ENALBE`: Enable CMSIS-NN library (set to 1)
- `CIS_SEL`: Camera sensor selection (if needed)

### Application Types

Available scenario applications (set via `APP_TYPE`):
- `allon_sensor_tflm` - Basic TensorFlow Lite Micro with sensor
- `allon_sensor_tflm_cmsis_nn` - TFLM with CMSIS-NN optimization
- `tflm_fd_fm` - Face detection and face mesh
- `tflm_yolov8_od` - YOLOv8 object detection
- `tflm_yolov8_pose` - YOLOv8 pose estimation
- `tflm_yolov8_gender_cls` - YOLOv8 gender classification
- `tflm_yolo11_od` - YOLO11 object detection
- `tflm_peoplenet` - PeopleNet from TAO
- `kws_pdm_record` - Keyword spotting with microphone
- `pdm_record` - PDM microphone recording
- `imu_read` - IMU sensor reading
- `hello_world_cmsis_cv` - CMSIS-CV example
- `hello_world_cmsis_dsp` - CMSIS-DSP example
- `ei_standalone_inferencing` - Edge Impulse inferencing
- `ei_standalone_inferencing_camera` - Edge Impulse with camera

### Firmware Image Generation

After building, generate the firmware image:

```bash
cd ../we2_image_gen_local/
cp ../EPII_CM55M_APP_S/obj_epii_evb_icv30_bdv10/gnu_epii_evb_WLCSP65/EPII_CM55M_gnu_epii_evb_WLCSP65_s.elf input_case1_secboot/
./we2_local_image_gen project_case1_blp_wlcsp.json  # Linux
# or
./we2_local_image_gen_macOS_arm64 project_case1_blp_wlcsp.json  # macOS
```

Output firmware: `./output_case1_sec_wlcsp/output.img`

### Flashing

Flash the firmware using Python xmodem tools:

```bash
# Install requirements first
pip install -r xmodem/requirements.txt

# Flash firmware (replace /dev/ttyACM0 with your device)
python3 xmodem/xmodem_send.py --port=/dev/ttyACM0 --baudrate=921600 --protocol=xmodem --file=we2_image_gen_local/output_case1_sec_wlcsp/output.img
```

Alternative: Use Edge Impulse CLI tools:
```bash
himax-flash-tool -d WiseEye2 -f <path_to_firmware_img_file>
```

## Architecture

### Directory Structure

- `EPII_CM55M_APP_S/` - Main application source code and build system
  - `app/` - Application layer with main.c and scenario examples
  - `drivers/` - Hardware abstraction layer and peripheral drivers
  - `library/` - Third-party libraries (TensorFlow Lite Micro, CMSIS, etc.)
  - `os/` - Operating system support (FreeRTOS)
  - `board/` - Board-specific configurations
  - `device/` - Device-specific low-level code
  - `middleware/` - Middleware components (FatFS, etc.)
  - `linker_script/` - Memory layout definitions
  - `external/` - External device drivers (camera sensors)

### Key Libraries

- **TensorFlow Lite Micro**: AI inference engine with NPU acceleration
- **CMSIS-NN**: ARM optimized neural network kernels
- **CMSIS-CV**: Computer vision algorithms
- **CMSIS-DSP**: Digital signal processing functions
- **FreeRTOS**: Real-time operating system
- **Edge Impulse SDK**: For Edge Impulse model deployment

### Hardware Support

- ARM Cortex-M55 CPU with TrustZone
- Himax NPU (Neural Processing Unit) 
- Camera interfaces (OV5647, IMX219, IMX477)
- PDM microphone
- IMU sensors (ICM42688)
- SPI Flash, PSRAM
- Various communication interfaces (I2C, SPI, UART)

## Model Integration

### TensorFlow Lite Models

Models are typically stored as `.tflite` files in `model_zoo/` and compiled into firmware as C arrays. Model placement in flash memory starts after address 0x200000 (first 2MB reserved for firmware).

### Camera Sensor Support

To switch camera sensors, modify the scenario app makefile (not the main makefile):
```makefile
# In scenario app makefile (e.g., allon_sensor_tflm.mk)
CIS_SUPPORT_INAPP_MODEL = cis_imx219  # or cis_imx477, cis_ov5647
```

Supported sensors: OV5647, IMX219, IMX477

### CMSIS-NN Optimization

Enable CMSIS-NN for optimized neural network operations:
```makefile
# In main makefile
LIB_CMSIS_NN_ENALBE = 1
LIB_CMSIS_NN_VERSION = 7_0_0  # or default version
```

## Development Tools

### Prerequisites

- ARM GNU Toolchain 13.2.rel1
- GNU Make
- Python 3 with xmodem and pyserial packages
- Serial terminal (minicom on Linux, TeraTerm on Windows)

### Debugging

SWD debugging support available via PyOCD in `swd_debugging/` directory.

### Memory Layout

Device memory map available in FAQ.md. Key addresses:
- Firmware: 0x000000 - 0x1FFFFF (first 2MB)
- Models: 0x200000+ (4KB aligned)

## Architecture Patterns

### Application Structure

Each scenario app follows a consistent pattern:
- `*.c/*.cpp` - Main application logic and initialization
- `*.h` - Application headers and interface definitions  
- `common_config.h` - Application-specific configuration (memory addresses, debug flags)
- `cvapp*.cpp` - Computer vision processing pipeline
- `*.mk` - Build configuration and dependencies
- `drv_user_defined.mk` - Hardware driver selection
- `hardfault_handler.c` - Error handling for debugging

### Model Integration Pattern

Models are integrated using a standardized approach:
1. Model files placed in `model_zoo/[app_name]/`
2. Flash addresses defined in `common_config.h` (starts at 0x200000+)
3. Model loading handled in `cvapp*.cpp` files
4. Inference pipeline implemented with TensorFlow Lite Micro

### Driver Configuration

Hardware features are configured through:
- Main `makefile` - Global settings (toolchain, board, libraries)
- Scenario `*.mk` - App-specific drivers and features
- `drv_user_defined.mk` - Driver subsystem selection

## Common Tasks

### Adding New Models

1. Convert model to TensorFlow Lite format
2. Use Vela compiler for NPU optimization (if applicable)
3. Place model file in `model_zoo/[app_name]/` directory
4. Define model flash address in `common_config.h` (must be 4KB aligned, ≥0x200000)
5. Update `cvapp*.cpp` to load and use the model
6. Test compilation and runtime behavior

### Creating Custom Applications

1. Copy existing scenario app directory as template (e.g., `allon_sensor_tflm`)
2. Rename files and update `APP_TYPE` in main makefile
3. Modify `common_config.h` for memory layout and debug settings
4. Update `cvapp*.cpp` for AI/CV processing logic
5. Configure camera sensor in scenario app `*.mk` if needed
6. Build, flash, and validate on hardware

### Sensor Integration

- **Camera sensors**: Modify `CIS_SUPPORT_INAPP_MODEL` in scenario app makefile
- **IMU**: Use `imu_read` example as reference, drivers in `drivers/inc/hx_drv_*.h`
- **Microphone**: Use `pdm_record` or `kws_pdm_record` examples
- **Other sensors**: Check `drivers/inc/` for available peripheral drivers

## Working with the Current Application

The current working directory is `tflm_yolo11_od` (YOLO11 object detection). Key files:
- `common_config.h` - Model flash address at 0x3AB7B000
- Model location: `model_zoo/tflm_yolo11_od/yolo11n_*.tflite`
- Main makefile currently set to `APP_TYPE = allon_sensor_tflm` (not yolo11)

To switch to YOLO11 app: Change main makefile `APP_TYPE = tflm_yolo11_od`

## Troubleshooting

- **Build errors**: Check ARM GNU Toolchain 13.2.rel1 in PATH
- **Flash errors**: Verify device permissions and COM port (typically /dev/ttyACM0 on Linux)
- **Model issues**: Ensure 4KB alignment and correct address placement in `common_config.h`
- **Camera issues**: Verify sensor selection in scenario app makefile (not main makefile)
- **Memory issues**: Check flash layout in linker scripts and memory map in FAQ.md