/**
 * Source code for Urban motor controller functionality
 */

#include "../include/main.h"
#include "CanThrottle.h"

extern Encoder encoder;

CanThrottle throttle;

//  BLDCMotor(int pp, (optional R, KV))
//  - pp  - pole pair number
//  - R   - phase resistance value - optional
//  - KV  - motor KV rating [rpm/V] - optional
// For Surpass C2216-880KV motor | 7 pole pairs | 0.108 Resistance | 880KV
BLDCMotor motor = BLDCMotor(7);

// For Koford 129H169T motor | 10 pole pairs | TBD Resistance | TBD
// BLDCMotor motor = BLDCMotor(10);

//  BLDCDriver6PWM( int phA_h, int phA_l, int phB_h, int phB_l, int phC_h, int phC_l, int en)
//  - phA_h, phA_l - A phase pwm pin high/low pair 
//  - phB_h, phB_l - B phase pwm pin high/low pair
//  - phB_h, phC_l - C phase pwm pin high/low pair
//  - enable pin    - (optional input)
BLDCDriver6PWM driver = BLDCDriver6PWM(13,12, 27,26, 33,32);

// InlineCurrentSensor constructor
//  - shunt_resistor  - shunt resistor value
//  - gain  - current-sense op-amp gain
//  - phA   - A phase adc pin
//  - phB   - B phase adc pin
//  - phC   - C phase adc pin (optional - _NC if not using)
// InlineCurrentSense current_sense  = InlineCurrentSense(0.01, 50.0, A0, T1, T2);

// velocity set point variable
float target_velocity = 0; // 2Rad/s ~ 20rpm

// commander communication instance
Commander command = Commander(Serial);
void doTarget(char* cmd) {command.scalar(&target_velocity,cmd); }
void onMotor(char* cmd){ command.motor(&motor, cmd); }

void BaseFOC( void * pvParameters ) {

  Serial.println("Base Ready");
  motor.useMonitoring(Serial);

  // link the motor to the sensor
  motor.linkSensor(&encoder);

  // power supply voltage [V] - Change this locally for testing
  driver.voltage_power_supply = 12;
  // Max DC voltage allowed - default voltage_power_supply
  driver.voltage_limit = 50.4;
  driver.pwm_frequency = 30000; //based on mcu used
  driver.init();
  // link driver
  motor.linkDriver(&driver);
  // link current sense and driver
  // current_sense.linkDriver(&driver);

  // aligning voltage
  motor.voltage_sensor_align = 1; // Volts

  // choose FOC modulation
  motor.foc_modulation = FOCModulationType::SinePWM;

  // set motion control loop to be used
  motor.controller = MotionControlType::velocity;

  // controller configuration based on the control type 
  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 20;
  motor.PID_velocity.D = 0.001;

  // velocity low pass filtering time constant
  motor.LPF_velocity.Tf = 0.01;

  // angle loop controller
  motor.P_angle.P = 20;

  // setting the limits - Change this locally for testing
  // either voltage
  motor.voltage_limit = 1; // Volts - default driver.voltage_limit
  // or current 
  // motor.current_limit = 22; // Amps - default 0.2Amps

  // angle loop velocity limit
  motor.velocity_limit = 50;

  // monitoring values to display
  // motor.monitor_variables = _MON_TARGET | _MON_VEL | _MON_ANGLE; // default _MON_TARGET | _MON_VOLT_Q | _MON_VEL | _MON_ANGLE
  
  command.add('M', onMotor, "motor");

  // initialise motor
  motor.init();

  // initialise the current sensing
  // current_sense.init();
  // link the current sense to the motor
  // motor.linkCurrentSense(&current_sense);

  // align encoder and start FOC
  motor.initFOC();

  command.add('T', doTarget, "target velocity");

  _delay(1000);

  for(;;) { // equivalent to loop()

    throttle.loop();
    //  throttle.get(); // Use this to get a throttle value in [0,255]. Scale it as needed. 

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

  }
}

