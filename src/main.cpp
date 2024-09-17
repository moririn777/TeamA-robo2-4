#include <Arduino.h>
#include <Motor.h>
#include <PS4Controller.h>

/* JOYSTICK */
const int DEAD_ZONE = 30;

// pwm dir channel
Motor motor1(32, 22, 1);
Motor motor2(33, 21, 2);
Motor motor3(25, 19, 3);
Motor motor4(26, 28, 4);
Motor motor5(14, 17, 5);

/*switch*/
const int SW_FRONT_PIN = 33;
const int SW_REAR_PIN = 32;

/* MOTOR FUNCTION */
void stopMotor();

void setup() {
  Serial.begin(115200);

  uint8_t bt_mac[6];
  esp_read_mac(bt_mac, ESP_MAC_BT);
  Serial.printf("Bluetooth Mac Address => %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                bt_mac[0], bt_mac[1], bt_mac[2], bt_mac[3], bt_mac[4],
                bt_mac[5]);

  PS4.begin("08:B6:1F:ED:4f:4E");
  Serial.printf("ready.\n");

  pinMode(SW_FRONT_PIN, INPUT_PULLDOWN);
  pinMode(SW_REAR_PIN, INPUT_PULLDOWN);
}

void loop() {

  bool isOnRearSwitch = digitalRead(SW_FRONT_PIN);
  bool isOnRearSwitch = digitalRead(SW_REAR_PIN);


  if (!PS4.isConnected()) {
    Serial.printf("PS4 controller disconnected.\n");
    stopMotor();
    return;
  }

  if (DEAD_ZONE <= abs(PS4.RStickY())) {
    motor1.run(abs(PS4.RStickY()), (PS4.RStickY() > 0 ? 0 : 1)); // 右モーター
  }
  if (DEAD_ZONE <= abs(PS4.LStickY())) {
    motor2.run(abs(PS4.LStickY()), (PS4.LStickY() > 0 ? 0 : 1)); // 左モーター
  }
  if (DEAD_ZONE > abs(PS4.LStickY()) && DEAD_ZONE > abs(PS4.RStickY())) {
    stopMotor();
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

  if (PS4.Right()) {
    motor5.run(64, 0);
  }
  if (PS4.Left()) {
    motor5.run(64, 1);
  }else{
    motor5.run(0,0);
  }

  if (PS4.PSButton()) {
    ESP.restart();
  }
}

/* STOP MOVING */
void stopMotor() {
  motor1.run(0, 0);
  motor2.run(0, 0);
}
