// Control rotation angle or speed and direction by sending pulse 
// 車輪サーボ1 PIN 19

#include "esp32-hal-ledc.h"
#include "M5Atom.h"

#define DUTY_F_LOW 5100   // 正回転の最小値
#define DUTY_F_HIGH 8500  // 正回転の最大値
#define DUTY_R_LOW 4600   // 逆回転の最小値
#define DUTY_R_HIGH 1200  // 正回転の最大値
#define PIN_1 19          // 車輪サーボ1
#define FREQ 50           // PWM周波数
#define RESOLUTION 16     // 16ビットの分解能（0～65535）

bool run = true;

void setup() {
    // シリアル通信の初期化
    Serial.begin(115200);
    delay(100);

    // M5の初期化
    M5.begin(true, false, false);

    // LEDC PIN設定
    ledcAttach(PIN_1, FREQ, RESOLUTION);
}


void loop() {
  if(run == true) {
    // 正回転
    for(int i = DUTY_F_LOW; i < DUTY_F_HIGH; i = i + 100){  
      ledcWrite(PIN_1, i);
      delay(50);
    }

    // 停止
    ledcWrite(PIN_1, 0);
    delay(1000);
    
    // 逆回転
    for(int i = DUTY_R_LOW; i > DUTY_R_HIGH; i = i - 100){  
      ledcWrite(PIN_1, i);
      delay(50);
    }
    
    // 停止
    ledcWrite(PIN_1, 0);

    run = false;
  }
}
