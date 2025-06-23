// Control rotation angle or speed and direction by sending pulse 
// 車輪サーボ1 PIN 19

#include "esp32-hal-ledc.h"
#include "M5Atom.h"

#define PIN_1 19          // 車輪サーボ1
#define FREQ 50           // PWM周波数
#define RESOLUTION 12     // 12ビットの分解能（4096段階）

// 各デューティ比（12bit対応）
int minDuty = (int)(4096 * 0.5 / 20.0); // 0.5ms に相当する duty（約102） → 最大逆回転
int centerDuty = (int)(4096 * 1.5 / 20.0); // 1.5ms に相当する duty（約307） → 停止
int maxDuty = (int)(4096 * 2.5 / 20.0); // 2.5ms に相当する duty（約512） → 最大正回転

// デューティの刻み幅
const int step = 5;          // 小さくすると精細に制御できる

// delay時間
const int dTime = 30;     // 小さいくすると、加速が速くなり、大きくするとなめらかに

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

    // 初期停止
    ledcWrite(PIN_1, centerDuty);
    delay(1000);
}


void loop() {
  if(run == true) {
    // ゆっくり正回転（停止→最大正転）
    for (int duty = centerDuty; duty <= maxDuty; duty += step) {
      ledcWrite(PIN_1, duty);
      delay(dTime);
    }

    delay(1000);  // 最大速度で1秒回転

    // ゆっくり停止（正転→停止）
    for (int duty = maxDuty; duty >= centerDuty; duty -= step) {
      ledcWrite(PIN_1, duty);
      delay(dTime);
    }

    delay(1000);

    // ゆっくり逆回転（停止→最大逆転）
    for (int duty = centerDuty; duty >= minDuty; duty -= step) {
      ledcWrite(PIN_1, duty);
      delay(dTime);
    }

    delay(1000);  // 最大逆回転で1秒

    // ゆっくり停止（逆転→停止）
    for (int duty = minDuty; duty <= centerDuty; duty += step) {
      ledcWrite(PIN_1, duty);
      delay(dTime);
    }

    delay(1000);

    run = false;
  }
}
