// Control rotation angle or speed and direction by sending pulse 
// Servo1 PIN 19
// Servo2 PIN 22

#include "esp32-hal-ledc.h"
#include "M5Atom.h"

#define DUTY_F_LOW 5100   // 正回転の最小値
#define DUTY_R_LOW 4600   // 逆回転の最小値
#define DUTY_STEP 300
#define PIN_1 19
#define PIN_2 22
#define FREQ 50           // PWM 周波数
#define RESOLUTION 16     // 16ビットの分解能（0～65535）  

void setup() {
    // M5 の初期化
    M5.begin(true, false, false);

    // LEDC PIN 設定
    ledcAttach(PIN_1, FREQ, RESOLUTION);
    ledcAttach(PIN_2, FREQ, RESOLUTION);
}

bool run = true;

void loop() {
  if(run == true) {
    // 前進
    ledcWrite(PIN_1, DUTY_F_LOW + DUTY_STEP);
    ledcWrite(PIN_2, DUTY_R_LOW - DUTY_STEP);    
    delay(2000);

    // 後退
    ledcWrite(PIN_1, DUTY_R_LOW - DUTY_STEP);
    ledcWrite(PIN_2, DUTY_F_LOW + DUTY_STEP);
    delay(2000);
    
    // 左旋回
    ledcWrite(PIN_1, DUTY_R_LOW - DUTY_STEP);
    ledcWrite(PIN_2, DUTY_R_LOW - DUTY_STEP);
    delay(2000);

    // 右旋回
    ledcWrite(PIN_1, DUTY_F_LOW + DUTY_STEP);
    ledcWrite(PIN_2, DUTY_F_LOW + DUTY_STEP);
    delay(2000);

    // 停止
    ledcWrite(PIN_1, 0);
    ledcWrite(PIN_2, 0);

    run = false;
  }
}
