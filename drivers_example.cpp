#include <Arduino.h>

// Motor Connections (Both must use PWM pins)
#define PWM1 5
#define PWM2 6
#define DIR1 8
#define DIR2 7

void setup() {

  Serial.begin(9600);

  // Set motor connections as outputs
  pinMode(PWM1, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(DIR2, OUTPUT);


  // Stop motor
  digitalWrite(PWM1, LOW);
  digitalWrite(DIR1, LOW);
  digitalWrite(PWM2, LOW);
  digitalWrite(DIR2, LOW);

}

void loop() {

  // Accelerate forward
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, LOW);
  for (int i = 0; i <= 255; i++) {
    analogWrite(PWM1, i);
    analogWrite(PWM2, i);
    Serial.print("Forward acc: ");
    Serial.println(i);
    delay(50);
  }

  Serial.println("STOP");
  delay(500);

  for (int i = 255; i >= 0; i--) {
    analogWrite(PWM1, i);
    analogWrite(PWM2, i);
    Serial.print("Forward dec: ");
    Serial.println(i);
    delay(50);
  }

  Serial.println("STOP");
  delay(500);

  // Accelerate reverse
  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, HIGH);

  for (int i = 0; i <= 255; i++) {
    analogWrite(PWM1, i);
    analogWrite(PWM2, i);
    Serial.print("Reverse acc: ");
    Serial.println(i);
    delay(50);
  }

  Serial.println("STOP");
  delay(500);
  
  for (int i = 255; i >= 0; i--) {
    analogWrite(PWM1, i);
    analogWrite(PWM2, i);
    Serial.print("Reverse dec: ");
    Serial.println(i);
    delay(50);
  }

  Serial.println("STOP");
  delay(500);

  // FUNCIONAMIENTO CABLES CAMBIADOS
  // Decelerate forward
  // digitalWrite(DIR1, HIGH);
  // for (int i = 0; i < 127; i++) {
  //   analogWrite(PWM1, i);
  //   Serial.print("Forward dec: ");
  //   Serial.println(i);
  //   delay(50);
  // }

  // Serial.println("STOP");
  // delay(500);
  
  // // Accelerate reverse
  // for (int i = 127; i <= 255; i++) {
  //   analogWrite(PWM1, i);
  //   Serial.print("Reverse acc: ");
  //   Serial.println(i);
  //   delay(50);
  // }

  // Serial.println("STOP");
  // delay(500);
}
