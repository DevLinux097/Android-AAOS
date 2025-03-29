# Hướng Dẫn SELinux Cơ Bản và Tạo Policy Cho Ứng Dụng "hello_world" trên AAOS

Bài viết này hướng dẫn cách tạo một policy SELinux đơn giản cho một ứng dụng "hello_world" trên Raspberry Pi 4 chạy Android.  Chúng ta sẽ đi qua các bước:

1.  **Tổng quan về SELinux:**  Giới thiệu về SELinux và vai trò của nó trong bảo mật Android.
2.  **Tạo ứng dụng "hello_world":**  Tạo một ứng dụng C++ đơn giản.
3.  **Tạo policy SELinux:**  Viết policy để cho phép ứng dụng "hello_world" truy cập vào một file cụ thể.
4.  **Tích hợp policy:**  Thêm policy vào hệ thống build của Android.
5.  **Kiểm tra:**  Xác minh rằng policy hoạt động đúng như mong đợi.
  
## **1. Tổng quan**
SELinux (Security-Enhanced Linux) là một module bảo mật của kernel Linux, cung cấp cơ chế kiểm soát truy cập bắt buộc (Mandatory Access Control - MAC).  Trong Android, SELinux đóng vai trò quan trọng trong việc:

- **`Cô lập ứng dụng`**:  Mỗi ứng dụng chạy trong một "domain" SELinux riêng biệt, hạn chế khả năng truy cập của ứng dụng vào các tài nguyên hệ thống.
- **`Ngăn chặn leo thang đặc quyền`**:  Ngay cả khi một ứng dụng bị xâm nhập, SELinux sẽ ngăn chặn kẻ tấn công truy cập vào các phần khác của hệ thống.
- **`Bảo vệ dữ liệu`**:  SELinux kiểm soát quyền truy cập vào các file, thư mục, socket, và các tài nguyên khác.

SELinux hoạt động dựa trên các `policy`.  Policy là một tập hợp các quy tắc định nghĩa quyền truy cập của các "domain" (thường là các tiến trình) vào các "type" (thường là các file).

## 2. Cấu trúc thư mục dự án
Dưới đây là cấu trúc thư mục của dự án:

```sh
android_source/external/
└── helloworld/
    ├── Android.bp      # File build script cho cả ứng dụng và thư viện
    ├── include/
    │   └── mmath.h     # Mã nguồn phụ, không sử dụng trong demo này
    └── src/
    ├── hello_world.cpp  # Mã nguồn chính của ứng dụng
    └── mmath.cpp        # Mã nguồn phụ, không sử dụng trong demo này
```

```sh
android_source/device/brcm/rpi4/sepolicy/
                            ├── hello_world_policy.te  # File policy SELinux
                            └── file_contexts          # File chứa context cho các file
```

Vai trò của từng file/thư mục:
- **`external/helloworld/`**:  Chứa mã nguồn của ứng dụng "hello_world".
- **`Android.bp`**:  File build script cho Bazel, định nghĩa cách build ứng dụng.
- **`src/hello_world.cpp`**:  Mã nguồn chính, in thông báo và ghi vào file `/data/selinux.txt`.

- **`android/device/brcm/rpi4/sepolicy/`**:  Chứa các file liên quan đến SELinux policy cho Raspberry Pi 4.
- **`hello_world_policy.te`**:  File policy SELinux cho ứng dụng "hello_world".
- **`file_contexts`**:  File chứa các context (nhãn) SELinux cho các file cụ thể.

## **3. Chi tiết mã nguồn**
### **3.1. File `hello_world.cpp`**
#### Chi tiết mã nguồn:
```cpp
#include <iostream>
#include <fstream>
#include <string>

int main() {
    fstream file("/data/selinux.txt", ios::in | ios::out);
    if (!file.is_open()) {
        cout << "Unable to open file!" << endl;
        return 1;
    }

    file << "Hello Selinux\n";
    file.close();
    std::cout << "File reading and writing completed!" << endl;

    return 0;
}
```

#### Giải thích:
- Ứng dụng in ra thông báo và ghi dữ liệu vào file /data/selinux.txt. Việc ghi vào file này là điểm quan trọng để kiểm tra SELinux policy.

### **3.2. File `hello_world_policy.te`**
#### Chi tiết mã nguồn:
```sh
module hello_world_policy 1.0;

require {
    type helloworld_exec;              # Type for the executable.
    type selinux_txt_file;              # Type for the text file.
    class file { read write open };     # Permissions we'll use for files.
}

# Define types and associate them with type attributes.
type hello_world_exec, exec_type;       # hello_world_exec is an executable type.
type selinux_txt_file, file_type;       # selinux_txt_file is a regular file type.

# Set up a standard domain for the hello_world process.
init_daemon_domain(hello_world_exec);

# Allow the hello_world process to read, write, and open the selinux.txt file.
allow hello_world_exec selinux_txt_file:file { read write open };
```

#### Giải thích:
- File này định nghĩa policy SELinux cho ứng dụng "hello_world". Nó khai báo các `type`, `class`, và các quy tắc cho phép ứng dụng truy cập vào file `/data/selinux.txt`.

### **3.2. File `hello_world_policy.te`**
#### Chi tiết mã nguồn:
- Lưu ý thêm các dòng sau vào cuối file.

```sh
# Set up a standard domain for the hello_world process.
init_daemon_domain(hello_world_exec);

# Allow the hello_world process to read, write, and open the selinux.txt file.
allow hello_world_exec selinux_txt_file:file { read write open };
```

#### Giải thích:
- File này gán nhãn SELinux (context) cho các file cụ thể. Nó đảm bảo rằng:
  - File thực thi `/vendor/bin/hello_world` có nhãn `helloworld_exec`.
  - File `/data/selinux.txt` có nhãn `selinux_txt_file`.
  
## **4. Cách biên dịch và chạy**
### **4.1. Biên dịch hello_world**
- Trong source Android, di chuyển đến thư mục external.
- Tạo thư mục helloworld và copy toàn bộ source code vào.
```sh
cd android_source/external
mkdir helloworld
cd helloworld
```

- Build toàn bộ module.
```sh
make hellworld
```

-  Kiểm tra file thực thi hello_world
```sh
ls -l $OUT/vendor/bin/hello_world
```

### **4.2. Cập nhật policy**
- Policy đã được đặt trong `android_source/device/brcm/rpi4/sepolicy/`.
- File `file_contexts` đã được cập nhật.
- Sao chép file hello_world_policy.te vào `android_source/device/brcm/rpi4/sepolicy/`.

- Build systemimage
 ```sh
cd android_source
source build/envsetup.sh
lunch aosp_rpi4_car-ap3a-userdebug
make systemimage -j$(nproc)
```

### **4.3. Flash systemimage vào thẻ nhớ**
```sh
sudo dd if=out/target/product/rpi4/system.img of=/dev/<p2> bs=1M
```

#### Trong đó:
- `out/target/product/rpi4/system.img`: Đường dẫn đến file system.img vừa được tạo ra.
- `/dev/<p2>`: Thiết bị đại diện cho phân vùng boot trên thẻ nhớ SD của bạn. Cẩn thận: Xác định chính xác thiết bị này. Sai sót có thể dẫn đến mất dữ liệu. Bạn có thể sử dụng lệnh lsblk để xem danh sách các thiết bị và phân vùng. Ví dụ: /dev/sdb1, /dev/mmcblk0p1,...

### **4.4. Cài đặt và Chạy**
#### **4.4.1. Cài đặt ứng dụng**
```sh
adb root
adb remount
adb shell touch /data/selinux.txt
chmod 666 /data/selinux.txt
adb push $OUT/vendor/bin/hello_world /vendor/bin/
adb shell chmod 755 /vendor/bin/hello_world
```

#### **4.4.2. TThay đổi policy tạm thời**
```sh
adb shell chcon u:object_r:selinux_txt_file:s0 /data/selinux.txt
adb shell chcon u:object_r:helloworld_exec:s0 /vendor/bin/hello_world
```

#### **4.4.3. Chạy chương trình**
```sh
adb shell /vendor/bin/hello_world
```

- Kiểm tra nội dung file `/data/selinux.txt`
```sh
cat /data/selinux.txt
```

- Nếu thấy dòng chữ "Data written by hello_world.", policy hoạt động đúng.
- Kiểm tra log SELinux (nếu cần): `dmesg | grep avc`

## 5. Kết Luận
Bài viết đã hướng dẫn cách tạo một policy SELinux đơn giản, build và kiểm tra ứng dụng trên Android (Raspberry Pi 4). Đây là bước khởi đầu quan trọng để hiểu và làm chủ SELinux trong Android.