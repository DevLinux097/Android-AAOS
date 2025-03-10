#ifndef ANDROID_HARDWARE_GPIO_H
#define ANDROID_HARDWARE_GPIO_H

#include <hardware/hardware.h>

__BEGIN_DECLS

#define GPIO_HARDWARE_MODULE_ID "mgpio"

#define INPUT   (0)	// GPIO direction as input
#define OUTPUT  (1)	// GPIO direction as output

#define LOW 	(0)	// GPIO value Low
#define HIGH 	(1)	// GPIO value High

#define BUFFER_MAX 		(4)
#define DIRECTION_MAX 	(35)
#define VALUE_MAX 		(30)

typedef enum ErrorCode
{
    FAILURE = -1, 
    SUCCESS
} error_code_e;

// Represent for device
struct gpio_device_t {
    struct hw_device_t common;
    int (*gpio_export)(int32_t pin);
    int (*gpio_unexport)(int32_t pin);
    int (*gpio_direction)(int32_t pin, int32_t dir);
    int (*gpio_read)(int32_t pin);
    int (*gpio_write)(int32_t pin, int32_t value);
};

// Represent for all of methods and propeties on device
struct gpio_module_t {
    struct hw_module_t common;
};

__END_DECLS

#endif // ANDROID_HARDWARE_GPIO_H