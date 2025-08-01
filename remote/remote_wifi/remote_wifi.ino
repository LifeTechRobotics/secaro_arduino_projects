// Control rotation angle or speed and direction by sending pulse 
// 車輪サーボ1 PIN 19
// 車輪サーボ2 PIN 22

#include "esp32-hal-ledc.h"
#include "M5Atom.h"
#include <WiFi.h>
#include <WiFiUdp.h>

const int PIN_1 = 19;               // 車輪サーボ1
const int PIN_2 = 22;               // 車輪サーボ2
const int FREQ = 50;                // PWM周波数
const int RESOLUTION = 12;          // 12ビットの分解能（4096段階）

// 各デューティ比
const int centerDuty = (int)(4096 * 1.5 / 20.0); // 1.5ms に相当する duty（約307） → 停止
const int centerDutyHigh = 313;  // 停止範囲の上限
const int centerDutyLow = 290;   // 停止範囲の下限
const int step = 10;             // デューティの刻み幅

// WiFi通信用
const char* ssid = "ssid";
const char* password = "password";
const char* DEVICE_NAME = "Secaro";
const int BROADCAST_PORT = 4210;   // ブロードキャスト送信用
const int CONTROL_PORT   = 4211;   // コマンド受信用
WiFiUDP udpBroadcast;
WiFiUDP udpControl;
IPAddress broadcastIP;

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

    // WiFiへ接続
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP());

    // ブロードキャストIPを計算（例: 192.168.1.255）
    IPAddress ip = WiFi.localIP();
    IPAddress subnet = WiFi.subnetMask();
    broadcastIP = (ip & subnet) | ~subnet;

    udpControl.begin(CONTROL_PORT);
    Serial.printf("UDP Server started at port %d\n", CONTROL_PORT);
}

void loop() {
  // 変数宣言 
  char buffer[8] = {0};
  bool valid = false;

  // ブロードキャストメッセージの送信
  String msg = String(DEVICE_NAME) + ":" + WiFi.localIP().toString();
  const uint8_t* msgC = reinterpret_cast<const uint8_t*>(msg.c_str());
  udpBroadcast.beginPacket(broadcastIP, BROADCAST_PORT);
  udpBroadcast.write(msgC, strlen(msg.c_str()));
  udpBroadcast.endPacket();
  // Serial.println("Broadcast: " + msg);

  // 受信処理
  int packetSize = udpControl.parsePacket();
  if (packetSize) {
    int len = udpControl.read(buffer, 8);
    // Serial.printf("Received: %s\n", buffer);
    if (len > 0 && len <= 2) buffer[len] = 0;
    valid = true;
  }

  if (valid) {
    // 電文解析
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
  }

  delay(20);
}

