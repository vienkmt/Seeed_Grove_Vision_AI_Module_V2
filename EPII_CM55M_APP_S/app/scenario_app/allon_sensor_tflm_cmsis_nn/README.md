# allon_sensor_tflm_cmsis_nn

## Overview

Person detection application using TensorFlow Lite Micro with CMSIS-NN optimization for enhanced performance on ARM Cortex-M55.

## Features

- **Chức năng**: Giống allon_sensor_tflm nhưng **tối ưu hóa CMSIS-NN**
- **Ứng dụng**: Person detection với hiệu suất cao hơn, sử dụng thư viện CMSIS-NN cho ARM Cortex-M55
- **Khác biệt**: Tăng tốc độ AI inference thông qua CMSIS-NN kernels

## Technical Details

### CMSIS-NN Optimization
- Utilizes ARM CMSIS-NN library version 7.0.0
- Optimized neural network kernels for ARM Cortex-M55
- Significant performance improvement over standard TensorFlow Lite Micro
- Hardware-accelerated convolution and activation functions

### Performance Benefits
- **Faster inference**: Reduced latency for person detection
- **Lower power consumption**: Optimized operations reduce CPU cycles
- **Better resource utilization**: Efficient use of ARM Cortex-M55 capabilities

## Build Configuration

This application requires CMSIS-NN to be enabled in the main makefile:

```makefile
LIB_CMSIS_NN_ENALBE = 1
LIB_CMSIS_NN_VERSION = 7_0_0
```

## Output

### Serial Console (UART)
- **Person detection results**: `person_score:X` where X is confidence level
  - Positive values (+): Person detected with high confidence
  - Negative values (-): No person detected
  - Range: -128 to +127 (int8)
  - Example: `person_score:115` = ~90% confidence person detected

### SPI Interface
- **JPEG images**: Raw camera frames sent via SPI protocol
- **Data format**: `DATA_TYPE_JPG`
- **SPI pins**: PB2/PB3/PB4/PB11

## Camera Support

Supports multiple camera sensors (configure in makefile):
- IMX219: `CIS_SUPPORT_INAPP_MODEL = cis_imx219`
- IMX477: `CIS_SUPPORT_INAPP_MODEL = cis_imx477` 
- OV5647: `CIS_SUPPORT_INAPP_MODEL = cis_ov5647`
- HM0360: `CIS_SUPPORT_INAPP_MODEL = cis_hm0360`

## Usage

1. **Build**: Set `APP_TYPE = allon_sensor_tflm_cmsis_nn` in main makefile
2. **Flash**: Follow standard firmware flashing procedure
3. **Monitor**: Connect serial terminal at 921600 baud to view results
4. **Test**: Place person in camera view to see positive person_score values

## Comparison with Standard Version

| Feature | allon_sensor_tflm | allon_sensor_tflm_cmsis_nn |
|---------|-------------------|----------------------------|
| AI Library | Standard TFLM | TFLM + CMSIS-NN |
| Performance | Baseline | **~2-3x faster** |
| Power Usage | Baseline | **Lower** |
| Memory Usage | Baseline | Similar |
| Accuracy | Same model | Same model |

## Technical Notes

- Model remains identical (person_detect_model_data.h)
- Input/output format unchanged (96x96 grayscale → int8 scores)
- Real-time processing at camera frame rate
- Hardware NPU acceleration via Ethos-U55