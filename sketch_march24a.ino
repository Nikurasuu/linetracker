#include <Wire.h>

int pi = 250;
int line = 0;
boolean noLine = false;
boolean noLineEvent = false;
int noLineCounter = 0;

boolean piStopped = false;

int motorPWMLinks_1 = 4;
int motorPWMLinks_2 = 5;
int motorPWMRechts_1 = 6;
int motorPWMRechts_2 = 7;

int dirMotorLinks_1 = 8;
int dirMotorLinks_2 = 9;
int dirMotorRechts_1 = 10;
int dirMotorRechts_2 = 11;

int linksDrift = 0;
int rechtsDrift = 0;

int ausgleichMotorLinks1 = 0;
int ausgleichMotorLinks2 = -20;
int ausgleichMotorRechts1 = 0;
int ausgleichMotorRechts2 = 10;

int grundGeschwindigkeit = 75;

int geschwindigkeitLinks;
int geschwindigkeitRechts;

boolean finishedRight;
boolean finishedLeft;
boolean opperationFinished;

void receiveEvent(int howMany) {
  while (1 < Wire.available()) {
    char c = Wire.read();
    Serial.print(c);
  }
  pi = Wire.read();
}

void seriellerMonitor() {
  Serial.print(line);
  Serial.print(", ");
  Serial.print(geschwindigkeitLinks);
  Serial.print(", ");
  Serial.print(geschwindigkeitRechts);
  Serial.println(";");
}

void datenAuswerten() {
  if (pi > 0 && pi < 200) {
    line = pi - 100;
  }
  if (pi == 225) {
    noLine = true;
    Serial.print("no Line");
  } else {
    noLine = false;
  }

  if (pi == 235) {
    piStopped = true;
    Serial.println("stopped..");
    analogWrite(motorPWMLinks_1, 0);
    analogWrite(motorPWMRechts_1, 0);
    analogWrite(motorPWMLinks_2, 0);
    analogWrite(motorPWMRechts_2, 0);
  } else {
    piStopped = false;
  }
}

void geschwindigkeitAnpassen() {
  if (line > 0) {
    geschwindigkeitLinks = grundGeschwindigkeit + (line * 0.2) + linksDrift;
    geschwindigkeitRechts = grundGeschwindigkeit - (line * 1.2) + rechtsDrift;
  } else if (line < 0) {
    geschwindigkeitLinks = grundGeschwindigkeit + (line * 1.2) + linksDrift;
    geschwindigkeitRechts = grundGeschwindigkeit - (line * 0.2) + rechtsDrift;
  }
  if (geschwindigkeitLinks < 0) {
    geschwindigkeitLinks = 0;
  } else if (geschwindigkeitLinks > 255) {
    geschwindigkeitLinks = 255;
  }
  if (line == 0) {
    geschwindigkeitLinks = grundGeschwindigkeit  + linksDrift;
    geschwindigkeitRechts = grundGeschwindigkeit  + rechtsDrift;
  }

  if (geschwindigkeitRechts < 0) {
    geschwindigkeitRechts = 0;
  } else if (geschwindigkeitRechts > 255) {
    geschwindigkeitRechts = 255;
  }

  digitalWrite(dirMotorLinks_1, HIGH);
  digitalWrite(dirMotorRechts_1, LOW);
  digitalWrite(dirMotorLinks_2, HIGH);
  digitalWrite(dirMotorRechts_2, LOW);
  int speedMotor1 = geschwindigkeitLinks + ausgleichMotorLinks1;
  if (speedMotor1 < 0) {
    speedMotor1 = 1;
  }
  int speedMotor2 = geschwindigkeitLinks + ausgleichMotorLinks2;
  if (speedMotor2 < 0) {
    speedMotor2 = 1;
  }
  int speedMotor3 = geschwindigkeitRechts + ausgleichMotorRechts1;
  if (speedMotor3 < 0) {
    speedMotor2 = 1;
  }
  int speedMotor4 = geschwindigkeitRechts + ausgleichMotorRechts2;
  if (speedMotor4 < 0) {
    speedMotor4 = 1;
  }
  Serial.println(speedMotor2);
  analogWrite(motorPWMLinks_1, speedMotor1);
  analogWrite(motorPWMLinks_2, speedMotor2);
  analogWrite(motorPWMRechts_1, speedMotor3);
  analogWrite(motorPWMRechts_2, speedMotor4);
}

void writeMotor(int directionLeft, int directionRight, int motorSpeedLeft , int motorSpeedRight , int howLong) {
  Serial.print("writeMotor: ");
  Serial.print(directionLeft);
  Serial.print(", ");
  Serial.print(directionRight);
  Serial.print(", ");
  Serial.print(motorSpeedLeft);
  Serial.print(", ");
  Serial.print(motorSpeedRight);
  Serial.print(", ");
  Serial.print(howLong);
  Serial.println(";");
  if (directionLeft == 0) {
    directionLeft = 1;
  } else if (directionLeft == 1) {
    directionLeft = 0;
  }
  motorSpeedLeft = motorSpeedLeft + linksDrift;
  motorSpeedRight = motorSpeedRight + rechtsDrift;
  digitalWrite(dirMotorLinks_1, directionLeft);
  digitalWrite(dirMotorLinks_2, directionLeft);
  digitalWrite(dirMotorRechts_1, directionRight);
  digitalWrite(dirMotorRechts_2, directionRight);
  int speedMotor1 = motorSpeedLeft + ausgleichMotorLinks1;
  if (speedMotor1 < 0) {
    speedMotor1 = 1;
  }
  int speedMotor2 = motorSpeedLeft + ausgleichMotorLinks2;
  if (speedMotor2 < 0) {
    speedMotor2 = 1;
  }
  int speedMotor3 = motorSpeedRight + ausgleichMotorRechts1;
  if (speedMotor3 < 0) {
    speedMotor2 = 1;
  }
  int speedMotor4 = motorSpeedRight + ausgleichMotorRechts2;
  if (speedMotor4 < 0) {
    speedMotor4 = 1;
  }
  Serial.println(speedMotor2);
  analogWrite(motorPWMLinks_1, speedMotor1);
  analogWrite(motorPWMLinks_2, speedMotor2);
  analogWrite(motorPWMRechts_1, speedMotor3);
  analogWrite(motorPWMRechts_2, speedMotor4);
  delay(howLong);
}

void setup() {
  Wire.begin(0x04);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}

void loop() {
  datenAuswerten();

  while (!piStopped) {
    datenAuswerten();
    geschwindigkeitAnpassen();
    seriellerMonitor();

    if (noLine) {
      writeMotor(0, 0, 90, 90, 300);
      datenAuswerten();
      while (noLine) {
        writeMotor(1, 1, 70, 70, 1);
        datenAuswerten();
      }
    }

    if(line == 100){
      writeMotor(0, 1, 90, 90, 800);
    }

    
    if(line == -100){
      writeMotor(1, 0, 90, 90, 800);
    }
    
    if (line < -80 && noLine == false) {
      writeMotor(1, 1, 90, 90, 50);
      writeMotor(1, 0, 90, 90, 300);
    }

    if (line > 80 && noLine == false) {
      writeMotor(1, 1, 90, 90, 50);
      writeMotor(0, 1, 90, 90, 300);
    }
  }
}
