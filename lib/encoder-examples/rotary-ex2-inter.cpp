// https://lastminuteengineers.com/rotary-encoder-arduino-tutorial/

#include "Arduino.h"

// Rotary Encoder Inputs
#define CLK 2
#define DT 3

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";

void setup() {
	
	// Set encoder pins as inputs
	pinMode(CLK,INPUT);
	pinMode(DT,INPUT);

	// Setup Serial Monitor
	Serial.begin(9600);

	// Read the initial state of CLK
	lastStateCLK = digitalRead(CLK);
	
	// Call updateEncoder() when any high/low changed seen
	// on interrupt 0 (pin 2), or interrupt 1 (pin 3)
	attachInterrupt(0, updateEncoder, CHANGE);
	attachInterrupt(1, updateEncoder, CHANGE);
}

void loop() {
	//Do some useful stuff here
}

void updateEncoder(){
	// Read the current state of CLK
	currentStateCLK = digitalRead(CLK);

	// If last and current state of CLK are different, then pulse occurred
	// React to only 1 state change to avoid double count
	if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){

		// If the DT state is different than the CLK state then
		// the encoder is rotating CCW so decrement
		if (digitalRead(DT) != currentStateCLK) {
			counter --;
			currentDir ="CCW";
		} else {
			// Encoder is rotating CW so increment
			counter ++;
			currentDir ="CW";
		}

		Serial.print("Direction: ");
		Serial.print(currentDir);
		Serial.print(" | Counter: ");
		Serial.println(counter);
	}

	// Remember last CLK state
	lastStateCLK = currentStateCLK;
}
