// Control rotation angle or speed and direction by sending pulse 
// 車輪サーボ1 PIN 19
// 車輪サーボ2 PIN 22

#include "esp32-hal-ledc.h"
#include "M5Atom.h"
#include "BluetoothSerial.h"

const int PIN_1 = 19;               // 車輪サーボ1
const int PIN_2 = 22;               // 車輪サーボ2
const int FREQ = 50;                // PWM周波数
const int RESOLUTION = 12;          // 12ビットの分解能（4096段階）

// 各デューティ比
const int centerDuty = (int)(4096 * 1.5 / 20.0); // 1.5ms に相当する duty（約307） → 停止
const int centerDutyHigh = 313;  // 停止範囲の上限
const int centerDutyLow = 290;   // 停止範囲の下限
const int step = 10;             // デューティの刻み幅

// Bluetooth通信用
const char* DEVICE_NAME = "Secaro";
BluetoothSerial SerialBT;

// 走行指令用
char command = '\0';
int spdL = 1;
int spdR = 1;

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

    // Bluetooth待ち受け開始
    SerialBT.begin(DEVICE_NAME);
    delay(200);
    SerialBT.setTimeout(2000);
}

void loop() {
  char buffer[8] = {0};
  int pos = 0;
  int c = 0;
  bool valid = false;
  
  // 受信処理
  while (SerialBT.available()) {
    if (pos == sizeof(buffer)) {
      break;
    }
    c = SerialBT.read();
    if (c == -1) { // 受信異常
      return;
    }
    if (c == '\n') { // 電文の末尾
      buffer[pos++] = '\0';
      valid = true;
      break;
    }else {
      buffer[pos++] = c;
    }
  }

  // 電文解析
  if (valid) {
    if (buffer[0] == 'l') {
      // 左輪速度
      spdL = String(buffer[1]).toInt();
    } else if (buffer[0] == 'r') {
      // 右輪速度
      spdR = String(buffer[1]).toInt();
    } else if (buffer[0] == 'F' || buffer[0] == 'B' || buffer[0] == 'L' || buffer[0] == 'R' || buffer[0] == 'S' || buffer[0] == 'P') {
      // 指令
      command = buffer[0];
    }
  }

  if (command == 'F') {
    // 前進
    ledcWrite(PIN_1, centerDutyHigh + step*spdL);
    ledcWrite(PIN_2, centerDutyLow - step*spdR);
  } else if (command == 'B') {
    // 後退
    ledcWrite(PIN_1, centerDutyLow - step*spdL);
    ledcWrite(PIN_2, centerDutyHigh + step*spdR);
  } else if (command == 'L') {
    // 左旋回
    ledcWrite(PIN_1, centerDutyLow - step*spdL);
    ledcWrite(PIN_2, centerDutyLow - step*spdR);
  } else if (command == 'R') {
    // 右旋回
    ledcWrite(PIN_1, centerDutyHigh + step*spdL);
    ledcWrite(PIN_2, centerDutyHigh + step*spdR);
  } else if (command == 'S') {
    // 停止
    ledcWrite(PIN_1, centerDuty);
    ledcWrite(PIN_2, centerDuty);
  } else if (command == 'P') {
    // 通信確認用
    SerialBT.println("PING OK");
  }

  delay(20);
}

