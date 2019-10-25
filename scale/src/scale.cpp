#include <SPI.h>
#include "display.h"

#define NUMBER_OF_DEVICES 4
#define CS_PIN 1
#define BLINK_PIN 2

LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CS_PIN);
float count = 0.0;

void setup() {
	pinMode(BLINK_PIN, OUTPUT);
	ledMatrix.init();
	ledMatrix.setRotation(true);
	ledMatrix.setIntensity(4); // range is 0-15
}

void loop() {
	digitalWrite(BLINK_PIN, HIGH);
	delay(50);

	displayValue(ledMatrix, count);
	count += 0.1;

	digitalWrite(BLINK_PIN, LOW);
	delay(50);
}
