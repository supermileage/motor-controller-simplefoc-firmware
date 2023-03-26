/**
 * Source code for Urban motor controller functionality
 */

#include "main.h"
#include "CanThrottle.h"

extern Encoder encoder;

CanThrottle throttle;

//  BLDCMotor(int pp, (optional R, KV))
//  - pp  - pole pair number
//  - R   - phase resistance value - optional
//  - KV  - motor KV rating [rpm/V] - optional
BLDCMotor motor = BLDCMotor(POLE_PAIRS);

//  BLDCDriver3PWM( int phA, int phB, int phC, int en)
//  - phA, phB, phC - A,B,C phase pwm pins
//  - enable pin    - (optional input)
BLDCDriver3PWM driver = BLDCDriver3PWM(PHASE_A,PHASE_B,PHASE_C);

// InlineCurrentSensor constructor
//  - shunt_resistor  - shunt resistor value
//  - gain  - current-sense op-amp gain
//  - phA   - A phase adc pin
//  - phB   - B phase adc pin
//  - phC   - C phase adc pin (optional - _NC if not using)
// InlineCurrentSense current_sense  = InlineCurrentSense(0.01, 50.0, A0, T1, T2);

// velocity set point variable
float target_velocity = 0; // 2Rad/s ~ 20rpm
float curr_vref = 0; // in V

int curr_temp_volt_int;
float curr_temp_volt;
float curr_temp_res;
float curr_temp; // in DegC

float max_current = 10;

// commander communication instance
Commander command = Commander(Serial);
void doTarget(char* cmd) {command.scalar(&target_velocity,cmd); }
void doMotor(char* cmd){ command.motor(&motor, cmd); }

void BaseFOC( void * pvParameters ) {

  DEBUG_SERIAL_LN("Base Ready");
  motor.useMonitoring(Serial);

  // VerboseMode::nothing        - display nothing - good for monitoring
  // VerboseMode::on_request     - display only on user request
  // VerboseMode::user_friendly  - display textual messages to the user (default)
  command.verbose = VerboseMode::user_friendly;

  // link the motor to the sensor
  motor.linkSensor(&encoder);

  // power supply voltage [V] - Change this locally for testing
  driver.voltage_power_supply = 24;
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
  motor.LPF_velocity.Tf = 0.2;

  // angle loop controller
  motor.P_angle.P = 20;

  // setting the limits - Change this locally for testing
  // either voltage
  motor.voltage_limit = 6; // Volts - default driver.voltage_limit
  // or current 
  // motor.current_limit = 22; // Amps - default 0.2Amps

  // angle loop velocity limit
  motor.velocity_limit = 2;

  // monitoring values to display
  // motor.monitor_variables; // default _MON_TARGET | _MON_VOLT_Q | _MON_VEL | _MON_ANGLE
  
  command.add('M', doMotor, "motor");

  // initialise motor
  motor.init();

  // initialise the current sensing
  // current_sense.init();
  // link the current sense to the motor
  // motor.linkCurrentSense(&current_sense);

  // align encoder and start FOC
  motor.initFOC();

  command.add('T', doTarget, "target velocity");

  throttle.begin();

  for(;;) { // equivalent to loop()

    if(digitalRead(OVC) == HIGH) {
      DEBUG_SERIAL("Current Exceeded Maximum");
      // break;
    }

    analogWrite(CSET, max_current*0.01);

    curr_temp_volt_int = analogRead(TEMP_CONV);
    curr_temp_volt = curr_temp_volt_int*(3.3/4096.0);
    curr_temp_res = 10000*(3.3-curr_temp_volt)/(curr_temp_volt);
    curr_temp = 1.0/(A_CONST + B_CONST*log(curr_temp_res) + C_CONST*pow((log(curr_temp_res)),3)) - 273.15; // Res -> Temp
    if(curr_temp > 100.0) {
      DEBUG_SERIAL("Temperatures Rising High");
    }
    if(curr_temp > 125.0) {
      DEBUG_SERIAL("Temperature Exceeded Maximum");
      break;
    }

    // curr_vref = analogRead(VREF);
    // if(1); // calculations to cross-check output voltage with selected current

    throttle.loop();
    //  throttle.get(); // TODO: Use this to get a throttle value in [0,255]. Scale it as needed. 

    // can display current motor position or phase voltage Ua
    // DEBUG_SERIAL_LN(motor.shaft_angle);

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

    // Necessary to not upset watchdog timer
    yield();
  }
}

