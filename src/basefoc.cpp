/**
 * Source code for motor controller functionality
 */

#include "../include/main.h"
extern Encoder encoder;

// BLDC motor & driver instance
// For Surpass C2216-880KV motor | 14 pole pairs | 0.108 Resistance | 880KV
BLDCMotor motor = BLDCMotor(14);

//  BLDCDriver3PWM( int phA, int phB, int phC, int enA, int enB, int enC )
//  - phA, phB, phC - A,B,C phase pwm pins
//  - enA, enB, enC - enable pin for each phase (optional)
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 5, 6, 8);

// InlineCurrentSensor constructor
//  - shunt_resistor  - shunt resistor value
//  - gain  - current-sense op-amp gain
//  - phA   - A phase adc pin
//  - phB   - B phase adc pin
//  - phC   - C phase adc pin (optional - _NC if not using)
InlineCurrentSense current_sense  = InlineCurrentSense(0.01, 50.0, A0, T1, T2);

// commander communication instance
Commander command = Commander(Serial);
void onMotor(char* cmd){ command.motor(&motor, cmd); }

void BaseFOC( void * pvParameters ) {

  Serial.println("Base Ready");
  motor.useMonitoring(Serial);

  // link the motor to the sensor
  motor.linkSensor(&encoder);

  // driver config
  driver.voltage_power_supply = 50.4; // Volts
  // driver.pwm_frequency = 20000; //based on mcu used
  driver.init();
  // link driver
  motor.linkDriver(&driver);
  // link current sense and driver
  current_sense.linkDriver(&driver);

  // aligning voltage
  motor.voltage_sensor_align = 3; // Volts

  // choose FOC modulation
  motor.foc_modulation = FOCModulationType::SinePWM;

  // set torque mode to be used
  // TorqueControlType::dc_current NEEDS CURRENT SENSING
  // TorqueControlType::foc_current NEEDS CURRENT SENSING
  // https://docs.simplefoc.com/torque_control#comparison
  motor.torque_controller = TorqueControlType::dc_current; //TBD
  // set motion control loop to be used
  motor.controller = MotionControlType::torque;

  // default voltage_power_supply
  motor.voltage_limit = 50.4; // Volts

  // angle loop velocity limit
  motor.velocity_limit = 50;

  // monitoring values to display
  // motor.monitor_variables = _MON_TARGET | _MON_VEL | _MON_ANGLE; // default _MON_TARGET | _MON_VOLT_Q | _MON_VEL | _MON_ANGLE

  // initialise motor
  motor.init();

  // initialise the current sensing
  current_sense.init();
  // link the current sense to the motor
  motor.linkCurrentSense(&current_sense);

  // align encoder and start FOC
  motor.initFOC();

  // set the inital target value
  motor.target = 2; // Volts
  
  _delay(1000);

  for(;;) { // equivalent to loop()

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
}

