// Control rotation angle or speed and direction by sending pulse 
// 車輪サーボ1 PIN 19
// 車輪サーボ2 PIN 22

#include "esp32-hal-ledc.h"
#include "M5Atom.h"

#define PIN_1 19          // 車輪サーボ1
#define PIN_2 22          // 車輪サーボ2
#define FREQ 50           // PWM周波数
#define RESOLUTION 12     // 12ビットの分解能（4096段階）

// 各デューティ比
const int centerDuty = (int)(4096 * 1.5 / 20.0); // 1.5ms に相当する duty（約307） → 停止
const int step = 50;         // デューティの刻み幅

// 実行フラグ
bool run = true;

void setup() {
    // シリアル通信の初期化
    Serial.begin(115200);
    delay(100);

    // M5の初期化
    M5.begin(true, false, false);

    // LEDC PIN設定
    ledcAttach(PIN_1, FREQ, RESOLUTION);
    ledcAttach(PIN_2, FREQ, RESOLUTION);

    // 初期停止
    ledcWrite(PIN_1, centerDuty);
    ledcWrite(PIN_2, centerDuty);
    delay(1000);
}

void loop() {
  if(run == true) {
    // 前進
    ledcWrite(PIN_1, centerDuty + step);
    ledcWrite(PIN_2, centerDuty - step);
    delay(2000);

    // 後退
    ledcWrite(PIN_1, centerDuty - step);
    ledcWrite(PIN_2, centerDuty + step);
    delay(2000);
    
    // 左旋回
    ledcWrite(PIN_1, centerDuty - step);
    ledcWrite(PIN_2, centerDuty - step);
    delay(2000);

    // 右旋回
    ledcWrite(PIN_1, centerDuty + step);
    ledcWrite(PIN_2, centerDuty + step);
    delay(2000);

    // 停止
    ledcWrite(PIN_1, centerDuty);
    ledcWrite(PIN_2, centerDuty);

    run = false;
  }
}
