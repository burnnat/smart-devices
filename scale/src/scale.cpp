#include <SPI.h>
#include "../lib/MAX7219LedMatrix/LedMatrix.h"

#define NUMBER_OF_DEVICES 4
#define CS_PIN 1
#define BLINK_PIN 2

LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CS_PIN);

void setup() {
  pinMode(BLINK_PIN, OUTPUT);
  ledMatrix.init();
  ledMatrix.setIntensity(4); // range is 0-15
  ledMatrix.setText("Hello World!");
}

void loop() {
  digitalWrite(BLINK_PIN, HIGH);
  ledMatrix.clear();
  ledMatrix.scrollTextLeft();
  ledMatrix.drawText();
  ledMatrix.commit(); // commit transfers the byte buffer to the displays
  delay(50);
  digitalWrite(BLINK_PIN, LOW);
  delay(50);
}
