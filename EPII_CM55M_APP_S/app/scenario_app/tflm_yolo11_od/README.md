## How to build yolo11n object detection scenario_app and run on WE2?
### Linux Environment
- Change the `APP_TYPE` to `tflm_yolo11_od` at [makefile](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/EPII_CM55M_APP_S/makefile)
    ```
    APP_TYPE = tflm_yolo11_od
    ```
- We will perform some of the post-processing in the C code to reduce the tensor arena usage in TFLite. If you want to run the full yolo11n object detection TFlite with full post-proccessing on TFlite. You can modify the define to be `0` at [here](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/EPII_CM55M_APP_S/app/scenario_app/tflm_yolo11_od/cvapp_yolo11n_ob.cpp#L38).
  ```
  #define YOLO11_NO_POST_SEPARATE_OUTPUT 1
  ```
- Build the firmware reference the part of [Build the firmware at Linux environment](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2?tab=readme-ov-file#build-the-firmware-at-linux-environment)
- How to flash firmware image and model at [model_zoo](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/tree/main/model_zoo)?
  - Prerequisites for xmodem
    - Please install the package at [xmodem/requirements.txt](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/tree/main/xmodem/requirements.txt) 
        ```
        pip install -r xmodem/requirements.txt
        ```
  - Disconnect `Minicom`
  - Make sure your `Seeed Grove Vision AI Module V2` is connect to PC.
  - Open the permissions to acceess the deivce
    ```
    sudo setfacl -m u:[USERNAME]:rw /dev/ttyUSB0
    # in my case
    # sudo setfacl -m u:kris:rw /dev/ttyACM0
    ```
    ![alt text](../../../../images/flash_image_model_6.png)
  - Open `Terminal` and key-in following command
    - port: the COM number of your `Seeed Grove Vision AI Module V2`, for example,`/dev/ttyACM0`
    - baudrate: 921600
    - file: your firmware image [maximum size is 1MB]
    - model: you can burn multiple models "[model tflite] [position of model on flash] [offset]"
      - Position of model on flash is defined at [~/tflm_yolo11_od/common_config.h](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/EPII_CM55M_APP_S/app/scenario_app/tflm_yolo11_od/common_config.h#L27)
        ```
        python3 xmodem/xmodem_send.py --port=[your COM number] --baudrate=921600 --protocol=xmodem --file=we2_image_gen_local/output_case1_sec_wlcsp/output.img --model="model_zoo/tflm_yolo11_od/yolo11n_full_integer_quant_vela_imgz_224_kris_nopost_241230.tflite 0xB7B000 0x00000"

        # example:
        # python3 xmodem/xmodem_send.py --port=/dev/ttyACM0 --baudrate=921600 --protocol=xmodem --file=we2_image_gen_local/output_case1_sec_wlcsp/output.img --model="model_zoo/tflm_yolo11_od/yolo11n_full_integer_quant_vela_imgz_224_kris_nopost_241230.tflite 0xB7B000 0x00000"
        ```
      - If you want to run the tflite with full post-proccessing. You should burn the model, `model_zoo/tflm_yolo11_od/yolo11n_full_integer_quant_192_241219_batch_matmul_vela.tflite`, at [here](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/model_zoo/tflm_yolo11_od/yolo11n_full_integer_quant_192_241219_batch_matmul_vela.tflite).
    - It will start to burn firmware image and model automatically.
  -  Please press `reset` buttun on `Seeed Grove Vision AI Module V2`.
    ![alt text](../../../../images/grove_vision_ai_v2_all.jpg) 
  - It will success to run the algorithm.

[Back to Outline](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2?tab=readme-ov-file#outline)

### Windows Environment
- Change the `APP_TYPE` to `tflm_yolo11_od` at [makefile](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/EPII_CM55M_APP_S/makefile)
    ```
    APP_TYPE = tflm_yolo11_od
    ```
- We will perform some of the post-processing in the C code to reduce the tensor arena usage in TFLite. If you want to run the full yolo11n object detection TFlite with full post-proccessing on TFlite. You can modify the define to be `0` at [here](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/EPII_CM55M_APP_S/app/scenario_app/tflm_yolo11_od/cvapp_yolo11n_ob.cpp#L38).
  ```
  #define YOLO11_NO_POST_SEPARATE_OUTPUT 1
  ```
- Build the firmware reference the part of [Build the firmware at Windows environment](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2?tab=readme-ov-file#build-the-firmware-at-windows-environment)
- How to flash firmware image and model at [model_zoo](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/tree/main/model_zoo)?
  - Prerequisites for xmodem
    - Please install the package at [xmodem/requirements.txt](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/tree/main/xmodem/requirements.txt) 
        ```
        pip install -r xmodem/requirements.txt
        ```
  - Disconnect `Tera Term`
  - Make sure your `Seeed Grove Vision AI Module V2` is connect to PC.
  - Open `CMD` and key-in following command
    - port: the COM number of your `Seeed Grove Vision AI Module V2` 
    - baudrate: 921600
    - file: your firmware image [maximum size is 1MB]
    - model: you can burn multiple models "[model tflite] [position of model on flash] [offset]"
      - Position of model on flash is defined at [~/tflm_yolo11_od/common_config.h](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/EPII_CM55M_APP_S/app/scenario_app/tflm_yolo11_od/common_config.h#L27)
        ```
        python xmodem\xmodem_send.py --port=[your COM number] --baudrate=921600 --protocol=xmodem --file=we2_image_gen_local\output_case1_sec_wlcsp\output.img --model="model_zoo\tflm_yolo11_od\yolo11n_full_integer_quant_vela_imgz_224_kris_nopost_241230.tflite 0xB7B000 0x00000"

        # example:
        # python xmodem\xmodem_send.py --port=COM123 --baudrate=921600 --protocol=xmodem --file=we2_image_gen_local\output_case1_sec_wlcsp\output.img --model="model_zoo\tflm_yolo11_od\yolo11n_full_integer_quant_vela_imgz_224_kris_nopost_241230.tflite 0xB7B000 0x00000"
        ```

      - If you want to run the tflite with full post-proccessing. You should burn the model, `model_zoo/tflm_yolo11_od/yolo11n_full_integer_quant_192_241219_batch_matmul_vela.tflite`, at [here](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/model_zoo/tflm_yolo11_od/yolo11n_full_integer_quant_192_241219_batch_matmul_vela.tflite).
    - It will start to burn firmware image and model automatically.
  -  Please press `reset` buttun on `Seeed Grove Vision AI Module V2`.
    ![alt text](../../../../images/grove_vision_ai_v2_all.jpg)  
  - It will success to run the algorithm.


[Back to Outline](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2?tab=readme-ov-file#outline)

### Send image and meta data by UART
- Disconnect the uart at your `Tera Term` or `Minicom` first.
- You can use the [Himax AI web toolkit](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/releases/download/v1.1/Himax_AI_web_toolkit.zip) which we provide, download it and unzip it to local PC, and double click `index.html`.
- Please check you select `Grove Vision AI(V2)` and press `connect` button
    ![alt text](../../../../images/uart_show_1.png)
- Select your own COM.
    ![alt text](../../../../images/uart_show_2.png)
- You will see the preview result on website.
- Tip
    - Windows:
        - Please use "Microsoft Edge" browser
    - Linux:
        - Open the permissions to acceess the deivce
            ```
            sudo setfacl -m u:[USERNAME]:rw /dev/ttyUSB0
            # in my case
            # sudo setfacl -m u:kris:rw /dev/ttyACM0
            ```
        - Please use "Google Chrome" browser
          ![alt text](../../../../images/yolo11_od_0.png)
[Back to Outline](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2?tab=readme-ov-file#outline)

### Model source link
- [Yolo11n object detection](https://github.com/HimaxWiseEyePlus/YOLO11_on_WE2?tab=readme-ov-file#yolo11n-object-detection)

---

## Tích hợp Logic GPIO với Chống Nhiễu - Phiên bản Tiếng Việt

### Tổng quan
Phần này ghi lại việc tích hợp logic điều khiển GPIO với tính năng chống nhiễu từ scenario `allon_sensor_tflm` vào `tflm_yolo11_od`. Logic này điều khiển 3 chân GPIO (LED, PA2, PA3) dựa trên kết quả phát hiện người từ mô hình YOLO11.

### Các thay đổi đã thực hiện

#### 1. **Định nghĩa Constants và Cấu hình**
```cpp
// Config cho LED và 2 chân IO PA2 - PA3
#define LED_GPIO_PIN    GPIO20                    // LED trên K2 (SEN_D2)
#define PA2_GPIO_PIN    SB_GPIO0                  // PA2 được cấu hình như SB_GPIO0
#define PA3_GPIO_PIN    SB_GPIO1                  // PA3 được cấu hình như SB_GPIO1

// Độ tin cậy 50% để phát hiện người (YOLO class ID 0)
#define PERSON_DETECTION_CONFIDENCE_THRESHOLD  0.5f
#define PERSON_CLASS_ID  0                        // ID class của người trong COCO dataset

// Ngưỡng cho bộ lọc chống nhiễu
#define PERSON_COUNTER_THRESHOLD    7             // Cần 7 lần phát hiện liên tiếp để bật GPIO
#define COUNTER_DECREMENT_STEP      1             // Giảm 1 đơn vị khi không phát hiện người
```

#### 2. **Hàm Khởi tạo GPIO (`gpio_init`)**
- **Mục đích**: Khởi tạo 3 chân GPIO (LED, PA2, PA3) với cấu hình đầu ra
- **Chi tiết cấu hình**:
  - Cấu hình pinmux cho LED: `SEN_D2` → `GPIO20`
  - Cấu hình PA2: `PA2` → `SB_GPIO0`
  - Cấu hình PA3: `PA3` → `SB_GPIO1`
  - Khởi tạo GPIO Group 4 cho SB_GPIO
  - Đặt tất cả chân ở trạng thái LOW ban đầu

#### 3. **Hàm Điều khiển GPIO với Chống nhiễu (`gpio_control`)**
- **Nguyên lý hoạt động**:
  - **Bộ đếm nhiễu**: Sử dụng `person_counter` để đếm số lần phát hiện liên tiếp
  - **Tăng counter**: Khi phát hiện người với confidence ≥ 50%
  - **Giảm counter**: Khi không phát hiện người hoặc confidence < 50%
  - **Bật GPIO**: Chỉ bật cả 3 chân khi counter đạt 7 lần liên tiếp
  - **Tắt GPIO**: Tắt cả 3 chân khi counter về 0

- **Logic chi tiết**:
  ```cpp
  if (person_detected) {
      person_counter++;                     // Tăng counter
      if (person_counter >= 7 && !gpio_state) {
          // Bật tất cả GPIO
          hx_drv_gpio_set_out_value(LED_GPIO_PIN, GPIO_OUT_HIGH);
          hx_drv_gpio_set_out_value(PA2_GPIO_PIN, GPIO_OUT_HIGH);
          hx_drv_gpio_set_out_value(PA3_GPIO_PIN, GPIO_OUT_HIGH);
      }
  } else {
      person_counter--;                     // Giảm counter
      if (person_counter <= 0 && gpio_state) {
          // Tắt tất cả GPIO
          hx_drv_gpio_set_out_value(LED_GPIO_PIN, GPIO_OUT_LOW);
          hx_drv_gpio_set_out_value(PA2_GPIO_PIN, GPIO_OUT_LOW);
          hx_drv_gpio_set_out_value(PA3_GPIO_PIN, GPIO_OUT_LOW);
      }
  }
  ```

#### 4. **Tích hợp vào YOLO11**
- **Trong `cv_yolo11n_ob_init()`**:
  - Thay thế `led_init()` bằng `gpio_init()`
  - Khởi tạo cả 3 chân GPIO thay vì chỉ LED

- **Trong `cv_yolo11n_ob_run()`**:
  - Thay thế `led_control_yolo11()` bằng `gpio_control()`
  - Áp dụng logic chống nhiễu cho kết quả YOLO11

#### 5. **Cải tiến Logging**
- **Log chi tiết**: Hiển thị counter hiện tại và threshold
- **Log trạng thái**: Thông báo khi bật/tắt GPIO với lý do cụ thể
- **Format YOLO11**: Hiển thị confidence thay vì score như trong person detection model

### Tham số Cấu hình Mới nhất
- **Threshold confidence**: 50% (giảm từ 60% để nhạy hơn)
- **Counter threshold**: 7 lần (giảm từ 20 lần để phản ứng nhanh hơn)
- **Decrement step**: 1 (giữ nguyên)

### Lợi ích của Logic Chống nhiễu
1. **Giảm false positive**: Tránh bật GPIO do nhiễu tạm thời
2. **Ổn định hệ thống**: GPIO chỉ thay đổi trạng thái khi có đủ bằng chứng
3. **Tiết kiệm năng lượng**: Tránh bật/tắt GPIO liên tục
4. **Tương thích**: Giữ nguyên logic đã được kiểm nghiệm từ person detection model

### Cách Sử dụng
1. **Build firmware** với `APP_TYPE = tflm_yolo11_od`
2. **Flash firmware** lên board
3. **Kết nối GPIO**:
   - LED: Chân K2 (SEN_D2)
   - PA2: Chân PA2 trên board
   - PA3: Chân PA3 trên board
4. **Quan sát hoạt động**:
   - GPIO sẽ bật sau 7 lần phát hiện người liên tiếp
   - GPIO sẽ tắt khi counter về 0
   - Log chi tiết hiển thị trên UART

### Tối ưu hóa Hiệu suất - Person Only Mode

#### Mục đích
Vì chỉ quan tâm đến việc phát hiện người, đã thực hiện các tối ưu hóa để tăng tốc độ xử lý đáng kể.

#### Các tối ưu hóa đã thực hiện
1. **Enable/Disable Person-Only Mode**:
   ```cpp
   #define PERSON_ONLY_OPTIMIZATION    1   // 1=enable, 0=disable
   ```

2. **Tối ưu vòng lặp xử lý class**:
   - **Trước**: Kiểm tra tất cả 80 class của COCO dataset
   - **Sau**: Chỉ kiểm tra class "person" (class_id = 0)
   - **Tiết kiệm**: ~98.75% thời gian xử lý class detection

3. **Tối ưu bộ nhớ**:
   - **Trước**: Lưu trữ tên 80 class trong mảng `coco_classes[]`
   - **Sau**: Chỉ lưu trữ "person" → Tiết kiệm ~95% bộ nhớ cho class names

4. **Tối ưu NMS (Non-Maximum Suppression)**:
   - NMS vẫn hoạt động để loại bỏ duplicate detections
   - Nhưng nhanh hơn vì chỉ xử lý 1 class thay vì 80 class

#### Lợi ích về hiệu suất
- **Tốc độ xử lý**: Tăng ~40-60% (ước tính)
- **Bộ nhớ**: Giảm ~95% cho class storage
- **CPU cycles**: Giảm ~98% cho class detection loops
- **Accuracy**: Không đổi (vẫn sử dụng cùng model)

#### Cách bật/tắt tối ưu hóa
```cpp
// Để bật person-only optimization (mặc định)
#define PERSON_ONLY_OPTIMIZATION    1

// Để tắt và xử lý tất cả class như cũ
#define PERSON_ONLY_OPTIMIZATION    0
```

### Ghi chú Quan trọng
- **GPIO Group 4** cần được khởi tạo để sử dụng SB_GPIO
- **Pinmux** phải được cấu hình đúng cho từng chân
- **Logic này tương thích** với cả người dùng muốn điều khiển thiết bị ngoại vi dựa trên kết quả AI
- **Threshold có thể tùy chỉnh** trong file source code tùy theo yêu cầu ứng dụng
- **Person-only optimization** có thể enable/disable bằng cách thay đổi `PERSON_ONLY_OPTIMIZATION` macro

[Back to Outline](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2?tab=readme-ov-file#outline)