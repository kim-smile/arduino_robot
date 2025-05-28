#include <SoftwareSerial.h>

// 핀 설정
const int trigPin = 4;
const int echoPin = 5;
float duration, distance;

#define LEDRED  9
#define LEDYEL  10
#define LEDGRN  11
#define VOLSEN  A0
#define CDSSEN  A1
#define BTN1    8

// 전역 변수
bool bStart = false;
int nVol, cdsVol, btnVol;
int lastMotion = -1;

SoftwareSerial mySerial(2, 3); // RX, TX

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(LEDRED, OUTPUT);
  pinMode(LEDYEL, OUTPUT);
  pinMode(LEDGRN, OUTPUT);

  pinMode(BTN1, INPUT);
  pinMode(VOLSEN, INPUT);
  pinMode(CDSSEN, INPUT);

  Serial.begin(115200);
  mySerial.begin(115200);

  Serial.println("------------ 프로그램 시작 -----------");
  Serial.println("--------software Serial 시작 --------");
}

void readSensor() {
  nVol = analogRead(VOLSEN);
  cdsVol = analogRead(CDSSEN);
  btnVol = digitalRead(BTN1);

  Serial.print("btnVol = ");
  Serial.print(btnVol);
  Serial.print(" | cdsVol = ");
  Serial.print(cdsVol);
  Serial.print(" | nVol(가변저항) = ");
  Serial.println(nVol);
}

bool toggleCheck() {
  return btnVol;
}

void ledCon(int inVol) {
  digitalWrite(LEDGRN, LOW);
  digitalWrite(LEDRED, HIGH);
  delay(inVol);
  digitalWrite(LEDRED, LOW);
  digitalWrite(LEDYEL, HIGH);
  delay(inVol);
  digitalWrite(LEDYEL, LOW);
  digitalWrite(LEDGRN, HIGH);
  delay(inVol);
}

void robotCon(int nMotion) {
  unsigned char exeCmd[15] = {0xff, 0xff, 0x4c, 0x53,
                              0x00, 0x00, 0x00, 0x00,
                              0x30, 0x0c,
                              0x03,
                              0x01, 0x00, 100,
                              0x00};
  exeCmd[11] = nMotion;
  exeCmd[14] = 0x00;
  for (int i = 6; i < 14; i++)
    exeCmd[14] += exeCmd[i];

  mySerial.write(exeCmd, 15);
  Serial.print("동작 전송: ");
  Serial.println(nMotion);
  delay(50);
}

void readUltrasonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

void loop() {
  readSensor();
  readUltrasonic();

  if (toggleCheck()) bStart = !bStart;

  if (bStart) {
    robotCon(19);
    delay(7000);
    bStart = false;
  } else {
    Serial.println("Waiting for button press...");
  }

  int currentMotion = -1;

  if (cdsVol <= 500 || distance > 150) {
    currentMotion = 115; // 앉기
  } else if (distance > 100 && distance <= 150) {
    currentMotion = 116; // 서기
  } else if (distance > 50 && distance <= 100) {
    currentMotion = 19;  // 인사
  } else if (distance <= 50) {
    currentMotion = 22;  // 전투태세
  }

  if (currentMotion != -1 && currentMotion != lastMotion) {
    robotCon(currentMotion);
    lastMotion = currentMotion;
  }

  delay(100);
}