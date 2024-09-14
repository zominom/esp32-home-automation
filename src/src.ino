#include "LedCommands.h"
#include "config.h"

#define PIR_PIN 4

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
      Serial.println("Motion ended!");
      pirState = LOW;
    }
  }
}
