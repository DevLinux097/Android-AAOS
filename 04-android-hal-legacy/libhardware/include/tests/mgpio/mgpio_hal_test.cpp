#define LOG_TAG	"mgpio_hal_test"

#include <utils/misc.h>
#include <utils/Log.h>
#include <map>
#include <stdio.h>
#include <hardware/mgpio.h>

int main()
{
    int ret;
    struct mgpio_module_t *module = NULL;
    struct mgpio_device_t *device = NULL;

    ret =  hw_get_module(GPIO_HARDWARE_MODULE_ID, (struct hw_module_t const**)&module);
    if (ret != 0) 
        printf("hw_get_module() failed (%s)\n", strerror(-ret));
    else {
        if (module->common.methods->open((struct hw_module_t *)module, "", ((hw_device_t **)&device)) != 0) {
            printf("Failed : HAL open (%s)\n",strerror(-ret));
            return 0;
        }
    }

    device->gpio_export(181);
    device->gpio_direction(181, 1);
    device->gpio_write(181, 1);

    printf("PHONGLT turn led on\n");	
    return 0;	
}