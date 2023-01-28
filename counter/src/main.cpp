//Optical Encoder Counter for Motor Controller

//This code aims to use the AMT103 Optical Encoder which should be placed
//on the brushless motor shaft. The AMT103 would then be connected to the ESP32 MicroController
//(the SAMD21 could also be used for testing) through an Analog Level Shifter [according to Talisha´s diagram].

//ask Talisha about Analog Level Shifter details

//Raul Vazquez Guerero

#include <SimpleFOC.h>
#include <math.h>

#define ESP32_BAUDRATE 115200

//Instantiating Encoder Class

/*To initialize the encoder we need to provide the encoder A and B
channel pins, encoder PPR and optionally index pin.*/

//  Encoder(int encA, int encB , int cpr, int index)
//  - encA, encB    - encoder A and B pins
//  - ppr           - pulses per rotation  (cpr=ppr*4) - mistake here from simplefoc, variable should be called CPR
//  - index pin     - (optional input)

//DO WE NEED THE OPTIONAL INPUT? Let´s assume NO for the moment...
//
int encA = 15; //D2 pin on the ESP32
int encB = 2; //D4 pin on the ESP32
//According to the AMT103 datasheet, the encoder is preset to PPR=2048 (meaning that CPR=8192)
int cpr = 8192;
int cprToPrint;

float angle_degrees; //defining a variable to measure the relative angle of the motor

Encoder encoder = Encoder(encA, encB, cpr);

// interrupt routine initialization
void doA(){encoder.handleA();}
void doB(){encoder.handleB();}

void setup() {
  // monitoring port
  Serial.begin(ESP32_BAUDRATE);

  // enable/disable quadrature mode
  encoder.quadrature = Quadrature::ON;

  // check if you need internal pullups
  encoder.pullup = Pullup::USE_EXTERN;
  
  // initialize encoder hardware
  encoder.init();
  // hardware interrupt enable
  encoder.enableInterrupts(doA, doB);

  Serial.println("Encoder ready");
  _delay(1000);
}

void loop() {

  //float angle_degrees = ((((encoder.getAngle()) * 2) * 180) / 3.146);

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