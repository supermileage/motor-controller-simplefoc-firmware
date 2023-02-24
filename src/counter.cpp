/**
  Optical Encoder Counter for Motor Controller

  This code aims to use the AMT103 Optical Encoder which should be placed
  on the brushless motor shaft. The AMT103 would then be connected to the ESP32 MicroController
  (the SAMD21 could also be used for testing) through an Analog Level Shifter [according to Talisha´s diagram].
  ask Talisha about Analog Level Shifter details

  Raul Vazquez Guerero
**/

#include "../include/main.h"

// According to the AMT103 datasheet, the encoder is preset to PPR=2048 (meaning that CPR=8192)
//  Encoder(int encA, int encB , int cpr, int index)
//  - encA, encB    - encoder A and B pins
//  - ppr           - impulses per rotation  (cpr=ppr*4)
//  - index pin     - (optional input)
Encoder encoder = Encoder(15, 2, 8192);

// Instantiating Encoder Class
int cprToPrint;
float angle_degrees; // defining a variable to measure the relative angle of the motor

void Counter( void * pvParameters ) {

  // enable/disable quadrature mode
  encoder.quadrature = Quadrature::ON;

  // check if you need internal pullups
  encoder.pullup = Pullup::USE_EXTERN;
  
  // initialize encoder hardware
  encoder.init();

  Serial.println("Encoder ready");
  _delay(1000);

  for(;;) {
    // IMPORTANT - call as frequently as possible
    // update the sensor values 
    encoder.update();
    
    // display the angle and the angular velocity to the terminal
    angle_degrees = encoder.getAngle();
    angle_degrees = ((((angle_degrees) * 4) * 180) / PI);
    angle_degrees = (int)angle_degrees % 360;
    
    cprToPrint = encoder.getFullRotations();
    cprToPrint = cprToPrint % 8192;

    Serial.print(angle_degrees);
    Serial.print("\t");
    Serial.println(cprToPrint);
  }
}