#include <Arduino.h>
#include <Motor.h>
#include <PS4Controller.h>

/* JOYSTICK */
const int DEAD_ZONE = 30;

// pwm dir channel
Motor motor1(32, 22, 1);
Motor motor2(33, 21, 2);
Motor motor3(25, 19, 3);
Motor motor4(26, 18, 4);
Motor motor5(14, 17, 5);

/*switch*/
const int SW_FRONT_PIN = 23;
const int SW_REAR_PIN = 34;

void setup() {
  Serial.begin(115200);

  uint8_t bt_mac[6];
  esp_read_mac(bt_mac, ESP_MAC_BT);
  Serial.printf("Bluetooth Mac Address => %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                bt_mac[0], bt_mac[1], bt_mac[2], bt_mac[3], bt_mac[4],
                bt_mac[5]);

  PS4.begin("08:D1:F9:37:41:F2");
  Serial.printf("ready.\n");

  pinMode(SW_FRONT_PIN, INPUT_PULLDOWN);
  pinMode(SW_REAR_PIN, INPUT_PULLDOWN);
}

void loop() {

  bool isOnFrontSwitch = digitalRead(SW_FRONT_PIN);
  bool isOnRearSwitch = digitalRead(SW_REAR_PIN);

  if (!PS4.isConnected()) {
    Serial.printf("PS4 controller disconnected.\n");
    motor1.run(0, 0);
    motor2.run(0, 0);
    motor3.run(0, 0);
    motor4.run(0, 0);
    motor5.run(0, 0);
    return;
  }

  if (DEAD_ZONE <= abs(PS4.RStickY())) {
    int pwr = abs(PS4.RStickY());
    bool dir = PS4.RStickY() > 0 ? 0 : 1;
    motor1.run(pwr, dir); // 右モーター
  } else {
    motor1.run(0, 0);
  }
  if (DEAD_ZONE <= abs(PS4.LStickY())) {
    int pwr = abs(PS4.LStickY());
    bool dir = PS4.LStickY() > 0 ? 1 : 0;
    motor2.run(pwr, dir); // 左モーター
  } else {
    motor2.run(0, 0);
  }

  if (PS4.R2Value() > 15) {
    uint8_t motor34Speed = PS4.R2Value() / 2;
    motor3.run(motor34Speed, 0); // 正転
    motor4.run(motor34Speed, 1);
  } else if (PS4.L2Value() > 15) {
    uint8_t motor34Speed = PS4.L2Value() / 2;
    motor3.run(motor34Speed, 1); // 逆転
    motor4.run(motor34Speed, 0);
  } else {
    motor3.run(0, 0);
    motor4.run(0, 0);
  }

  if (PS4.Right() && !isOnFrontSwitch) {
    motor5.run(64, 0);
  } else if (PS4.Left() && !isOnRearSwitch) {
    motor5.run(64, 1);
  } else {
    motor5.run(0, 0);
  }

  if (PS4.PSButton()) {
    ESP.restart();
  }
}