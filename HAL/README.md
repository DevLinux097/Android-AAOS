
# Hướng Dẫn Chi Tiết Xây Dựng GPIO HAL Module Trên AAOS
Trong bài viết này, chúng ta sẽ đi sâu vào từng bước để xây dựng một module HAL điều khiển GPIO trên Android. Ngoài ra, mình sẽ giải thích chi tiết vai trò của các hàm, đường dẫn tới các file, cách biên dịch và chạy test.

## 1. Tổng quan về HAL
HAL là gì? HAL (Hardware Abstraction Layer) là tầng trung gian giữa phần cứng và hệ điều hành. Nó cung cấp các API trừu tượng để tầng ứng dụng hoặc framework có thể giao tiếp với phần cứng mà không cần biết chi tiết cách phần cứng hoạt động.

Trong Android HAL được tổ chức dưới dạng thư viện chia sẻ (shared library). Các thư viện HAL được định nghĩa theo chuẩn và giao tiếp thông qua các cấu trúc dữ liệu và API cụ thể.

## 2. Cấu trúc thư mục dự án
Dưới đây là cấu trúc thư mục của dự án:

    android_source/hardware/libhardware/
    ├── include/hardware
    │   └── mgpio.h         # File header định nghĩa cấu trúc và API cho GPIO HAL
    ├── modules/mgpio
    │   ├── Android.bp      # File build script cho HAL
    │   └── mgpio.c         # File hiện thực HAL
    └── tests
        ├── Android.bp      # File build script cho test
        └── mgpio_hal_test.cpp # File kiểm thử HAL

Vai trò của từng file:
-  **mgpio.h**:
	-  Định nghĩa các API mà HAL cung cấp.
	-  Định nghĩa các cấu trúc dữ liệu (struct) để giao tiếp giữa tầng framework và HAL. Là giao diện chính để tầng framework hoặc ứng dụng sử dụng.

-  **mgpio.c**:
	- Triển khai các API đã định nghĩa trong mgpio.h.
	- Thực hiện các thao tác cụ thể với GPIO thông qua sysfs (/sys/class/gpio/).
-  **Android.bp**:
	- File build script của hệ thống Android để biên dịch HAL và chương trình kiểm thử.
- **mgpio_hal_test.cpp**:
	- Chương trình để test HAL.
	- Sử dụng các API từ HAL để kiểm tra việc điều khiển GPIO.

### 2.1. File mgpio.h - Định nghĩa API
File mgpio.h định nghĩa các API và cấu trúc dữ liệu cần thiết để giao tiếp với HAL.

#### Chi tiết mã nguồn:
```C
#ifndef ANDROID_HARDWARE_GPIO_H
#define ANDROID_HARDWARE_GPIO_H

#include <hardware/hardware.h>

__BEGIN_DECLS

// Unique ID for the GPIO HAL module
#define GPIO_HARDWARE_MODULE_ID "mgpio"

// GPIO direction definitions
#define INPUT   (0) // GPIO set as input
#define OUTPUT  (1) // GPIO set as output

// GPIO value definitions
#define LOW     (0) // GPIO value LOW
#define HIGH    (1) // GPIO value HIGH

// Buffer size definitions
#define BUFFER_MAX      (4)  // Maximum buffer size for GPIO operations
#define DIRECTION_MAX   (35) // Maximum path length for GPIO direction
#define VALUE_MAX       (30) // Maximum path length for GPIO value

// Enumeration for error codes
typedef enum ErrorCode {
    FAILURE = -1, // Operation failed
    SUCCESS       // Operation succeeded
} error_code_e;

// Structure representing a GPIO device
struct gpio_device_t {
    struct hw_device_t common; 						// Common fields for all HAL devices
    int (*gpio_export)(int32_t pin);       			// Export a GPIO pin
    int (*gpio_unexport)(int32_t pin);     			// Unexport a GPIO pin
    int (*gpio_direction)(int32_t pin, int32_t dir);// Set GPIO direction (input/output)
    int (*gpio_read)(int32_t pin);         			// Read value from GPIO pin
    int (*gpio_write)(int32_t pin, int32_t value);	// Write value to GPIO pin
};

// Structure representing the GPIO HAL module
struct gpio_module_t {
    struct hw_module_t common; // Common fields for all HAL modules
};

__END_DECLS

#endif // ANDROID_HARDWARE_GPIO_H
```

#### **Giải thích các thành phần:**
- **#define GPIO_HARDWARE_MODULE_ID "mgpio":**   Đây là ID duy nhất để nhận diện module HAL. Tầng framework sẽ sử dụng ID này để tải module HAL.  
-   **struct gpio_device_t:**
	-  Đại diện cho thiết bị GPIO.
    -   Chứa các hàm để thực hiện các thao tác như  `gpio_export`,  `gpio_direction`,  `gpio_read`, v.v.
-   **struct gpio_module_t:**
    -   Đại diện cho toàn bộ module HAL.
    -   Chứa thông tin chung về module như tên, phiên bản, và các phương thức.

### 2.2. File  mgpio.c
File `mgpio.c` triển khai các API đã định nghĩa trong `mgpio.h`. Các hàm này thực hiện thao tác với GPIO thông qua các file hệ thống Linux (`/sys/class/gpio/`).

#### Chi tiết mã nguồn: `Xem chi tiết ở file mgpio.c`

#### **Giải thích các hàm:**
-  **gpio_export(int32_t pin):**
    -   Export GPIO để sử dụng.
    -   Ghi số chân GPIO vào file  `/sys/class/gpio/export`.
-  **gpio_unexport(int32_t pin):**
    -   Unexport GPIO khi không còn sử dụng.
    -   Ghi số chân GPIO vào file  `/sys/class/gpio/unexport`.
-  **gpio_direction(int32_t pin, int32_t dir):**
    -   Thiết lập hướng GPIO là input hoặc output.
    -   Ghi giá trị  `"in"`  hoặc  `"out"`  vào file `/sys/class/gpio/gpio<pin>/direction`.
-  **gpio_read(int32_t pin):**
    -   Đọc giá trị từ chân GPIO.
    -   Đọc từ file  `/sys/class/gpio/gpio<pin>/value`.
-  **gpio_write(int32_t pin, int32_t value):**
    -   Ghi giá trị vào chân GPIO.
    -   Ghi  `"0"`  hoặc  `"1"`  vào file  `/sys/class/gpio/gpio<pin>/value`.

### 2.2. File mgpio_hal_test.cpp
File `mgpio_hal_test.cpp` là chương trình kiểm thử HAL. Sử dụng các API từ HAL để kiểm tra việc điều khiển GPIO.

#### Chi tiết mã nguồn: 

```C
#include <hardware/mgpio.h>
#include <stdio.h>

// Main test function
int main() {
    struct gpio_module_t *module = NULL; // Pointer to the GPIO module
    struct gpio_device_t *device = NULL; // Pointer to the GPIO device

    // Get the GPIO HAL module
    if (hw_get_module(GPIO_HARDWARE_MODULE_ID, (struct hw_module_t const**)&module) != 0) {
        printf("Failed to get GPIO HAL module\n"); // Print error if module cannot be loaded
        return -1;
    }

    // Open the GPIO device
    if (module->common.methods->open((struct hw_module_t *)module, "", (struct hw_device_t **)&device) != 0) {
        printf("Failed to open GPIO device\n"); // Print error if device cannot be opened
        return -1;
    }

    // Perform GPIO operations
    device->gpio_export(181);		// Export GPIO pin 181
    device->gpio_direction(181, 1);	// Set GPIO pin 181 as output
    device->gpio_write(181, 1);		// Write HIGH to GPIO pin 181

    printf("GPIO 181 turned ON successfully!\n"); 
    return 0;
}
```

## 3. Cách biên dịch và chạy kiểm thử
### 3.1. Biên dịch
Sử dụng công cụ `m` của Android để biên dịch:
`m mgpio_hal_test`
Sau khi biên dịch xong, file `mgpio_hal_test` sẽ được tạo ra.
### 3.2. Chạy kiểm thử
- Đẩy chương trình kiểm thử lên thiết bị:
    `adb push mgpio_hal_test /data/local/tmp` 

- Cấp quyền thực thi:
    `adb shell chmod +x /data/local/tmp/mgpio_hal_test` 

- Chạy chương trình kiểm thử:
    `adb shell /data/local/tmp/mgpio_hal_test` 
4.  Quan sát kết quả:
Nếu thành công, GPIO 181 sẽ được bật, và bạn sẽ thấy thông báo  `GPIO 181 turned ON successfully!`.

## 4. Kết luận
Qua bài viết này, bạn đã hiểu cách xây dựng một module HAL để điều khiển GPIO trên Android. Từ việc định nghĩa API, hiện thực HAL, viết script build, đến kiểm thử, bạn đã có cái nhìn tổng quan và chi tiết về cách hoạt động của HAL trong hệ điều hành Android.