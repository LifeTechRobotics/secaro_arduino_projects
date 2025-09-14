// LED PIN 33

const int LED_PIN = 33;

void setup() {
  // LEDピンを出力モードに設定
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // LED ON
  digitalWrite(LED_PIN, HIGH);
  delay(1000);  // 1秒点灯

  // LED OFF
  digitalWrite(LED_PIN, LOW);
  delay(1000);  // 1秒消灯
}
