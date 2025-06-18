// Control rotation angle or speed and direction by sending pulse 
// 車輪サーボ PIN 19
// 車輪サーボ PIN 22
// センサーサーボ PIN 23

#include <ESP32Servo.h>
#include <Wire.h>
#include <VL53L1X.h>
#include "esp32-hal-ledc.h"
#include "M5Atom.h"
#include "BluetoothSerial.h"

#define DUTY_F_LOW 5100        // 正回転の最小値
#define DUTY_R_LOW 4600        // 逆回転の最小値
#define DUTY_STEP 100

#define PIN_1 19               // 車輪サーボ PIN1
#define PIN_2 22               // 車輪サーボ PIN2
#define PIN_SS 23              // センサーサーボ PIN
#define CHANNEL_1 6            // チャンネル 1
#define CHANNEL_2 7            // チャンネル 2
#define FREQ 50                // PWM 周波数
#define RESOLUTION 16          // 16ビットの分解能（0～65535）
#define DEVICE_NAME "Secaro"  // Bluetooth デバイス名

// I2C PIN
#define SDA_PIN 26
#define SCL_PIN 32

// センサーサーボの回転角度
#define DEGREE_MIN 30
#define DEGREE_MAX 150

#define INTERVAL_TIME_SS 500   // センサーサーボの回転間隔
#define OBSTACLE_THRESHOLD 70  // 障害探知距離(mm)

// タイムアウト
#define TIME_OUT_SENSOR 100
#define TIME_OUT_SERIAL 500

BluetoothSerial SerialBT;
char command = '\0';        // 指令
short spdL = 1;             // 左輪速度
short spdR = 1;             // 右輪速度

// センサーサーボ変数
Servo sensorServo;
int deg = DEGREE_MIN;                // 角度
int deg_step = 30;                   // 1 回につきの回転角度
bool ccw = true;                     // 回転方向
unsigned long previousMillis = 0;    // 前回回転後時間
unsigned long currentMillis  = 0;    // 現在時間

// センサー変数
VL53L1X sensor;

// 回避変数
bool obstacleFlg = false;  // 回避フラグ
short spdL_taihi = 1;      // 回避前の車輪速度(左)
short spdR_taihi = 1;      // 回避前の車輪速度(右)
char command_taihi = '\0'; // 回避前の進行方向
int avoidTimeNeed = 0;     // 回避に必要な時間
int avoidTimeStart = 0;    // 回避開始時間
int avoidTimeCurrent = 0;  // 現在時間


void setup() {
    Serial.begin(115200);
    delay(100);

    // M5 の初期化
    M5.begin(true, false, false); // Serial, I2C, LED
    Wire.begin(SDA_PIN, SCL_PIN); // I2C 初期化

    // LEDC PIN 設定
    ledcAttachChannel(PIN_1, FREQ, RESOLUTION, CHANNEL_1);
    ledcAttachChannel(PIN_2, FREQ, RESOLUTION, CHANNEL_2);

    // センサーサーボ のピン設定
    sensorServo.attach(PIN_SS);
    sensorServo.write(DEGREE_MIN);

    // TOF センサーの初期化
    sensor.setBus(&Wire);
    sensor.setTimeout(TIME_OUT_SENSOR);
    if (!sensor.init()) {
      Serial.println("Failed to initialize sensor.\n");
    } else {
      sensor.setDistanceMode(VL53L1X::Short);
      sensor.setMeasurementTimingBudget(50000);
      sensor.startContinuous(50); // 測定間隔
    }

    // Bluetooth 待ち受け開始
    SerialBT.begin(DEVICE_NAME);
    delay(200);
    SerialBT.setTimeout(2000);
}

void loop() {
  char buffer[8] = {0};
  short pos = 0;
  int c = 0;
  bool valid = false;
  int distance = 0;
  
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
      spdL = constrain(String(&buffer[1]).toInt(), 1, 9);
    } else if (buffer[0] == 'r') {
      // 右輪速度
      spdR = constrain(String(&buffer[1]).toInt(), 1, 9);
    } else if (buffer[0] == 'F' || buffer[0] == 'B' || buffer[0] == 'L' || buffer[0] == 'R' || buffer[0] == 'S' || buffer[0] == 'P') {
      // 指令
      command = buffer[0];
    }
  }

  if (command == 'F') {
    distance = sensor.read();
    if (distance > 0 && distance < OBSTACLE_THRESHOLD) { // 障害物が検知された
      Serial.println("Obstacle detected.\n");

      // 回避モード
      obstacleFlg = true;

      // 回避前の速度と指令を退避
      spdL_taihi = spdL;
      spdR_taihi = spdR;
      command_taihi = command;

      // 回避中は最小速度
      spdL = 1;
      spdR = 1;

      // 右回転して回避
      command = 'R';

      // 回避必要な時間
      avoidTimeNeed = 5000; //暫定

      // 回避開始時間
      avoidTimeStart = millis();
    } else {
      // 前進
      ledcWrite(PIN_1, DUTY_F_LOW + DUTY_STEP*(spdL-1));
      ledcWrite(PIN_2, DUTY_R_LOW - DUTY_STEP*(spdR-1));

      currentMillis = millis();
      if (previousMillis == 0) { // 初回の場合
        previousMillis = currentMillis;
      }
      // 前進時のみセンサーを動かす
      if ((currentMillis - previousMillis) >= INTERVAL_TIME_SS) {
        // センサーサーボの角度を設定
        if (ccw) {
          deg += deg_step;
          if (deg >= DEGREE_MAX) {
            deg = DEGREE_MAX;
          }
        } else {
          deg -= deg_step;
          if (deg < DEGREE_MIN) {
            deg = DEGREE_MIN;
          }
        }
        // Serial.printf("degree: %d\n", deg);
        sensorServo.write(deg);
        previousMillis = millis();

        // 次回の回転方向を決める
        if (deg >= DEGREE_MAX) {
          ccw = false;
        } else if (deg <= DEGREE_MIN) {
          ccw = true;
        }
      }
    }
  } else if (command == 'B') {
    // 後退
    ledcWrite(PIN_1, DUTY_R_LOW - DUTY_STEP*(spdL-1));
    ledcWrite(PIN_2, DUTY_F_LOW + DUTY_STEP*(spdR-1));
  } else if (command == 'L') {
    // 左旋回
    ledcWrite(PIN_1, DUTY_R_LOW - DUTY_STEP*(spdL-1));
    ledcWrite(PIN_2, DUTY_R_LOW - DUTY_STEP*(spdR-1));
  } else if (command == 'R') {
    if (obstacleFlg) { // 回避モード
      avoidTimeCurrent = millis();
      if ((avoidTimeCurrent - avoidTimeStart) >= avoidTimeNeed) { // 回避終了
        obstacleFlg = false;

        // 回避前の速度に戻す
        spdL = spdL_taihi;
        spdR = spdR_taihi;

        // 回避前の指令に戻す
        command = command_taihi;

        // 回避用変数初期化
        avoidTimeStart = 0;
        avoidTimeCurrent = 0;
        avoidTimeNeed = 0;
        spdL_taihi = 1;
        spdR_taihi = 1;
        command_taihi = '\0';        
      } else {
        // 右旋回
        ledcWrite(PIN_1, DUTY_F_LOW + DUTY_STEP*(spdL-1));
        ledcWrite(PIN_2, DUTY_F_LOW + DUTY_STEP*(spdR-1));
      }
    } else {
      // 右旋回
      ledcWrite(PIN_1, DUTY_F_LOW + DUTY_STEP*(spdL-1));
      ledcWrite(PIN_2, DUTY_F_LOW + DUTY_STEP*(spdR-1));
    }
  } else if (command == 'S') {
    // 停止
    ledcWrite(PIN_1, 0);
    ledcWrite(PIN_2, 0);
  } else if (command == 'P') {
    // 通信確認用
    SerialBT.println("PING OK");
  }

  delay(20);
}

