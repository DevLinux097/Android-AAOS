# Hướng Dẫn Thêm Kernel Module GPIO Đơn Giản vào Raspberry Pi 4 (Android 15)

Bài viết này hướng dẫn chi tiết cách tạo và thêm một kernel module đơn giản vào Raspberry Pi 4 chạy Android 15. Module này sẽ điều khiển GPIO 27, đặt mức logic cao (HIGH) khi module được nạp và mức logic thấp (LOW) khi module được gỡ bỏ.

## **1. Tổng quan**

Kernel module là một phần của kernel, có thể được nạp (load) và gỡ bỏ (unload) động mà không cần khởi động lại hệ thống.  Module `mgpio` này sẽ:

1.  Yêu cầu (request) và cấu hình GPIO 27 làm đầu ra (output).
2.  Đặt GPIO 27 lên mức cao (HIGH) khi module được nạp.
3.  Đặt GPIO 27 xuống mức thấp (LOW) khi module được gỡ bỏ.
4.  In ra thông báo trạng thái của GPIO 27.

## **2. Cấu trúc thư mục dự án**

```sh
rpi_kernel/common/drivers/mgpio/
                            ├── Makefile    # File Makefile để biên dịch module
                            └── mgpio.c     # Mã nguồn của kernel module
```

Vai trò của từng file:
- **`Makefile`**:  Chứa các lệnh để biên dịch module `mgpio.c` thành file `mgpio.o`.
- **`mgpio.c`**:  Mã nguồn của kernel module, sử dụng các API của kernel để tương tác với GPIO.

## **3. Chi tiết mã nguồn**
### **3.1. File `mgpio.c`**
#### Chi tiết mã nguồn:

```c
#include <linux/module.h>
#include <linux/gpio.h>

#define GPIO_NUMBER_27  27
#define LOW     0
#define HIGH    1

static int __init mgpio_driver_init(void)
{
    // Configure GPIO27 as output
    gpio_request(GPIO_NUMBER_27, "gpio_27");
    gpio_direction_output(GPIO_NUMBER_27, LOW);

    // Set GPIO27 to HIGH
    gpio_set_value(GPIO_NUMBER_27, HIGH);
    pr_info("GPIO27 set to HIGH, status: %d!\n", gpio_get_value(GPIO_NUMBER_27));

    return 0;
}

static void __exit mgpio_driver_exit(void)
{
    // Set GPIO27 to LOW before exiting
    gpio_set_value(GPIO_NUMBER_27, LOW);
    gpio_free(GPIO_NUMBER_27);
    pr_info("GPIO27 set to LOW\n");
}

module_init(mgpio_driver_init);
module_exit(mgpio_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DevLinux");
MODULE_DESCRIPTION("Interger GPIO Driver for Raspberry Pi 4 (BCM2711)");
```

#### Giải thích:
- `mgpio_driver_init`: Hàm khởi tạo module. Hàm này được gọi khi module được nạp vào kernel.
- `mgpio_driver_exit`: Hàm kết thúc module. Hàm này được gọi khi module bị gỡ bỏ khỏi kernel.
- `module_init`, `module_exit`: Các macro này đăng ký các hàm mgpio_driver_init và mgpio_driver_exit là các hàm khởi tạo và kết thúc của module.
- `MODULE_LICENSE, MODULE_AUTHOR, MODULE_DESCRIPTION`: Các macro này cung cấp thông tin về module.

### **3.2. File Makefile**
#### Chi tiết mã nguồn:

```sh
EXTRA_CFLAGS = -Wall
obj-y  += mgpio.o
```

#### Giải thích:
- `EXTRA_CFLAGS = -Wall`: Thêm cờ -Wall để bật tất cả các cảnh báo (warnings) trong quá trình biên dịch.
- `obj-y += mgpio.o`: Chỉ định rằng file mgpio.o sẽ được biên dịch và liên kết vào kernel (builtin kernel module).

## **4. Cách biên dịch và chạy**
### **4.1. Biên dịch**
- Trong source Linux, di chuyển đến thư mục drivers.
- Tạo thư mục mgpio và copy toàn bộ source code vào.
```sh
cd rpi_kernel/common/drivers/mgpio
mkdir mgpio
cd mgpio
# Copy toàn bộ source code vào thư mục này
```

- Build toàn bộ kernel.
```sh
tools/bazel build --config=fast --config=stamp //common:rpi4
```

- Kiểm tra file đã được build.
```sh
cd rpi_kernel
ls -l  bazel-bin/common/rpi4/arch/arm64/boot/Image
```

- Thay thế file Image
 ```sh
cp bazel-bin/common/rpi4/arch/arm64/boot/Image android_source/device/brcm/rpi4-kernel/Image
```

- Build lại bootimage
 ```sh
cd android_source
source build/envsetup.sh
lunch aosp_rpi4_car-ap3a-userdebug
make bootimage -j$(nproc)
```

### **4.2. Flash bootimage vào thẻ nhớ**
```sh
sudo dd if=out/target/product/rpi4/boot.img of=/dev/<p1> bs=1M
```

#### Trong đó:
- `out/target/product/rpi4/boot.img`: Đường dẫn đến file boot.img vừa được tạo ra.
- `/dev/<p1>`: Thiết bị đại diện cho phân vùng boot trên thẻ nhớ SD của bạn. Cẩn thận: Xác định chính xác thiết bị này. Sai sót có thể dẫn đến mất dữ liệu. Bạn có thể sử dụng lệnh lsblk để xem danh sách các thiết bị và phân vùng. Ví dụ: /dev/sdb1, /dev/mmcblk0p1,...

### **4.3. Kiểm tra kết quả**
Sau khi flash, hãy kiểm tra lại xem module đã được nạp thành công chưa bằng cách kết nối với thiết bị qua adb shell và sử dụng lệnh lsmod để xem danh sách các module đã được nạp. Bạn cũng có thể kiểm tra trạng thái của GPIO 27

## **5. Kết luận**
Đây là quy trình hoàn chỉnh, từ biên dịch module, tích hợp vào kernel, build boot.img, và flash vào thẻ nhớ trên Android 15.