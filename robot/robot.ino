//Control rotation angle or speed and direction by sending pulse 
//Servo1 PIN 19
//Servo2 PIN 22

#define DUTY_LOW 1500
#define DUTY_MID 5000
#define DUTY_HIGH 8500
#define DUTY_STEP 1000
#define TIMER_WIDTH 16
#define CHANNEL_1 1
#define CHANNEL_2 2

#include "esp32-hal-ledc.h"
#include "M5Atom.h"

void setup() {
    M5.begin(true, false, true);  // Initialize serial port, LED
    ledcSetup(CHANNEL_1, 50, TIMER_WIDTH);
    ledcSetup(CHANNEL_2, 50, TIMER_WIDTH);
    ledcAttachPin(19, CHANNEL_1);
    ledcAttachPin(22, CHANNEL_2);
}

bool run = true;

void loop() {
  if(run == true) {
    //前進
    ledcWrite(CHANNEL_1, DUTY_MID - DUTY_STEP);
    ledcWrite(CHANNEL_2, DUTY_MID + DUTY_STEP);
    delay(2000);
    
    //後退
    ledcWrite(CHANNEL_1, DUTY_MID + DUTY_STEP);
    ledcWrite(CHANNEL_2, DUTY_MID - DUTY_STEP);
    delay(2000);
    
    //正回転
    ledcWrite(CHANNEL_1, DUTY_MID + DUTY_STEP);
    ledcWrite(CHANNEL_2, DUTY_MID + DUTY_STEP);
    delay(2000);

    //逆回転
    ledcWrite(CHANNEL_1, DUTY_MID - DUTY_STEP);
    ledcWrite(CHANNEL_2, DUTY_MID - DUTY_STEP);
    delay(2000);

    // 停止
    ledcWrite(CHANNEL_1, 0);
    ledcWrite(CHANNEL_2, 0);

    run = false;
  }
}
