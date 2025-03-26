package android.hardware.mgpio;

import android.hardware.mgpio.Status;
import android.hardware.mgpio.Direction;
import android.hardware.mgpio.Voltage;

interface IMgpio {
    Status exportGpio(in int pin);
    Status unexportGpio(in int pin);
    int readValue(in int pin);
    Status writeValue(in int pin, in int value);
    Status setDirection(in int pin, in Direction dir);
}