#ifndef MAIN_H
#define MAIN_H

#include "Arduino.h"
#include "SimpleFOC.h"
#include <math.h>

// Sensor Debug Interval in s, 0 for off
#define DEBUG_SERIAL_EN         1

#if DEBUG_SERIAL_EN
    #define DEBUG_SERIAL_LN(x) Serial.println(x)
    #define DEBUG_SERIAL(x) Serial.print(x)
    #define DEBUG_SERIAL_B(x, b) Serial.println(x, b)
#else
    #define DEBUG_SERIAL_LN(x) { }
    #define DEBUG_SERIAL(x) { }
    #define DEBUG_SERIAL_B(x, b) {}
#endif

// PCB Pinout Macros
#define CSET 20
#define OVC 23
#define TEMP 18
#define VREF 14

#endif