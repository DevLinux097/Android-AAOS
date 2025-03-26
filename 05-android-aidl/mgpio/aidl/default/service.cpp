#define LOG_TAG "android.hardware.mgpio-service"

#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <android-base/logging.h>

#include "Mgpio.h"

using namespace aidl::android::hardware::mgpio;

int main() {
    // Log the start of the Mgpio AIDL.
    LOG(INFO) << "Mgpio AIDL starting";

    // Set the thread pool max thread count to 0.  This means the service will
    // handle each request in the calling thread, rather than creating new threads.
    // This is generally preferred for simple HALs that don't have long-running
    // operations, to avoid thread creation overhead.
    if (!ABinderProcess_setThreadPoolMaxThreadCount(0)) {
        LOG(ERROR) << "Failed to set thread pool max thread count";
        return EXIT_FAILURE; // Use EXIT_FAILURE for clarity
    }

    // Create a shared pointer to the Mgpio implementation. SharedRefBase::make
    // is the preferred way to create instances of AIDL-generated classes.
    std::shared_ptr<Mgpio> mgpio = ndk::SharedRefBase::make<Mgpio>();
    if (!mgpio) {
        LOG(ERROR) << "Failed to create Mgpio instance";
        return EXIT_FAILURE;
    }

    // Construct the full instance name.  This is "interface_name/instance_name".
    // In this case, it's "android.hardware.mgpio.IMgpio/default".
    const std::string instance = std::string() + IMgpio::descriptor + "/default";

    // Register the service with the service manager.  This makes it available
    // to clients. asBinder() gets the underlying IBinder object, and .get()
    // gets the raw pointer, which is what AServiceManager_addService expects.
    binder_status_t status = AServiceManager_addService(mgpio->asBinder().get(), instance.c_str());

    // Check the status of the service registration.
    if (status != STATUS_OK) {
        // Log an error and exit if registration fails.  This is a critical
        // failure.
        LOG(ERROR) << "Failed to register mgpio HAL service. Status: " << status;
        return EXIT_FAILURE;
    }

    // Log that the service is running.
    LOG(INFO) << "Mgpio HAL service is running";

    // Join the binder thread pool.  This is *essential*.  Without this, the
    // service won't be able to receive any incoming requests.  This call
    // blocks indefinitely.
    ABinderProcess_joinThreadPool();

    // This should never be reached, as ABinderProcess_joinThreadPool() never
    // returns unless there's a serious error.
    LOG(ERROR) << "Mgpio HAL service unexpectedly exited"; // Log if it unexpectedly exits
    return EXIT_FAILURE;
}