//Control rotation angle or speed and direction by sending pulse 

#define DUTY_LOW 1500
#define DUTY_MID 5000
#define DUTY_HIGH 8500
#include "esp32-hal-ledc.h"
#include "M5Atom.h"

const int ledPin = 19;       // Servo PIN 19 を使用
const int pwmChannel = 1;    // チャンネル 1 を使う
const int freq = 50;         // PWM 周波数
const int resolution = 16;   // 8ビットの分解能（0～255）

void setup() {
    // M5 の初期化
    M5.begin(true, false, true);
    // PWM チャンネルの初期化
    ledcSetup(pwmChannel, freq, resolution);
    // チャンネルとピンの紐づけ
    ledcAttachPin(ledPin, pwmChannel);
}

bool run = true;

void loop() {
  if(run == true) {
    //正回転
    for(int i = DUTY_MID; i < DUTY_HIGH; i = i + 100){  
      ledcWrite(pwmChannel, i);
      delay(50);
    }

    // 停止
    ledcWrite(pwmChannel, 0);
    delay(1000);
    
    //逆回転
    for(int i = DUTY_MID; i > DUTY_LOW; i = i - 100){  
      ledcWrite(pwmChannel, i);
      delay(50);
    }
    
    // 停止
    ledcWrite(pwmChannel, 0);

    run = false;
  }
}
