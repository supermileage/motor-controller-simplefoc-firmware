/**
 * Source code for Urban motor controller encoder functionality
 */

#include "main.h"

// According to the AMT103 datasheet, the encoder is preset to PPR=2048 (meaning that CPR=8192)
//  Encoder(int encA, int encB , int cpr, int index)
//  - encA, encB    - encoder A and B pins
//  - ppr           - impulses per rotation  (cpr=ppr*4)
//  - index pin     - (optional input)
Encoder encoder = Encoder(ENC_A, ENC_B, PPR);

// interrupt routine initialization
void doA(){encoder.handleA();}
void doB(){encoder.handleB();}

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

  encoder.enableInterrupts(doA, doB);

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
    
    // cprToPrint = encoder.getFullRotations();
    // cprToPrint = cprToPrint % 8192;

    // DEBUG_SERIAL(String(angle_degrees));
    // DEBUG_SERIAL("\t");
    // DEBUG_SERIAL(String(cprToPrint) + "\n");

    // Necessary to not upset watchdog timer
    yield();
  }
}