/**
 * Source code for motor controller functionality
 */

#include "Arduino.h"
#include <SimpleFOC.h>

//  Encoder(int encA, int encB , int cpr, int index)
//  - encA, encB    - encoder A and B pins
//  - ppr           - impulses per rotation  (cpr=ppr*4)
//  - index pin     - (optional input)
Encoder sensor = Encoder(2, 3, 2048); //will we have hardware or software interrupts?

// interrupt routine initialization
void doA(){sensor.handleA();}
void doB(){sensor.handleB();}
//void doIndex(){encoder.handleIndex();} if we have more than 2 hardware interrupts

// BLDC motor & driver instance
// For Surpass C2216-880KV motor | 14 pole pairs | 0.108 Resistance | 880KV
BLDCMotor motor = BLDCMotor(14);

//  BLDCDriver3PWM( int phA, int phB, int phC, int enA, int enB, int enC )
//  - phA, phB, phC - A,B,C phase pwm pins
//  - enA, enB, enC - enable pin for each phase (optional)
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 5, 6, 8);

// commander communication instance
Commander command = Commander(Serial);
void onMotor(char* cmd){ command.motor(&motor, cmd); }

void setup() {

  // enable/disable quadrature mode
  sensor.quadrature = Quadrature::ON;

  // check if you need internal pullups, enable if so
  // sensor.pullup = Pullup::USE_EXTERN;

  // initialise magnetic sensor hardware
  sensor.init();
  // link the motor to the sensor
  motor.linkSensor(&sensor);

  // enable encoder hardware interrupts
  sensor.enableInterrupts(doA, doB);

  // driver config
  // power supply voltage [V]
  driver.voltage_power_supply = 12;
  driver.init();
  // link driver
  motor.linkDriver(&driver);

  // aligning voltage
  motor.voltage_sensor_align = 3;

  // choose FOC modulation
  motor.foc_modulation = FOCModulationType::SinePWM;

  // set torque mode to be used
  // TorqueControlType::voltage    ( default )
  // TorqueControlType::dc_current NEEDS CURRENT SENSING
  // TorqueControlType::foc_current NEEDS CURRENT SENSING
  // https://docs.simplefoc.com/torque_control#comparison
  motor.torque_controller = TorqueControlType::voltage;
  // set motion control loop to be used
  motor.controller = MotionControlType::torque;

  // default voltage_power_supply
  motor.voltage_limit = 12;

  // velocity low pass filtering time constant
  motor.LPF_velocity.Tf = 0.01;

  // angle loop controller
  motor.P_angle.P = 20;
  // angle loop velocity limit
  motor.velocity_limit = 50;

  // use monitoring with serial for motor init
  // monitoring port
  Serial.begin(115200);
  motor.useMonitoring(Serial);
  // used to disable debug outut
  // SimpleFOCDebug::enable(NULL);

  // monitoring values to display
  // motor.monitor_variables = _MON_TARGET | _MON_VEL | _MON_ANGLE; // default _MON_TARGET | _MON_VOLT_Q | _MON_VEL | _MON_ANGLE

  // initialise motor
  motor.init();
  // align encoder and start FOC
  motor.initFOC();

  // set the inital target value
  motor.target = 2; // Volts

  // define the motor id
  command.add('A', onMotor, "motor");

  // Run user commands to configure and the motor (find the full command list in docs.simplefoc.com)
  Serial.println(F("Motor commands sketch | Initial motion control > torque/voltage : target 2V."));
  
  _delay(1000);
}

void loop() {
  // IMPORTANT - call as frequently as possible
  // update the sensor values 
  sensor.update();
  
  // can display the angle and the angular velocity to the terminal
  Serial.println(sensor.getVelocity());

  // can display current motor position or phase voltage Ua
  Serial.println(motor.shaft_angle);

  // Function running the low level torque control loop
  // it calculates the gets motor angle and sets the appropriate voltages 
  // to the phase pwm signals
  // - the faster you can run it the better Arduino UNO ~1ms, Bluepill ~ 100us
  motor.loopFOC();

  // iterative function setting the outter loop target
  // velocity, position or voltage
  // if target not set in parameter below, use motor.target variable
  motor.move();

  // motor monitoring
  motor.monitor();
  
  // user communication
  command.run();
}