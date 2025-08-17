# allon_sensor_tflm_fatfs

## Overview

Person detection application with SD card storage capability for image logging and data collection.

## Features

- **Chức năng**: Person detection + **lưu ảnh vào thẻ SD**
- **Ứng dụng**: Thu thập và lưu trữ hình ảnh với AI detection, phù hợp cho data logging
- **Tính năng**: Ghi hình ảnh captured vào SD card thông qua FAT file system

## Technical Details

### SD Card Storage
- **File System**: FAT32 via FatFS middleware
- **Storage Format**: JPEG images with metadata
- **File Naming**: Sequential numbering with timestamps
- **Storage Location**: Root directory of SD card

### Data Logging Capabilities
- **Image Storage**: Captured frames saved as JPEG files
- **AI Results**: Person detection scores logged with each image
- **Metadata**: Timestamp, frame number, detection confidence
- **Automatic Management**: File rotation and storage optimization

## Hardware Requirements

### SD Card Setup
- **SD Card**: Class 10 or higher recommended
- **Format**: FAT32 file system
- **Capacity**: 8GB+ recommended for extensive logging
- **Interface**: SPI connection to Grove Vision AI V2

### Connection
```
SD Card → Grove Vision AI V2
- CS   → Configured SPI CS pin
- MOSI → SPI MOSI  
- MISO → SPI MISO
- SCK  → SPI Clock
- VCC  → 3.3V
- GND  → Ground
```

## Output

### Serial Console (UART)
- **Person detection results**: `person_score:X` where X is confidence level
  - Positive values (+): Person detected with high confidence
  - Negative values (-): No person detected
  - Range: -128 to +127 (int8)
- **File operations**: SD card write status and errors
- **Storage info**: Available space, file count

### SD Card Files
- **Images**: `img_XXXX.jpg` (sequential numbering)
- **Log file**: `detection.log` (AI results and metadata)
- **Format example**:
  ```
  Frame: 1234, Timestamp: 12345678, Person_Score: 115, File: img_1234.jpg
  Frame: 1235, Timestamp: 12345679, Person_Score: -15, File: img_1235.jpg
  ```

## Build Configuration

This application includes FatFS support:

```makefile
# In scenario app makefile
SUPPORT_FATFS = 1  # Enable SD card storage
```

## Usage

### Setup
1. **Prepare SD card**: Format as FAT32
2. **Insert SD card** into Grove Vision AI V2
3. **Build**: Set `APP_TYPE = allon_sensor_tflm_fatfs` in main makefile
4. **Flash**: Follow standard firmware flashing procedure

### Operation
1. **Start recording**: Device automatically begins saving images
2. **Monitor**: Check serial console for status messages
3. **Data collection**: Images and detection results saved to SD card
4. **Retrieve data**: Remove SD card to access collected files

## Camera Support

Supports multiple camera sensors (configure in makefile):
- IMX219: `CIS_SUPPORT_INAPP_MODEL = cis_imx219`
- IMX477: `CIS_SUPPORT_INAPP_MODEL = cis_imx477`
- OV5647: `CIS_SUPPORT_INAPP_MODEL = cis_ov5647`
- HM0360: `CIS_SUPPORT_INAPP_MODEL = cis_hm0360`

## Applications

### Data Collection
- **Training Data**: Collect images for model improvement
- **Surveillance**: Long-term monitoring with storage
- **Research**: Data gathering for computer vision research
- **Quality Control**: Automated inspection with logging

### Industrial Use Cases
- **Security Systems**: Store detected person events
- **Occupancy Monitoring**: Log presence data over time
- **Behavior Analysis**: Collect data for pattern recognition
- **Compliance**: Maintain records for audit purposes

## Performance Notes

- **Storage Speed**: Depends on SD card class and size
- **Frame Rate**: May be reduced due to file I/O operations
- **Capacity Planning**: ~10-50KB per JPEG depending on scene complexity
- **Reliability**: Proper SD card ejection recommended

## Troubleshooting

### Common Issues
- **SD Card not detected**: Check formatting (FAT32) and connections
- **Write errors**: Ensure SD card has sufficient free space
- **Slow performance**: Use Class 10+ SD card
- **File corruption**: Properly eject SD card before removal

### Error Messages
- `SD Init fail`: SD card initialization failed
- `File write error`: Storage write operation failed
- `Disk full`: SD card storage capacity exceeded