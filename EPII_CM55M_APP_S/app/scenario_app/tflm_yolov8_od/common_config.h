/*
 * common_config.h
 *
 *  Created on: Nov 22, 2022
 *      Author: bigcat-himax
 */


#ifndef SCENARIO_TFLM_2IN1_FD_FL_PL_COMMON_CONFIG_H_
#define SCENARIO_TFLM_2IN1_FD_FL_PL_COMMON_CONFIG_H_

#define FRAME_CHECK_DEBUG	1
#define EN_ALGO				1
// #define SPI_SEN_PIC_CLK				(10000000)
#define SPI_SEN_PIC_CLK				(12000000)


#define DBG_APP_LOG 0

//current FW image is 409600 bytes => 0x64000. set  0~0x171000 as FW area
#define FW_IMG_SZ							0x3A171000




//0x34B7B000 - correct address for YOLOv8 model (2213248 bytes => ~2.1MB)
#define YOLOV8_OBJECT_DETECTION_FLASH_ADDR 0x34B7B000


#endif /* SCENARIO_TFLM_2IN1_FD_FL_PL_COMMON_CONFIG_H_ */
