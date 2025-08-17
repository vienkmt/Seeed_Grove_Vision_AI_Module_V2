# LED Control cho Person Detection

## Tổng quan

Hướng dẫn cấu hình và sử dụng LED chỉ báo kết quả person detection trên Grove Vision AI V2 với vi điều khiển HX6538.

## Hardware Configuration

### LED Connection
- **Chân K2** của HX6538 tương ứng với **SEN_D2** pin
- **GPIO Mapping**: SEN_D2 → GPIO20
- **Logic điều khiển**:
  - `person_score < 0` (âm) → **LED TẮT** (GPIO Low)
  - `person_score > 0` (dương) → **LED BẬT** (GPIO High)

### GPIO Mapping
```c
// K2 corresponds to SEN_D2 which maps to GPIO20
#define LED_GPIO_PIN    GPIO20
```

## Software Implementation

### 1. Pinmux Configuration
```c
// Cấu hình SEN_D2 pin thành GPIO20 function
hx_drv_scu_set_SEN_D2_pinmux(SCU_SEN_D2_PINMUX_GPIO20);
```

### 2. GPIO Initialization
```c
static void led_init(void)
{
#ifdef IP_gpio
    // Configure pinmux for LED - K2 corresponds to SEN_D2 pin
    // Map SEN_D2 to GPIO20 function
    hx_drv_scu_set_SEN_D2_pinmux(SCU_SEN_D2_PINMUX_GPIO20);
    
    // Configure GPIO20 as output
    hx_drv_gpio_set_output(LED_GPIO_PIN, GPIO_OUT_LOW);
    hx_drv_gpio_set_out_value(LED_GPIO_PIN, GPIO_OUT_LOW);
    
    xprintf("LED GPIO initialized on K2 (SEN_D2 -> GPIO20)\\n");
#endif
}
```

### 3. LED Control Logic
```c
static void led_control(int8_t person_score)
{
#ifdef IP_gpio
    if (person_score > 0) {
        // Person detected - Turn ON LED
        hx_drv_gpio_set_out_value(LED_GPIO_PIN, GPIO_OUT_HIGH);
        xprintf("LED ON - Person detected (score: %d)\\n", person_score);
    } else {
        // No person - Turn OFF LED  
        hx_drv_gpio_set_out_value(LED_GPIO_PIN, GPIO_OUT_LOW);
        xprintf("LED OFF - No person (score: %d)\\n", person_score);
    }
#endif
}
```

## Implementation Steps

### 1. Code Modifications

**File: `cvapp.cpp`**

#### A. Thêm includes và defines:
```cpp
#ifdef IP_gpio
#include "hx_drv_gpio.h"
#endif
#include "hx_drv_scu.h"

// LED Configuration - Assuming K2 maps to AON_GPIO1
#define LED_GPIO_PIN    AON_GPIO1
```

#### B. Thêm LED functions:
- `led_init()` - Khởi tạo GPIO cho LED
- `led_control(int8_t person_score)` - Điều khiển LED dựa trên score

#### C. Gọi functions trong flow:
- `led_init()` trong `cv_init()`
- `led_control(person_score)` trong `cv_run()` sau khi có kết quả inference

### 2. Build Configuration

Đảm bảo GPIO driver được enable trong makefile:
```makefile
# Trong drv_user_defined.mk hoặc scenario makefile
IP_INST_gpio = y
```

### 3. Hardware Verification

#### Pin Assignment:
- **K2 Physical Pin** → **SEN_D2** → **GPIO20**
- **Pinmux Options for SEN_D2**:
  1. `SCU_SEN_D2_PINMUX_SEN_D2_0` - Camera data line (default)
  2. `SCU_SEN_D2_PINMUX_SEN_D2_1` - Camera data line
  3. `SCU_SEN_D2_PINMUX_GPIO20` - GPIO function (for LED)

#### Example Hardware Layout:
```
HX6538        LED Circuit
K2/SEN_D2 ────────[LED]────[Resistor]────GND
(GPIO20)             ↑
                (Active High)
```

## Usage Example

### Expected Output:
```
LED GPIO initialized on K2 (SEN_D2 -> GPIO20)
person_score:-15
LED OFF (K2/SEN_D2) - No person (score: -15)
person_score:115
LED ON (K2/SEN_D2) - Person detected (score: 115)
person_score:112  
LED ON (K2/SEN_D2) - Person detected (score: 112)
person_score:-8
LED OFF (K2/SEN_D2) - No person (score: -8)
```

### Real-time Behavior:
- **Không có người**: LED tắt, person_score âm (-128 đến -1)
- **Có người**: LED bật, person_score dương (+1 đến +127)
- **Threshold**: score = 0 là ngưỡng bật/tắt

## Troubleshooting

### Common Issues:

1. **LED không bật/tắt**:
   - Kiểm tra GPIO pinmux configuration
   - Verify chân K2 mapping trong hardware
   - Check `IP_gpio` enabled trong build

2. **GPIO initialization failed**:
   - Đảm bảo `#ifdef IP_gpio` có hiệu lực
   - Kiểm tra pinmux function có đúng

3. **LED logic ngược**:
   - Kiểm tra hardware: Active High vs Active Low
   - Điều chỉnh `GPIO_OUT_HIGH/LOW` cho phù hợp

### Debug Commands:
```c
// Thêm debug info
xprintf("GPIO init status: %d\\n", status);
xprintf("Current GPIO value: %d\\n", gpio_value);
```

## Advanced Configuration

### Multiple LEDs:
```c
#define LED_PERSON_PIN     AON_GPIO1  // Green LED - Person detected
#define LED_NO_PERSON_PIN  AON_GPIO0  // Red LED - No person
```

### PWM Brightness Control:
- Sử dụng PWM driver thay vì simple GPIO
- Điều chỉnh brightness theo confidence level

### Threshold Customization:
```c
#define PERSON_DETECTION_THRESHOLD  50  // Custom threshold thay vì 0
if (person_score > PERSON_DETECTION_THRESHOLD) {
    // LED ON
}
```

## Performance Notes

- **GPIO operations**: ~1-2μs execution time
- **LED response**: Immediate (same frame as detection)
- **Power consumption**: ~2-20mA depending on LED
- **No impact** on AI inference performance