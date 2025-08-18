/*
 * cvapp.cpp
 *
 *  Created on: 2022�~11��18��
 *      Author: 902452
 */

#include <cstdio>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "board.h"
#include "cvapp.h"
#include "cisdp_sensor.h"

#include "WE2_core.h"
#include "WE2_device.h"

#include "ethosu_driver.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"

#include "xprintf.h"
#include "cisdp_cfg.h"
#ifdef IP_gpio
#include "hx_drv_gpio.h"
#endif
#include "hx_drv_scu.h"

#include "person_detect_model_data_vela.h"
#include "common_config.h"

// LED Configuration - K2 maps to SEN_D2 which can be configured as GPIO20
#define LED_GPIO_PIN    GPIO20

// PA2 and PA3 GPIO Configuration
#define PA2_GPIO_PIN    SB_GPIO0    // PA2 can be configured as SB_GPIO0
#define PA3_GPIO_PIN    SB_GPIO1    // PA3 can be configured as SB_GPIO1

// LED threshold - 70% confidence for person detection
// With int8 range (-128 to +127), 70% ≈ +55
#define PERSON_DETECTION_THRESHOLD  55

// Counter thresholds for noise filtering
#define PERSON_COUNTER_THRESHOLD    20  // Need 20 consecutive detections to turn LED on
#define COUNTER_DECREMENT_STEP      1   // Decrement by 1 when no person detected

#define LOCAL_FRAQ_BITS (8)
#define SC(A, B) ((A<<8)/B)

#define INPUT_SIZE_X 96
#define INPUT_SIZE_Y 96

#ifdef TRUSTZONE_SEC
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#ifndef TRUSTZONE
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#define U55_BASE	BASE_ADDR_APB_U55_CTRL
#endif
#endif

#define TENSOR_ARENA_BUFSIZE  (125*1024)
__attribute__(( section(".bss.NoInit"))) uint8_t tensor_arena_buf[TENSOR_ARENA_BUFSIZE] __ALIGNED(32);

using namespace std;

namespace {

constexpr int tensor_arena_size = TENSOR_ARENA_BUFSIZE;
uint32_t tensor_arena = (uint32_t)tensor_arena_buf;

struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */
tflite::MicroInterpreter *int_ptr=nullptr;
TfLiteTensor* input, *output;
};


void img_rescale(
        const uint8_t*in_image,
        const int32_t width,
        const int32_t height,
        const int32_t nwidth,
        const int32_t nheight,
        int8_t*out_image,
        const int32_t nxfactor,
        const int32_t nyfactor) {
    int32_t x,y;
    int32_t ceil_x, ceil_y, floor_x, floor_y;

    int32_t fraction_x,fraction_y,one_min_x,one_min_y;
    int32_t pix[4];//4 pixels for the bilinear interpolation
    int32_t out_image_fix;

    for (y = 0; y < nheight; y++) {//compute new pixels
        for (x = 0; x < nwidth; x++) {
            floor_x = (x*nxfactor) >> LOCAL_FRAQ_BITS;//left pixels of the window
            floor_y = (y*nyfactor) >> LOCAL_FRAQ_BITS;//upper pixels of the window

            ceil_x = floor_x+1;//right pixels of the window
            if (ceil_x >= width) ceil_x=floor_x;//stay in image

            ceil_y = floor_y+1;//bottom pixels of the window
            if (ceil_y >= height) ceil_y=floor_y;

            fraction_x = x*nxfactor-(floor_x << LOCAL_FRAQ_BITS);//strength coefficients
            fraction_y = y*nyfactor-(floor_y << LOCAL_FRAQ_BITS);

            one_min_x = (1 << LOCAL_FRAQ_BITS)-fraction_x;
            one_min_y = (1 << LOCAL_FRAQ_BITS)-fraction_y;

            pix[0] = in_image[floor_y * width + floor_x];//store window
            pix[1] = in_image[floor_y * width + ceil_x];
            pix[2] = in_image[ceil_y * width + floor_x];
            pix[3] = in_image[ceil_y * width + ceil_x];

            //interpolate new pixel and truncate it's integer part
            out_image_fix = one_min_y*(one_min_x*pix[0]+fraction_x*pix[1])+fraction_y*(one_min_x*pix[2]+fraction_x*pix[3]);
            out_image_fix = out_image_fix >> (LOCAL_FRAQ_BITS * 2);
            out_image[nwidth*y+x] = out_image_fix-128;
        }
    }
}

static void _arm_npu_irq_handler(void)
{
    /* Call the default interrupt handler from the NPU driver */
    ethosu_irq_handler(&ethosu_drv);
}

/**
 * @brief  Initialises the NPU IRQ
 **/
static void _arm_npu_irq_init(void)
{
    const IRQn_Type ethosu_irqnum = (IRQn_Type)U55_IRQn;

    /* Register the EthosU IRQ handler in our vector table.
     * Note, this handler comes from the EthosU driver */
    EPII_NVIC_SetVector(ethosu_irqnum, (uint32_t)_arm_npu_irq_handler);

    /* Enable the IRQ */
    NVIC_EnableIRQ(ethosu_irqnum);

}

static int _arm_npu_init(bool security_enable, bool privilege_enable)
{
    int err = 0;

    /* Initialise the IRQ */
    _arm_npu_irq_init();

    /* Initialise Ethos-U55 device */
    const void * ethosu_base_address = (void *)(U55_BASE);

    if (0 != (err = ethosu_init(
                            &ethosu_drv,             /* Ethos-U driver device pointer */
                            ethosu_base_address,     /* Ethos-U NPU's base address. */
                            NULL,       /* Pointer to fast mem area - NULL for U55. */
                            0, /* Fast mem region size. */
							security_enable,                       /* Security enable. */
							privilege_enable))) {                   /* Privilege enable. */
    	xprintf("failed to initalise Ethos-U device\n");
            return err;
        }

    xprintf("Ethos-U55 device initialised\n");

    return 0;
}

// GPIO initialization function for LED, PA2, and PA3
static void gpio_init(void)
{
#ifdef IP_gpio
    // Configure pinmux for LED - K2 corresponds to SEN_D2 pin
    // Map SEN_D2 to GPIO20 function
    hx_drv_scu_set_SEN_D2_pinmux(SCU_SEN_D2_PINMUX_GPIO20);
    
    // Configure PA2 pin as SB_GPIO0
    hx_drv_scu_set_PA2_pinmux(SCU_PA2_PINMUX_SB_GPIO0, 1);
    
    // Configure PA3 pin as SB_GPIO1  
    hx_drv_scu_set_PA3_pinmux(SCU_PA3_PINMUX_SB_GPIO1, 1);
    
    // Initialize GPIO Group 4 for SB_GPIO (PA2/PA3)
    hx_drv_gpio_init(GPIO_GROUP_4, HX_GPIO_GROUP_4_BASE);
    
    // Configure all pins as output with initial low state
    hx_drv_gpio_set_output(LED_GPIO_PIN, GPIO_OUT_LOW);
    hx_drv_gpio_set_out_value(LED_GPIO_PIN, GPIO_OUT_LOW);
    
    hx_drv_gpio_set_output(PA2_GPIO_PIN, GPIO_OUT_LOW);
    hx_drv_gpio_set_out_value(PA2_GPIO_PIN, GPIO_OUT_LOW);
    
    hx_drv_gpio_set_output(PA3_GPIO_PIN, GPIO_OUT_LOW);
    hx_drv_gpio_set_out_value(PA3_GPIO_PIN, GPIO_OUT_LOW);
    
    xprintf("GPIO initialized: LED on K2 (SEN_D2->GPIO20), PA2->SB_GPIO0, PA3->SB_GPIO1\n");
#else
    xprintf("GPIO support not enabled\n");
#endif
}

// GPIO control function with noise filtering using counters for LED, PA2, and PA3
static void gpio_control(int8_t person_score)
{
#ifdef IP_gpio
    static int person_counter = 0;      // Counter for consecutive person detections
    static bool gpio_state = false;    // Current GPIO state (LED, PA2, PA3)
    
    if (person_score > PERSON_DETECTION_THRESHOLD) {
        // Person detected with high confidence - increment counter
        person_counter++;
        xprintf("Person detected (score: %d), counter: %d/%d\n", 
                person_score, person_counter, PERSON_COUNTER_THRESHOLD);
        
        // Turn all GPIOs ON only after reaching threshold
        if (person_counter >= PERSON_COUNTER_THRESHOLD && !gpio_state) {
            hx_drv_gpio_set_out_value(LED_GPIO_PIN, GPIO_OUT_HIGH);
            hx_drv_gpio_set_out_value(PA2_GPIO_PIN, GPIO_OUT_HIGH);
            hx_drv_gpio_set_out_value(PA3_GPIO_PIN, GPIO_OUT_HIGH);
            gpio_state = true;
            xprintf("GPIO ON - LED, PA2, PA3 activated (%d consecutive detections)\n", 
                    PERSON_COUNTER_THRESHOLD);
        }
        
        // Cap the counter to avoid overflow
        if (person_counter > PERSON_COUNTER_THRESHOLD) {
            person_counter = PERSON_COUNTER_THRESHOLD;
        }
    } else {
        // No person or low confidence - decrement counter
        if (person_counter > 0) {
            person_counter -= COUNTER_DECREMENT_STEP;
            xprintf("No person (score: %d), counter decreased to: %d\n", 
                    person_score, person_counter);
        }
        
        // Turn all GPIOs OFF when counter reaches 0
        if (person_counter <= 0 && gpio_state) {
            hx_drv_gpio_set_out_value(LED_GPIO_PIN, GPIO_OUT_LOW);
            hx_drv_gpio_set_out_value(PA2_GPIO_PIN, GPIO_OUT_LOW);
            hx_drv_gpio_set_out_value(PA3_GPIO_PIN, GPIO_OUT_LOW);
            gpio_state = false;
            person_counter = 0;  // Ensure counter doesn't go negative
            xprintf("GPIO OFF - LED, PA2, PA3 deactivated (Counter reached 0)\n");
        }
    }
#endif
}

int cv_init(bool security_enable, bool privilege_enable)
{
	int ercode = 0;

	if(_arm_npu_init(security_enable, privilege_enable)!=0)
		return -1;

#if (FLASH_XIP_MODEL == 1)
	static const tflite::Model*model = tflite::GetModel((const void *)0x3A180000);
#else
	static const tflite::Model*model = tflite::GetModel((const void *)g_person_detect_model_data_vela);
#endif

	if (model->version() != TFLITE_SCHEMA_VERSION) {
		xprintf(
			"[ERROR] model's schema version %d is not equal "
			"to supported version %d\n",
			model->version(), TFLITE_SCHEMA_VERSION);
		return -1;
	}
	else {
		xprintf("model's schema version %d\n", model->version());
	}

	static tflite::MicroErrorReporter micro_error_reporter;
	static tflite::MicroMutableOpResolver<1> op_resolver;

	if (kTfLiteOk != op_resolver.AddEthosU()){
		xprintf("Failed to add Arm NPU support to op resolver.");
		return false;
	}

	static tflite::MicroInterpreter static_interpreter(model, op_resolver, (uint8_t*)tensor_arena, tensor_arena_size, &micro_error_reporter);

	if(static_interpreter.AllocateTensors()!= kTfLiteOk) {
		return false;
	}
	int_ptr = &static_interpreter;
	input = static_interpreter.input(0);
	output = static_interpreter.output(0);

	// Initialize GPIO pins (LED, PA2, PA3) for person detection indication
	gpio_init();

	xprintf("initial done\n");

	return ercode;
}

int cv_run() {
	int ercode = 0;
	
	// Timing measurement (commented out due to missing timer function)
	// uint32_t start_time = hx_drv_timer_cm55x_get_current_time_us();
	// static uint32_t frame_count = 0;
	// static uint32_t last_fps_time = 0;

	//give image to input tensor
	img_rescale((uint8_t*)app_get_raw_addr(), app_get_raw_width(), app_get_raw_height(), INPUT_SIZE_X, INPUT_SIZE_Y,
			input->data.int8, SC(app_get_raw_width(), INPUT_SIZE_X), SC(app_get_raw_height(), INPUT_SIZE_Y));

	TfLiteStatus invoke_status = int_ptr->Invoke();

	if(invoke_status != kTfLiteOk)
	{
		xprintf("invoke fail\n");
		return -1;
	}
	else
		xprintf("invoke pass\n");

	//retrieve output data
	int8_t person_score = output->data.int8[1];
	// int8_t no_person_score = output->data.int8[0];  // Unused variable

	xprintf("person_score:%d\n",person_score);
	
	// Control GPIO pins (LED, PA2, PA3) based on person detection
	gpio_control(person_score);
	
	//error_reporter->Report(
	//	   "person score: %d, no person score: %d\n", person_score,
	//	   no_person_score);

	return ercode;
}

int cv_deinit()
{
	//TODO: add more deinit items here if need.
	return 0;
}

