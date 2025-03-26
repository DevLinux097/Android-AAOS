#include <iostream>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <aidl/android/hardware/mgpio/BnMgpio.h>    // Include generated header (BnMgpio for the service)
#include <aidl/android/hardware/mgpio/IMgpio.h>     // Include the IMgpio interface
#include <aidl/android/hardware/mgpio/Status.h>     // Include the Status enum
#include <aidl/android/hardware/mgpio/Direction.h>  // Include Direction enum

// Use namespaces to avoid typing the full path repeatedly
using aidl::android::hardware::mgpio::IMgpio;
using aidl::android::hardware::mgpio::Direction;
using aidl::android::hardware::mgpio::Status;
using std::shared_ptr;
using std::string;
using std::cerr;
using std::endl;

int main() {
    // Set the maximum number of threads for the binder process
    // Start the binder thread pool
    ABinderProcess_setThreadPoolMaxThreadCount(1);
    ABinderProcess_startThreadPool();

    // Get the service manager
    string serviceName = string(IMgpio::descriptor) + "/default";

    // Wait for the service to be available.  Get the raw AIBinder*
    AIBinder* rawBinder = AServiceManager_waitForService(serviceName.c_str());
    if (rawBinder == nullptr) {
        cerr << "Could not connect to service: " << serviceName << endl;
        return 1;
    }

    // Convert the raw AIBinder* to a shared_ptr<IMgpio>.
    shared_ptr<IMgpio> service = IMgpio::fromBinder(ndk::SpAIBinder(rawBinder));

    if (!service) {
        cerr << "Service is null." << endl;
        return 1;
    }

    // Call the service methods for testing
    Status exportStatus;
    int pin = 181; // Example: GPIO pin 181

    // Test exportGpio
    ndk::ScopedAStatus exportResult = service->exportGpio(pin, &exportStatus);  // Pass &exportStatus
    if (!exportResult.isOk()) {
        cerr << "Error exporting GPIO " << pin << ": " << exportResult.getDescription() << endl;
    } else if (exportStatus != Status::SUCCESS) { // Check the Status value
        cerr << "Error exporting GPIO " << pin << " (Status: " << static_cast<int>(exportStatus) << ")" << endl;
    } else {
        std::cout << "Exported GPIO " << pin << " successfully." << endl;
    }

    // Test setDirection (OUTPUT)
    Status dirStatus; // Declare the Status variable
    ndk::ScopedAStatus dirStatusResult = service->setDirection(pin, Direction::OUTPUT, &dirStatus); // Pass &dirStatus
     if (!dirStatusResult.isOk()) {
        cerr << "Error setting direction OUTPUT for GPIO " << pin <<": " << dirStatusResult.getDescription() <<  endl;
    } else if (dirStatus != Status::SUCCESS) {
        cerr << "Error setting direction OUTPUT for GPIO " << pin << " (Status: " << static_cast<int>(dirStatus) << ")" << endl;
    }
    else {
        std::cout << "Set direction OUTPUT successfully." << endl;
    }

    // Test writeValue (HIGH)
    Status writeStatus; //Declare Status variable
    ndk::ScopedAStatus writeStatusResult = service->writeValue(pin, 1, &writeStatus); // Write HIGH (1), Pass &writeStatus
    if (!writeStatusResult.isOk()) {
        cerr << "Error writing HIGH to GPIO " << pin <<": " << writeStatusResult.getDescription() <<  endl;
    } else if(writeStatus != Status::SUCCESS){
        cerr << "Error writing HIGH to GPIO " << pin << " (Status: " << static_cast<int>(writeStatus) << ")" << endl;
    }
    else {
        std::cout << "Write HIGH successful." << endl;
    }

    // Test readValue
    int value;
    ndk::ScopedAStatus readValueResult = service->readValue(pin, &value);
    if(!readValueResult.isOk()){
         cerr << "Error reading from GPIO " << pin <<": " << readValueResult.getDescription() <<  endl;
    }
    else{
        std::cout << "Read value from GPIO " << pin << ": " << value << endl;
    }

    // Test writeValue (LOW)
    writeStatusResult = service->writeValue(pin, 0, &writeStatus); //Pass &writeStatus
    if (!writeStatusResult.isOk()) {
        cerr << "Error writing LOW to GPIO " << pin <<": " << writeStatusResult.getDescription() <<  endl;
    }else if(writeStatus != Status::SUCCESS){
        cerr << "Error writing LOW to GPIO " << pin << " (Status: " << static_cast<int>(writeStatus) << ")" << endl;
    }
    else {
        std::cout << "Write LOW successful." << endl;
    }

    // Test readValue again
    readValueResult = service->readValue(pin, &value);

    if(!readValueResult.isOk()){
         cerr << "Error reading from GPIO " << pin <<": " << readValueResult.getDescription() <<  endl;
    }
    else{
        std::cout << "Read value from GPIO " << pin << ": " << value << endl;
    }

    // Test unexportGpio
    Status unexportStatus; //Declare Status variable
    ndk::ScopedAStatus unexportStatusResult = service->unexportGpio(pin, &unexportStatus); //Pass &unexportStatus
    if (!unexportStatusResult.isOk()) {
        cerr << "Error unexporting GPIO " << pin << ": " << unexportStatusResult.getDescription() << endl;
    } else if (unexportStatus != Status::SUCCESS) {
        cerr << "Error unexporting GPIO " << pin << " (Status: " << static_cast<int>(unexportStatus) << ")" << endl;
    }
    else {
        std::cout << "Unexported GPIO " << pin << " successfully." << endl;
    }

    // Keep the process alive to handle callbacks.  ESSENTIAL!
    ABinderProcess_joinThreadPool();

    return 0;
}