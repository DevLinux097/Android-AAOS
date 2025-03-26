# Hướng Dẫn Chi Tiết Xây Dựng GPIO AIDL Module Trên AAOS 15

## 1. Tổng quan về AIDL

## 2. Cấu trúc thư mục dự án
Dưới đây là cấu trúc thư mục của dự án:

```sh
android_source/hardware/interfaces/mgpio
├── aidl                                # Thư mục chứa định nghĩa giao diện AIDL
│   └── android
│   │   │   └── hardware
│   │   │       └── mgpio               # Gói (package) AIDL cho mgpio
│   │   │           ├── Direction.aidl  # Định nghĩa enum Direction (hướng: INPUT, OUTPUT)
│   │   │           ├── IMgpio.aidl     # Định nghĩa giao diện IMgpio (các phương thức như readValue, writeValue,...)
│   │   │           ├── Status.aidl     # Định nghĩa enum Status (trạng thái: SUCCESS, FAILURE)
│   │   │           └── Voltage.aidl    # (Có thể không cần thiết) Định nghĩa kiểu Voltage (nếu cần)
│   │   └── default                     # Cấu hình mặc định cho việc triển khai HIDL/binderized HAL
│   │   │   ├── Android.bp              # File build script cho phần triển khai HAL (C++)
│   │   │   ├── Mgpio.cpp               # Hiện thực các phương thức của giao diện IMgpio (phần logic chính)
│   │   │   ├── Mgpio.h                 # File header cho Mgpio.cpp (khai báo class, các phương thức,...)
│   │   │   ├── mgpio.rc                # File cấu hình init script (khởi động service khi boot)
│   │   │   ├── mgpio.xml               # File manifest (khai báo service với hệ thống)
│   │   │   └── service.cpp             # File main, tạo và đăng ký service (binder)
│   │   └── tests                       # Thư mục chứa mã nguồn kiểm thử (tests)
│   │       ├── Android.bp              # Tệp tin cấu hình build cho phần kiểm thử
│   │       └── mgpio_aidl_test.cpp     # Mã nguồn kiểm thử
│   └────── Android.bp                  # File build script (blueprint) cho phần AIDL (biên dịch các file .aidl)
```

Vai trò của từng file:
- **IMgpio.aidl**
  - Định nghĩa các interfaces dược client sử dụng.

- **Direction.aidl**
  - Định nghĩa hai giá trị: INPUT và OUTPUT.

- **Status.aidl**
  - Định nghĩa hai giá trị: SUCCESS và FAILURE.

- **Voltage.aidl**
  - (Tùy chọn) Định nghĩa kiểu dữ liệu Voltage (nếu cần).

- **android_source/hardware/interfaces/mgpio/aidl/Android.bp**
  - Chỉ định các file .aidl cần được biên dịch.
  - Cấu hình để tạo ra các thư viện Java và C++ từ các file .aidl.
  - Các thư viện này sẽ được sử dụng bởi cả service (phần C++) và client (phần Java).

- **android_source/hardware/interfaces/mgpio/aidl/android/default/Android.bp**
  - Chỉ định các file nguồn C++ (Mgpio.cpp, service.cpp) cần được biên dịch.
  - Tạo ra một shared library (thường là .so) cho service.

- **Mgpio.h**
  - Khai báo các phương thức của class Mgpio.

- **Mgpio.cpp**
  - Triển khai mã nguồn cụ thể cho các phương thức trong class Mgpio.

- **mgpio.rc**
  - Chỉ định service mgpio được khởi động khi hệ thống boot.
  - Cấu hình các thông tin liên quan đến service (ví dụ, user, group, quyền, capabilities).

- **mgpio.xml**
  - File manifest (khai báo service).

- **service.cpp**
  - File main của service. Đăng ký đối tượng này với Service Manager (sử dụng defaultServiceManager()->addService(...)). Service Manager là một thành phần trung tâm của Android, quản lý các service binder.
  - Tham gia vào vòng lặp binder (thường là ProcessState::self()->startThreadPool(); IPCThreadState::self()->joinThreadPool();). Điều này cho phép service xử lý các yêu cầu từ client một cách liên tục.

## 2.2. Mã nguồn dự án
`Xem chi tiết ở cây thư mục trên`

## 3. Cách biên dịch và chạy kiểm thử
### 3.1. Biên dịch AIDL module

```sh
m libmgpio
m android.hardware.mgpio-service
```

Kiểm tra file đã được build

```sh
ls -l $OUT/vendor/lib64/hw/mgpio.default.so
```

### 3.1. Biên dịch tests module

```sh
m mgpio_aidl_test
```

Sau khi biên dịch xong, file `mgpio_aidl_test` sẽ được tạo ra.

```sh
ls -l $OUT/vendor/bin/mgpio_aidl_test
```

### 3.3. Cài đặt
```sh
adb root
adb remount
adb push $OUT/vendor/lib64/libmgpio.so /vendor/lib64/
adb push $OUT/vendor/bin/hw/android.hardware.mgpio-service /vendor/bin/hw/
adb push $OUT/vendor/etc/init/mgpio.rc /vendor/etc/init/
adb push $OUT/vendor/etc/vintf/manifest/mgpio.xml /vendor/etc/vintf/manifest/
adb push $OUT/vendor/bin/mgpio_aidl_test vendor/bin
```

### 3.4. Thiết lập quyền

```sh
adb shell chmod 755 /vendor/bin/hw/android.hardware.mgpio-service
adb shell chmod 755 /data/nativetest64/mgpio_aidl_test/mgpio_aidl_test
```

### 3.5. Chạy test và quan sát kết quả:
Chạy test binary

```sh
adb shell /vendor/bin/mgpio_aidl_test vendor/bin
```

Nếu thành công, GPIO 181 sẽ được bật và sẽ thấy output của các test case.

## 4. Kết luận