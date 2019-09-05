#include <NewPing.h>
#include "GyverTM1637.h"

#define MIN_PULLUP_DIST 2
#define MAX_PULLUP_DIST 30
#define MIN_PULLUP_TIME 500

const int but = 3;
const int us_trig = 4;
const int us_echo = 5;
const int indi_clk = 7;
const int indi_dio = 8;

byte GripTouched = 0, GripPrevTouched = 0;
byte GripStatus, GripPrevStatus;
long StartTime = 0, FinishTime = 0, PrevPullupTime = 0;
boolean HeadOnTopStatus = false, HeadOnTopPrevStatus = false;
long PullupTimeSet[10];
int PullupCounter = 0;
NewPing sonar(us_trig, us_echo, 100);
GyverTM1637 disp(indi_clk, indi_dio);

void setup() {
  Serial.begin(9600);
  pinMode(but, INPUT);
  disp.clear();
  disp.brightness(7);
}

void loop() {  
  long RunTime = millis();
  boolean HeadOnTopStatus = GetCompletedPullup();

  if (ReadGripStatus()) {
    if (GripTouched > 0 && GripPrevTouched == 0) {
      StartTime = RunTime;
      PrevPullupTime = RunTime;
      Serial.println(F("======= Pull-up set started ======="));
    }

    Serial.print(String(RunTime));
    Serial.print(F("\t"));
    PrintGripStatus();
    Serial.println("");

    if (GripTouched == 0 && GripPrevTouched > 0) {
      FinishTime = RunTime;
      Serial.print(F("======= Pull-up set FINISHED , excercise time="));
      Serial.print(String((FinishTime - StartTime) / 1000));
      Serial.print(F(" seconds, "));
      Serial.print(String(PullupCounter));
      Serial.println(F(" Pullups completed. ======="));

      PullupCounter = 0;
    }
  }

  if ((HeadOnTopStatus != HeadOnTopPrevStatus) && (GripTouched > 0)) {
    if (HeadOnTopStatus) {
      if (RunTime - PrevPullupTime > MIN_PULLUP_TIME) {
        PullupTimeSet[PullupCounter] = RunTime - PrevPullupTime; //Store one more pullup
        PrevPullupTime = RunTime;
        PullupCounter++;

        Serial.print(String(PullupCounter));
        Serial.print(F(" done in "));
        Serial.print(String(PullupTimeSet[PullupCounter - 1]));
        Serial.println(F(" mseconds."));
        disp.clear();
        delay(1000);
        disp.scroll(3, PullupCounter, 50);
        delay(400);
      } 
      else {
        Serial.print(String(RunTime - PrevPullupTime));
        Serial.print(F(" mseconds is too fast for pullup!!!! Minimal time is "));
        Serial.println(String(MIN_PULLUP_TIME));
      }
    }
  }

  HeadOnTopPrevStatus = HeadOnTopStatus;
}

boolean ReadGripStatus() {
  int ChangeCounter = 0;
  GripPrevTouched = GripTouched;
  GripTouched = 0;

  GripPrevStatus = GripStatus;

  if (digitalRead(but) == HIGH)
    GripStatus = 1;
  else
    GripStatus = 0;
    
  GripTouched += GripStatus;
  if (GripStatus != GripPrevStatus) 
    ChangeCounter++;

  if (ChangeCounter > 0)
    return true;
  else
    return false;
}

void PrintGripStatus() {
    Serial.print(String(GripStatus));
    Serial.print("\t");
}

boolean GetCompletedPullup() {
  unsigned int distance = sonar.ping_cm();
  
  if (distance > MAX_PULLUP_DIST || distance < MIN_PULLUP_DIST)
    return false;
  else
    return true;
}
