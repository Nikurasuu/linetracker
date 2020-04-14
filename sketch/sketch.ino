#include <Wire.h>

#define MOTOR_PWM_LINKS_1 4
#define MOTOR_PWM_LINKS_2 5
#define MOTOR_PWM_RECHTS_1 6
#define MOTOR_PWM_RECHTS_2 7

#define DIR_MOTOR_LINKS_1 8
#define DIR_MOTOR_LINKS_2 9
#define DIR_MOTOR_RECHTS_1 10
#define DIR_MOTOR_RECHTS_2 11

#define LINKS_DRIFT 0
#define RECHTS_DRIFT 0

#define AUSGLEICH_MOTOR_LINKS_1 0
#define AUSGLEICH_MOTOR_LINKS_2 0
#define AUSGLEICH_MOTOR_RECHTS_1 0
#define AUSGLEICH_MOTOR_RECHTS_2 0

#define GRUND_GESCHWINDIGKEIT 50

int geschwindigkeitLinks;
int geschwindigkeitRechts;

int pi = 250;
int line = 0;
boolean noLine = false;
boolean noLineEvent = false;
int noLineCounter = 0;

boolean piStopped = false;

boolean finishedRight;
boolean finishedLeft;
boolean opperationFinished;

boolean greenLeft = false;
boolean greenRight = false;
boolean greenBoth = false;

boolean cornerLeft = false;
boolean cornerRight = false;
int counter = 0;

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
    analogWrite(MOTOR_PWM_LINKS_1, 0);
    analogWrite(MOTOR_PWM_LINKS_2, 0);
    analogWrite(MOTOR_PWM_RECHTS_1, 0);
    analogWrite(MOTOR_PWM_RECHTS_2, 0);
  } else {
    piStopped = false;
  }

  if (pi == 205) {
    greenLeft = true;
  } else {
    greenLeft = false;
  }

  if (pi == 206) {
    greenRight = true;
  } else {
    greenLeft = false;
  }

  if (pi == 207) {
    greenBoth = true;
  } else {
    greenBoth = false;
  }

  if (pi == 201) {
    cornerLeft = true;
  } else {
    cornerLeft = false;
  }

  if (pi == 202) {
    cornerRight = true;
  } else {
    cornerRight = false;
  }
}

void geschwindigkeitAnpassen() {
  if (line > 0) {
    geschwindigkeitLinks = GRUND_GESCHWINDIGKEIT + (line * 0.2) + LINKS_DRIFT;
    geschwindigkeitRechts = GRUND_GESCHWINDIGKEIT - (line * 1.2) + RECHTS_DRIFT;
  } else if (line < 0) {
    geschwindigkeitLinks = GRUND_GESCHWINDIGKEIT + (line * 1.2) + LINKS_DRIFT;
    geschwindigkeitRechts = GRUND_GESCHWINDIGKEIT - (line * 0.2) + RECHTS_DRIFT;
  }
  if (geschwindigkeitLinks < 0) {
    geschwindigkeitLinks = 0;
  } else if (geschwindigkeitLinks > 255) {
    geschwindigkeitLinks = 255;
  }
  if (line == 0) {
    geschwindigkeitLinks = GRUND_GESCHWINDIGKEIT  + LINKS_DRIFT;
    geschwindigkeitRechts = GRUND_GESCHWINDIGKEIT  + RECHTS_DRIFT;
  }

  if (geschwindigkeitRechts < 0) {
    geschwindigkeitRechts = 0;
  } else if (geschwindigkeitRechts > 255) {
    geschwindigkeitRechts = 255;
  }

  digitalWrite(DIR_MOTOR_LINKS_1, LOW);
  digitalWrite(DIR_MOTOR_RECHTS_1, LOW);
  digitalWrite(DIR_MOTOR_LINKS_2, LOW);
  digitalWrite(DIR_MOTOR_RECHTS_2, LOW);
  int speedMotor1 = geschwindigkeitLinks + AUSGLEICH_MOTOR_LINKS_1;
  if (speedMotor1 < 0) {
    speedMotor1 = 1;
  }
  int speedMotor2 = geschwindigkeitLinks + AUSGLEICH_MOTOR_LINKS_2;
  if (speedMotor2 < 0) {
    speedMotor2 = 1;
  }
  int speedMotor3 = geschwindigkeitRechts + AUSGLEICH_MOTOR_RECHTS_1;
  if (speedMotor3 < 0) {
    speedMotor2 = 1;
  }
  int speedMotor4 = geschwindigkeitRechts + AUSGLEICH_MOTOR_RECHTS_2;
  if (speedMotor4 < 0) {
    speedMotor4 = 1;
  }
  Serial.println(speedMotor2);
  analogWrite(MOTOR_PWM_LINKS_1, speedMotor1);
  analogWrite(MOTOR_PWM_LINKS_2, speedMotor2);
  analogWrite(MOTOR_PWM_RECHTS_1, speedMotor3);
  analogWrite(MOTOR_PWM_RECHTS_2, speedMotor4);
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
  motorSpeedLeft = motorSpeedLeft + LINKS_DRIFT;
  motorSpeedRight = motorSpeedRight + RECHTS_DRIFT;
  digitalWrite(DIR_MOTOR_LINKS_1, directionLeft);
  digitalWrite(DIR_MOTOR_LINKS_2, directionLeft);
  digitalWrite(DIR_MOTOR_RECHTS_1, directionRight);
  digitalWrite(DIR_MOTOR_RECHTS_2, directionRight);
  int speedMotor1 = motorSpeedLeft + AUSGLEICH_MOTOR_LINKS_1;
  if (speedMotor1 < 0) {
    speedMotor1 = 1;
  }
  int speedMotor2 = motorSpeedLeft + AUSGLEICH_MOTOR_LINKS_2;
  if (speedMotor2 < 0) {
    speedMotor2 = 1;
  }
  int speedMotor3 = motorSpeedRight + AUSGLEICH_MOTOR_RECHTS_1;
  if (speedMotor3 < 0) {
    speedMotor2 = 1;
  }
  int speedMotor4 = motorSpeedRight + AUSGLEICH_MOTOR_RECHTS_2;
  if (speedMotor4 < 0) {
    speedMotor4 = 1;
  }
  analogWrite(MOTOR_PWM_LINKS_1, speedMotor1);
  analogWrite(MOTOR_PWM_LINKS_2, speedMotor2);
  analogWrite(MOTOR_PWM_RECHTS_1, speedMotor3);
  analogWrite(MOTOR_PWM_RECHTS_2, speedMotor4);
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
        writeMotor(0, 0, 70, 70, 300);
        datenAuswerten();
         while (noLine) {
            writeMotor(1, 1, 70, 70, 1);
            datenAuswerten();
        }
    }

    if (greenLeft) {
        
    }

    if (greenRight) {
        
    }

    if (greenBoth) {
        
    }

    if (cornerLeft) {
        counter = 0;
        Serial.println("cornerLeft.");
        writeMotor(0, 0, 70, 70, 400);
        while (line < -30) {
            writeMotor(1, 0, 70, 70, 10);
            datenAuswerten();
            counter++;
            if(counter >= 60){
                writeMotor(0, 0, 0, 0, 2000);
                counter = 0;
            }
        }
    }

    if (cornerRight) {
        counter = 0;
        Serial.println("cornerRight.");
        writeMotor(0, 0, 70, 70, 400);
        while (line > 30) {
            writeMotor(0, 1, 70, 70, 10);
            datenAuswerten();
            counter++;
            if(counter >= 60){
                writeMotor(0, 0, 0, 0, 2000);
                counter = 0;
            }
        }
    }
    
    if (line < -70 && noLine == false) {
        writeMotor(1, 1, 70, 70, 50);
        writeMotor(1, 0, 70, 70, 300);
    }

    if (line > 70 && noLine == false) {
        writeMotor(1, 1, 70, 70, 50);
        writeMotor(0, 1, 70, 70, 300);
    }
  }
}
