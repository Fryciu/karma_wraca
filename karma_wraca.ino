// Serial commands list:
// Input commands (received by Arduino)::
// - PING - respond with PING
// - EXTRUDE - extrude karma (move servo) (and report DONE)
// Output commands (returned by Arudino):
// - PING - response to PING command
// - DONE - reported when actions (EXTRUDE fro now) is complete

#include <Servo.h>
#include <Ds1302.h>
#include <time.h>

const auto SERVO_PWM = 9;
const auto TickDelay = 200; // ms
const int MAX_CMD_SIZE = 10;
const int CLOCK_CLK = 5;
const int CLOCK_DAT = 6;
const int CLOCK_RESET = 7;
const int EXTRUDE_DELAY = 5;

Servo feeder;
Ds1302 rtcClock(CLOCK_RESET,CLOCK_CLK, CLOCK_DAT);
char command[MAX_CMD_SIZE]; // max of 10 chars

void setup() {
  pinMode(SERVO_PWM, OUTPUT);
  feeder.attach(SERVO_PWM);
  feeder.write(180);
  Serial.begin(9600);
  rtcClock.init();
  Serial.println("// Karma wraca - Hello!");
}

void loop() {
  //Serial.println("// loop ");
  // read things from the serial
  if (Serial.available()) {
    auto command = Serial.readStringUntil('\n');
    execCommand(command);
  }

  delay(TickDelay);
}

void extrudeKarma() {
  Serial.println("//start move");
  // Move servo from 0 to 180 degrees
  for (int i = 180; i > 0; i--) {     // scale it to use it with the servo (value between 0 and 180)
    feeder.write(i);                  // sets the servo position according to the scaled value
    delay(EXTRUDE_DELAY);                           // waits for the servo to get there
  }
  
  // Move servo from 180 back to 0 degrees
  for (int i = 180; i > 0; i--) {
    feeder.write(180-i);                  // sets the servo position according to the scaled value
    delay(EXTRUDE_DELAY);                           // waits for the servo to get there
  }
  Serial.println("//move done");
}

void execCommand(String cmd) {
  if (cmd == "EXTRUDE") {
      extrudeKarma();
      Serial.println("DONE");
  } else if (cmd == "TIME") {
      Serial.println("// - reporting time");
      Serial.println(getDS1302UnixTime());
  } else if (cmd == "SETTIME") {
      Serial.println("// - settime received");
      setTime();
  } else if (cmd == "PING") {
      Serial.println("PING");
  } else {
      Serial.println("//ERROR: unknown command!");
      Serial.println(cmd);
      //while (1);
  }
}

unsigned long getDS1302UnixTime() {
  Ds1302::DateTime now;
  rtcClock.getDateTime(&now);

  tm* timeInfo = new tm;
  timeInfo->tm_year = now.year+100; 
  timeInfo->tm_mon = now.month;
  timeInfo->tm_mday = now.day;
  timeInfo->tm_hour = now.hour;
  Serial.print("// Hour: ");
  Serial.println(timeInfo->tm_hour);
  timeInfo->tm_min = now.minute;
  Serial.print("// min: ");
  Serial.println(timeInfo->tm_min);
  timeInfo->tm_sec = now.second;
  timeInfo->tm_isdst = 0;

  // Calculate unix time
  time_t unixTime = mktime(timeInfo);
  Serial.print("// unix time: ");
  Serial.println(unixTime);

  // Convert to seconds
  return unixTime;
}

void setTime() {
  if (!Serial.available()) {
    Serial.println("// FATAL: no data on serial - time expected");
    while(1);
  }

  auto x = Serial.readStringUntil('\n');
  Ds1302::DateTime dt;
  // decode unix time
  auto unixTime = time_t(x.toInt());
  // convert to date time
  struct tm* timeInfo = localtime(&unixTime);
  Serial.print("//");
  Serial.print(unixTime);
  Serial.print("\n");
  dt.year = timeInfo->tm_year-100;
  dt.month = timeInfo->tm_mon;
  dt.day = timeInfo->tm_mday;
  dt.hour = timeInfo->tm_hour;
  Serial.print("// Hour: ");
  Serial.println(timeInfo->tm_hour);
  dt.minute = timeInfo->tm_min;
  Serial.print("// min: ");
  Serial.println(timeInfo->tm_min);
  dt.second = timeInfo->tm_sec;
  // set time to DS1302
  rtcClock.setDateTime(&dt);
  Serial.println("DONE");
}

uint8_t parseDigits(char* str, uint8_t count) {
    uint8_t val = 0;
    while(count-- > 0) val = (val * 10) + (*str++ - '0');
    return val;
}
