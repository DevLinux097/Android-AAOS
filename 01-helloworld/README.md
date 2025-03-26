# Hướng Dẫn Chi Tiết Xây Dựng Ứng Dụng "Hello World" và Shared Library trên AAOS

Bài viết này hướng dẫn từng bước để xây dựng một ứng dụng "Hello World" đơn giản, sử dụng một thư viện chia sẻ (`shared library`) tự định nghĩa trên Android. Chúng ta sẽ tìm hiểu về cấu trúc dự án, cách biên dịch, và cách chạy ứng dụng.

## 1. Tổng quan

Ứng dụng "Hello World" này sẽ:

1.  In ra dòng chữ "Hello, World!".
2.  Thực hiện các phép toán cộng, trừ, nhân, chia sử dụng thư viện chia sẻ `mmath`.

Thư viện `mmath` cung cấp các hàm toán học cơ bản. Việc tách riêng các hàm toán học vào một thư viện chia sẻ giúp tái sử dụng code và làm cho cấu trúc dự án gọn gàng hơn.

## 2. Cấu trúc thư mục dự án
Dưới đây là cấu trúc thư mục của dự án:

```sh
    android_source/external/helloworld/
    ├── Android.bp          # File build script cho cả ứng dụng và thư viện
    ├── include/
    │   └── mmath.h         # File header định nghĩa API cho thư viện mmath
    └── src/
        ├── hello_world.cpp     # Mã nguồn chính của ứng dụng Hello World
        └── mmath.cpp           # Triển khai các hàm của thư viện mmath
```

Vai trò của từng file:

- **`Android.bp`**: File build script của hệ thống Android (Soong) để biên dịch ứng dụng và thư viện.
- **`mmath.h`**: File header định nghĩa các hàm (API) mà thư viện `mmath` cung cấp.  Đây là giao diện để `hello_world.cpp` sử dụng các hàm trong `mmath.cpp`.
- **`hello_world.cpp`**: Mã nguồn chính của ứng dụng "Hello World". File này gọi các hàm từ thư viện `mmath` và thực hiện ghi file.
- **`mmath.cpp`**: Chứa code triển khai (implementation) của các hàm toán học được định nghĩa trong `mmath.h`.

## 3. Chi tiết mã nguồn
### 3.1. File `include/mmath.h`
#### Chi tiết mã nguồn:
```c++
#ifndef MY_SHARED_LIB_H
#define MY_SHARED_LIB_H

int add(int a, int b);       // Function to add two integers
int subtract(int a, int b);  // Function to subtract two integers
int multiply(int a, int b);  // Function to multiply two integers
int divide(int a, int b);    // Function to divide two integers

#endif // MY_SHARED_LIB_H
```

#### Giải thích:
- `#ifndef MY_SHARED_LIB_H, #define MY_SHARED_LIB_H, #endif`: Đây là các preprocessor directives, đảm bảo rằng file header này chỉ được include một lần duy nhất trong quá trình biên dịch, tránh lỗi do định nghĩa trùng lặp.
- `int add(int a, int b);, ...`: Khai báo (declaration) các hàm mà thư viện mmath cung cấp.

### 3.2. File `include/mmath.cpp`
#### Chi tiết mã nguồn:
```cpp
#include "mmath.h"

int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a * b;
}

int divide(int a, int b) {
    if (b != 0) {
        return a / b;
    } else {
        return 0;
    }
}
```

#### Giải thích:
- `int add(int a, int b) { ... }, ...`: Định nghĩa (definition) các hàm đã được khai báo trong mmath.h.

### 3.3. File `include/hello_world.cpp`
#### Chi tiết mã nguồn:
```cpp
#include <iostream>
#include <fstream>
#include "../include/mmath.h" // Include the mmath header

int main() {
    int a = 10;
    int b = 5;

    std::cout << "Hello, World!" << std::endl;
    std::cout << "Addition: " << add(a, b) << std::endl;                // Call add function from mmath
    std::cout << "Subtraction: " << subtract(a, b) << std::endl;        // Call subtract function from mmath
    std::cout << "Multiplication: " << multiply(a, b) << std::endl;     // Call multiply function
    std::cout << "Division: " << divide(a, b) << std::endl;             // Call divide function from mmath

    return 0;
}
```

#### Giải thích:
- Hàm `main` là điểm bắt đầu của chương trình. Nó in ra thông báo "Hello, World!", gọi các hàm từ thư viện mmath để thực hiện các phép tính.

### 3.4. File `include/hello_world.cpp`
#### Chi tiết mã nguồn:

```cpp
cc_library_shared {
    name: "mmath",
    srcs: ["src/mmath.cpp"],
    include_dirs: ["external/helloworld/include"], // Path to include directory
}

cc_binary {
    name: "hello_world",
    srcs: ["src/hello_world.cpp"],
    shared_libs: ["mmath"], // Link with the mmath shared library
    vendor: true,
}
```

#### Giải thích:
- File này mô tả cách build thư viện mmath và file thực thi hello_world sử dụng hệ thống build Soong của Android. Nó chỉ định các file nguồn, thư mục include, và liên kết giữa file thực thi với thư viện chia sẻ.

## 4. Cách biên dịch và chạy
### 4.1. Biên dịch
- Trong source Android, di chuyển đến thư mục external.
- Tạo thư mục helloworld và copy toàn bộ source code vào.

```sh
cd android_source/external
mkdir helloworld
cd helloworld
# Copy toàn bộ source code vào thư mục này
```

- Build toàn bộ module.
  
```sh
make hellworld
```

- Kiểm tra file đã được build.

```sh
# Kiểm tra thư viện mmath
ls -l $OUT/vendor/lib64/libmmath.so

# Kiểm tra file thực thi hello_world
ls -l $OUT/vendor/bin/hello_world
```

### 4.2. Cài đặt và Chạy
#### 4.2.1.  Cài đặt thư viện
```sh
adb root
adb remount
adb push $OUT/vendor/lib64/libmmath.so /vendor/lib64/
```

#### 4.2.2. Cài đặt ứng dụng
```sh
adb push $OUT/vendor/bin/hello_world /vendor/bin/
```

#### 4.2.3. Thiết lập quyền
```sh
adb shell chmod 755 /vendor/lib64/libmmath.so
adb shell chmod 755 /vendor/bin/hello_world
adb shell chown root:root /vendor/lib64/libmmath.so
adb shell chown root:shell /vendor/bin/hello_world
```

#### 4.2.4 Chạy chương trình
```sh
adb shell /vendor/bin/hello_world
```

Kết quả output:

```sh
Hello, World!
Addition: 15
Subtraction: 5
Multiplication: 50
Division: 2
```

## 5. Kết luận
Bài viết này đã cung cấp hướng dẫn chi tiết để xây dựng một ứng dụng "Hello World" đơn giản và một thư viện chia sẻ trên Android. Bạn đã học được cách tổ chức dự án, viết code, build, và chạy ứng dụng. Đây là nền tảng quan trọng để bạn có thể phát triển các ứng dụng phức tạp.



