#include "../lib/HX711/HX711.h"
#include "display.h"

#define CALIBRATE false
#define CALIBRATE_SAMPLES 25

#define DISPLAY_NUMBER_OF_DEVICES 4
#define DISPLAY_CS_PIN 1

#define LOADCELL_DOUT_PIN 2
#define LOADCELL_SCK_PIN 12
#define LOADCELL_SCALE -10970.0
#define LOADCELL_TARE_SAMPLES 20
#define LOADCELL_WEIGHT_SAMPLES 5

#define BLINK_PIN 0

const String usernames[] = { "NAT", "ANNA" };
unsigned int currentUser = 1;

HX711 loadcell = HX711();
LedMatrix ledMatrix = LedMatrix(DISPLAY_NUMBER_OF_DEVICES, DISPLAY_CS_PIN);

void setup() {
	pinMode(BLINK_PIN, OUTPUT);

	// Set up display
	if (CALIBRATE) {
		Serial.begin(9600);
		Serial.println();
		Serial.println("Starting calibration...");
	}
	else {
		ledMatrix.init();
		ledMatrix.setRotation(true);
		ledMatrix.setIntensity(4); // range is 0-15
		displayLoading(ledMatrix);
	}

	// Set up load cell
	loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
	loadcell.tare(LOADCELL_TARE_SAMPLES);

	if (CALIBRATE) {
		loadcell.set_scale();
	}
	else {
		loadcell.set_scale(LOADCELL_SCALE);
	}

	displayText(ledMatrix, usernames[currentUser]);
	delay(2500);
}

void loop() {
	digitalWrite(BLINK_PIN, HIGH);

	if (CALIBRATE) {
		if (loadcell.wait_ready_timeout(500)) {
			float reading = loadcell.get_units(CALIBRATE_SAMPLES);
			Serial.print("Reading: ");
			Serial.println(reading, 2);
		}
		else {
			Serial.println("HX711 not found.");
		}
	}
	else {
		displayValue(ledMatrix, max(loadcell.get_units(LOADCELL_WEIGHT_SAMPLES), 0.0f));
	}

	digitalWrite(BLINK_PIN, LOW);
	delay(25);
}
