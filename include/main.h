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

// Motor Controller Parameters
// For Surpass C2216-880KV motor | 7 pole pairs | 0.108 Resistance | 880KV
// For Koford 129H169T motor | 5 pole pairs | TBD Resistance | TBD
// For SHINANO LA034-040NN07A | 3 pole pairs
#define POLE_PAIRS 5

// Driver Phases
#define PHASE_A 32
#define PHASE_B 26
#define PHASE_C 25

// Encoder Macros
#define ENC_A 15
#define ENC_B 4
#define PPR 256

// ESP32-PCB Check Macros
#define CSET 20
#define OVC 23
#define TEMP_CONV 13
#define VREF 14

// Voltage-Resistance Thermistor Macros
// https://www.allaboutcircuits.com/industry-articles/how-to-obtain-the-temperature-value-from-a-thermistor-measurement/
// https://www.murata.com/en-eu/api/pdfdownloadapi?cate=&partno=NCP18XH103F03RB
#define A_CONST 0.000929028203
#define B_CONST 0.000248284038
#define C_CONST 0.000000191337313

#endif