#include <Servo.h>

const auto SERVO_PWM = 9;
const auto TickDelay = 200; // ms
const int MAX_CMD_SIZE = 10;

Servo feeder;
char command[MAX_CMD_SIZE]; // max of 10 chars

void setup() {
  feeder.attach(SERVO_PWM);
  Serial.begin(9600);
  Serial.println("// Karma wraca - Hello!");
}

void loop() {
  //Serial.println("// loop ");
  // read things from the serial
  for (int n = 0; Serial.available() > 0; n++) {
    Serial.println("// loop");
    if (n >= MAX_CMD_SIZE) {
      Serial.println("//ERROR: command too long! Trapping");
      while (1);
    }

    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      execCommand(command);
      for (int i = 0; i < MAX_CMD_SIZE; i++) {
        command[i] = 0;
      }
      break; // need to reset the loop to reset n
    }

    command[n] = c;
  }

  delay(TickDelay);
}

void extrudeKarma() {
  // Move servo from 0 to 180 degrees
  for (int i = 0; i < 180; i++) {     // scale it to use it with the servo (value between 0 and 180)
    feeder.write(i);                  // sets the servo position according to the scaled value
    delay(15);                           // waits for the servo to get there
  }
  
  // Move servo from 180 back to 0 degrees
  for (int i = 0; i < 180; i++) {
    feeder.write(180-i);                  // sets the servo position according to the scaled value
    delay(15);                           // waits for the servo to get there
  }
}

void execCommand(const char* cmd) {
  if (strcmp(cmd, "EXTRUDE") == 0) {
      Serial.println("// - starting!");
  } else if (strcmp(cmd, "TIME") == 0) {
      Serial.println("// - reporting time");
  } else if (strcmp(cmd, "PING") == 0) {
    Serial.println("PING");
  } else {
    Serial.println("//ERROR: unknown command!");
    Serial.println(cmd);
    //while (1);
  }
}
