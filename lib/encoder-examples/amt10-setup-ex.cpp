// https://hackaday.io/project/9914-open-robotics-eurobot/log/34812-amt10-encoder-setup
// Example setup and testing code for amt 10 series encoders. Optimal for testing basic device functionality.

#include "Arduino.h"

int A = 12;
int B = 11;
int I = 10;

int countTick = 0;
int countIndex = 0;
char precTick = 0;
char precIndex = 0;
char tick = 0;
char tickB =0;
char index = 0;

void setup() {
  pinMode(A, INPUT);
  pinMode(B, INPUT);
  pinMode(I, INPUT);
  Serial.begin(9600);

}

void loop() {
  tick = digitalRead(A);
  tickB = digitalRead(B);
  index = digitalRead(I);
  
  if(tick != precTick)
  {
    if(tick != tickB)
    {
      countTick = countTick + tick;
      precTick = tick;
    }
    else
    {
      countTick = countTick - tick;
      precTick = tick;
    }
    Serial.print("tick :");
    Serial.println(countTick);
  }
  
  if(index != precIndex)
  {
    if(countTick > 0)
    {
      countIndex = countIndex + index;
      precIndex = index;
    }
    else
    {
      countIndex = countIndex - index;
      precIndex = index;
    }
    countTick = 0;
    Serial.print("turn :");
    Serial.println(countIndex);
  }
  
  
}