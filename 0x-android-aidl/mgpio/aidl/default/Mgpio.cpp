#define LOG_TAG "rpi4_gpio"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <android-base/logging.h>

#include "Mgpio.h"

#define BUFFER_MAX 256
#define VALUE_MAX 256
#define DIRECTION_MAX 256

namespace aidl::android::hardware::mgpio {

::ndk::ScopedAStatus Mgpio::exportGpio(int pin, Status* _aidl_return) {
    LOG(DEBUG) << "exportGpio: pin=" << pin;

    char buffer[BUFFER_MAX];
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1) {
        LOG(ERROR) << "Failed to open /sys/class/gpio/export: " << strerror(errno);
        *_aidl_return = Status::FAILURE;
        return ndk::ScopedAStatus::ok(); // Always return ok()
    }

    ssize_t bytesWritten = snprintf(buffer, sizeof(buffer), "%d", pin);
    if (bytesWritten < 0 || (size_t)bytesWritten >= sizeof(buffer)) {
        LOG(ERROR) << "Failed to format pin number: " << pin;
        close(fd);
        *_aidl_return = Status::FAILURE;
        return ndk::ScopedAStatus::ok();
    }

    if (write(fd, buffer, bytesWritten) != bytesWritten) {
        LOG(ERROR) << "Failed to write to /sys/class/gpio/export: " << strerror(errno);
        close(fd);
        *_aidl_return = Status::FAILURE;
        return ndk::ScopedAStatus::ok();
    }

    close(fd);
    *_aidl_return = Status::SUCCESS;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus Mgpio::unexportGpio(int pin, Status* _aidl_return) {
    LOG(DEBUG) << "unexportGpio: pin=" << pin;

    char buffer[BUFFER_MAX];
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1) {
        LOG(ERROR) << "Failed to open /sys/class/gpio/unexport: " << strerror(errno);
        *_aidl_return = Status::FAILURE;
        return ndk::ScopedAStatus::ok();
    }

    ssize_t bytesWritten = snprintf(buffer, sizeof(buffer), "%d", pin);
     if (bytesWritten < 0 || (size_t)bytesWritten >= sizeof(buffer)) {
        LOG(ERROR) << "Failed to format pin number: " << pin;
        close(fd);
        *_aidl_return = Status::FAILURE;
        return ndk::ScopedAStatus::ok();
    }

    if (write(fd, buffer, bytesWritten) != bytesWritten) {
        LOG(ERROR) << "Failed to write to /sys/class/gpio/unexport: " << strerror(errno);
        close(fd);
        *_aidl_return = Status::FAILURE;
        return ndk::ScopedAStatus::ok();
    }

    close(fd);
    *_aidl_return = Status::SUCCESS;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus Mgpio::readValue(int pin, int* _aidl_return) {
    LOG(DEBUG) << "readValue: pin=" << pin;

    char path[VALUE_MAX];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        LOG(ERROR) << "Failed to open " << path << ": " << strerror(errno);
        *_aidl_return = -1; // Indicate failure
        return ndk::ScopedAStatus::ok();
    }

    char value;
    if (read(fd, &value, 1) != 1) {
        LOG(ERROR) << "Failed to read from " << path << ": " << strerror(errno);
        close(fd);
        *_aidl_return = -1; // Indicate failure
        return ndk::ScopedAStatus::ok();
    }

    close(fd);

    // Convert char '0' or '1' to integer 0 or 1
    if (value == '0') {
        *_aidl_return = 0;
    } else if (value == '1') {
        *_aidl_return = 1;
    } else {
        LOG(ERROR) << "Unexpected value read from " << path << ": " << value;
        *_aidl_return = -1; // Indicate failure
        return ndk::ScopedAStatus::ok();
    }

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus Mgpio::writeValue(int pin, int value, Status* _aidl_return) {
    LOG(DEBUG) << "writeValue: pin=" << pin << ", value=" << value;

    char path[VALUE_MAX];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);

    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        LOG(ERROR) << "Failed to open " << path << ": " << strerror(errno);
        *_aidl_return = Status::FAILURE;
        return ndk::ScopedAStatus::ok();
    }

    char charValue = (value == 0) ? '0' : '1';
    if (write(fd, &charValue, 1) != 1) {
        LOG(ERROR) << "Failed to write to " << path << ": " << strerror(errno);
        close(fd);
        *_aidl_return = Status::FAILURE;
        return ndk::ScopedAStatus::ok();
    }

    close(fd);
    *_aidl_return = Status::SUCCESS;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus Mgpio::setDirection(int pin, Direction dir, Status* _aidl_return) {
    LOG(DEBUG) << "setDirection: pin=" << pin << ", dir=" << static_cast<int>(dir); // Log the int value of dir

    char path[DIRECTION_MAX];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);

    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        LOG(ERROR) << "Failed to open " << path << ": " << strerror(errno);
        *_aidl_return = Status::FAILURE;
        return ndk::ScopedAStatus::ok();
    }

    const char* directionStr;
    if (dir == Direction::INPUT) {
        directionStr = "in";
    } else if (dir == Direction::OUTPUT) {
        directionStr = "out";
    } else {
        LOG(ERROR) << "Invalid direction: " << static_cast<int>(dir);
        *_aidl_return = Status::FAILURE; // Or a more specific error
        return ndk::ScopedAStatus::ok();
    }

    size_t dirLen = strlen(directionStr);

    if (write(fd, directionStr, dirLen) != (ssize_t)dirLen) {
        LOG(ERROR) << "Failed to write to " << path << ": " << strerror(errno);
        close(fd);
        *_aidl_return = Status::FAILURE;
        return ndk::ScopedAStatus::ok();
    }

    close(fd);
    *_aidl_return = Status::SUCCESS;
    return ndk::ScopedAStatus::ok();
}

} // namespace aidl::android::hardware::mgpio