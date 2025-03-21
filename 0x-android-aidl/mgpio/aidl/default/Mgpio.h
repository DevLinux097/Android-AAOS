#pragma once

#include <aidl/android/hardware/mgpio/IMgpio.h>
#include <aidl/android/hardware/mgpio/BnMgpio.h>
#include <aidl/android/hardware/mgpio/Status.h>
#include <aidl/android/hardware/mgpio/Direction.h>
#include <aidl/android/hardware/mgpio/Voltage.h>
#include <android/binder_status.h>

namespace aidl::android::hardware::mgpio {

using aidl::android::hardware::mgpio::Status;
using aidl::android::hardware::mgpio::Direction;
using aidl::android::hardware::mgpio::Voltage;

class Mgpio : public BnMgpio {
public:
    Mgpio() = default;
    virtual ~Mgpio() = default;

    ::ndk::ScopedAStatus exportGpio(int pin, Status* _aidl_return) override;
    ::ndk::ScopedAStatus unexportGpio(int pin, Status* _aidl_return) override;
    ::ndk::ScopedAStatus readValue(int pin, int* _aidl_return) override;
    ::ndk::ScopedAStatus writeValue(int pin, int value, Status* _aidl_return) override;
    ::ndk::ScopedAStatus setDirection(int pin, Direction dir, Status* _aidl_return) override;
};

} // namespace aidl::android::hardware::mgpio