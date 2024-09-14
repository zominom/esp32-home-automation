#include "LedCommands.h"

#define PIR_PIN 4

const char* CONTROLLER_ADDR = "be:27:de:00:2e:ca";
const char* SERVICE_UUID = "FFF0";
const char* CHAR_UUID = "FFF3";

int pirState = LOW;

bool is_on = false;

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
}

void loop() {
  bool pirVal = digitalRead(PIR_PIN) == HIGH;
  if (pirVal && pirState == LOW) {
    Serial.println("Motion detected");

    LedCommands led_cmd(CONTROLLER_ADDR, SERVICE_UUID, CHAR_UUID);
    if (led_cmd.is_initialized())
    {
      led_cmd.set_power(is_on);
      is_on = !is_on;
    }

    pirState = HIGH;
    delay(3000);
  }
  else
  {
    if (pirState == HIGH)
    {
      // we have just turned of
      Serial.println("Motion ended!");
      pirState = LOW;
    }
  }
}
