#include "../include/main.h"

void BaseFOC( void * pvParameters );
void Counter( void * pvParameters );

TaskHandle_t basefoc;
TaskHandle_t counter;

void setup() {

   // monitoring port
   Serial.begin(115200);
   Serial.println("Starting Setup");

   xTaskCreatePinnedToCore(
                    BaseFOC,
                    "basefoc",
                    10000,      // TBD - Stack size of task
                    NULL,       // Parameter of the task
                    1,          // Task Priority (higher is greater)
                    NULL,   // Task handle for tracking
                    0);         // Core to execute on              
  // delay(500); 

  xTaskCreatePinnedToCore(
                    Counter,
                    "counter",
                    10000,      // TBD - Stack size of task
                    NULL,       // Parameter of the task
                    1,          // Task Priority (higher is greater)
                    NULL,   // Task handle for tracking
                    1);         // Core to execute on
    // delay(500); 
}

void loop() {
  
}