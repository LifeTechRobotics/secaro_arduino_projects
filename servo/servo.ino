//Control rotation angle or speed and direction by sending pulse 
//Servo PIN 19

#define DUTY_LOW 1500
#define DUTY_MID 5000
#define DUTY_HIGH 8500
#define TIMER_WIDTH 16
#define CHANNEL_1 1
#include "esp32-hal-ledc.h"
#include "M5Atom.h"

void setup() {
    M5.begin(true, false, true);  // Initialize serial port, LED
    ledcSetup(CHANNEL_1, 50, TIMER_WIDTH);
    ledcAttachPin(19, CHANNEL_1);
}

bool run = true;

void loop() {
  if(run == true) {
    //正回転
    for(int i = DUTY_MID; i > DUTY_LOW; i = i - 100){  
      ledcWrite(CHANNEL_1, i);
      delay(50);
    }
    
    // 停止
    ledcWrite(CHANNEL_1, 0);
    delay(1000);
    
    //逆回転
    for(int i = DUTY_MID; i < DUTY_HIGH; i = i + 100){  
      ledcWrite(CHANNEL_1, i);
      delay(50);
    }

    // 停止
    ledcWrite(CHANNEL_1, 0);

    run = false;
  }
}
