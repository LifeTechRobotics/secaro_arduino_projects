//Control rotation angle or speed and direction by sending pulse 
//Servo1 PIN 19
//Servo2 PIN 22

#define DUTY_LOW 1500
#define DUTY_MID 5000
#define DUTY_HIGH 8500
#define DUTY_STEP 1000

#include "esp32-hal-ledc.h"
#include "M5Atom.h"

const int ledPin1 = 19;       // Servo1 PIN 19 を使用
const int ledPin2 = 22;       // Servo2 PIN 22 を使用
const int pwmChannel1 = 1;    // チャンネル 1 を使う
const int pwmChannel2 = 2;    // チャンネル 2 を使う
const int freq = 50;         // PWM 周波数
const int resolution = 16;   // 8ビットの分解能（0～255）

void setup() {
    // M5 の初期化
    M5.begin(true, false, true);
    // PWM チャンネルの初期化    
    ledcSetup(pwmChannel1, freq, resolution);
    ledcSetup(pwmChannel2, freq, resolution);
    // チャンネルとピンの紐づけ
    ledcAttachPin(ledPin1, pwmChannel1);
    ledcAttachPin(ledPin2, pwmChannel2);
}

bool run = true;

void loop() {
  if(run == true) {
    //前進
    ledcWrite(pwmChannel1, DUTY_MID + DUTY_STEP);
    ledcWrite(pwmChannel2, DUTY_MID - DUTY_STEP);
    delay(2000);

    //後退
    ledcWrite(pwmChannel1, DUTY_MID - DUTY_STEP);
    ledcWrite(pwmChannel2, DUTY_MID + DUTY_STEP);
    delay(2000);
    
    //左旋回
    ledcWrite(pwmChannel1, DUTY_MID - DUTY_STEP);
    ledcWrite(pwmChannel2, DUTY_MID - DUTY_STEP);
    delay(2000);

    //右旋回
    ledcWrite(pwmChannel1, DUTY_MID + DUTY_STEP);
    ledcWrite(pwmChannel2, DUTY_MID + DUTY_STEP);
    delay(2000);

    // 停止
    ledcWrite(pwmChannel1, 0);
    ledcWrite(pwmChannel2, 0);

    run = false;
  }
}
