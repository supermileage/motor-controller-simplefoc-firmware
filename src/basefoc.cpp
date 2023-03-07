/**
 * Source code for Urban motor controller functionality
 */

#include "../include/main.h"
extern Encoder encoder;

//  BLDCMotor(int pp, (optional R, KV))
//  - pp  - pole pair number
//  - R   - phase resistance value - optional
//  - KV  - motor KV rating [rpm/V] - optional
// For Surpass C2216-880KV motor | 14 pole pairs | 0.108 Resistance | 880KV
BLDCMotor motor = BLDCMotor(14);

// For Koford 129H169T motor | 10 pole pairs | TBD Resistance | TBD
// BLDCMotor motor = BLDCMotor(10);

//  BLDCDriver3PWM( int phA, int phB, int phC, int enA, int enB, int enC )
//  - phA, phB, phC - A,B,C phase pwm pins
//  - enA, enB, enC - enable pin for each phase (optional)
BLDCDriver3PWM driver = BLDCDriver3PWM(12, 13, 14);

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

  // power supply voltage [V]
  driver.voltage_power_supply = 48;
  // Max DC voltage allowed - default voltage_power_supply
  driver.voltage_limit = 50.4;
  driver.pwm_frequency = 30000; //based on mcu used
  driver.init();
  // link driver
  motor.linkDriver(&driver);
  // link current sense and driver
  current_sense.linkDriver(&driver);

  // aligning voltage
  motor.voltage_sensor_align = 3; // Volts

  // choose FOC modulation
  motor.foc_modulation = FOCModulationType::SinePWM;

  // set motion control loop to be used
  motor.controller = MotionControlType::velocity;

  // setting the limits
  // either voltage
  // motor.voltage_limit = 50.4; // Volts - default driver.voltage_limit
  // of current 
  motor.current_limit = 40; // Amps - default 0.2Amps

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

  // velocity set point variable
  float target_velocity = 2; // 2Rad/s ~ 20rpm

  _delay(1000);

  for(;;) { // equivalent to loop()

    // can display current motor position or phase voltage Ua
    // Serial.println(motor.shaft_angle);

    // Function running the low level torque control loop
    // it calculates the gets motor angle and sets the appropriate voltages 
    // to the phase pwm signals
    // - the faster you can run it the better Arduino UNO ~1ms, Bluepill ~ 100us
    motor.loopFOC();

    // iterative function setting the outer loop target
    // velocity, position or voltage
    // if target not set in parameter below, use motor.target variable
    motor.move(target_velocity);

    // motor monitoring
    motor.monitor();
    
    // user communication
    command.run();

    _delay(1000);
  }
}

