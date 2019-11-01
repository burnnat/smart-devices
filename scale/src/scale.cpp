#include "../lib/HX711/HX711.h"
#include "display.h"

#define CALIBRATE false
#define CALIBRATE_SAMPLES 25

#define DISPLAY_CS_PIN 1

#define LOADCELL false
#define LOADCELL_DOUT_PIN 2
#define LOADCELL_SCK_PIN 12
#define LOADCELL_SCALE -10970.0
#define LOADCELL_TARE_SAMPLES 20
#define LOADCELL_WEIGHT_SAMPLES 5

#define LED_BLUE_PIN 15
#define LED_YELLOW_PIN 16

#define BUTTON_BLUE_PIN 4
#define BUTTON_YELLOW_PIN 5

#define BLINK_PIN 0

String usernames[] = { "NAT", "ANNA" };
byte ledPins[] = { LED_BLUE_PIN, LED_YELLOW_PIN };
int buttonPins[] = { BUTTON_BLUE_PIN, BUTTON_YELLOW_PIN };

unsigned int userCount = 2;
unsigned int currentUser = 1;

HX711 loadcell = HX711();
Display display = Display(DISPLAY_CS_PIN, userCount, ledPins, usernames);

void setup() {
	pinMode(BLINK_PIN, OUTPUT);

	// Set up display
	if (CALIBRATE) {
		Serial.begin(9600);
		Serial.println();
		Serial.println("Starting calibration...");
	}
	else {
		display.init();
		display.displayLoading();
	}

	if (LOADCELL) {
		// Set up load cell
		loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
		loadcell.tare(LOADCELL_TARE_SAMPLES);

		if (CALIBRATE) {
			loadcell.set_scale();
		}
		else {
			loadcell.set_scale(LOADCELL_SCALE);
		}
	}

	display.displayUser(currentUser);
	delay(2500);
}

void loop() {
	digitalWrite(BLINK_PIN, HIGH);

	if (LOADCELL) {
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
			display.displayValue(max(loadcell.get_units(LOADCELL_WEIGHT_SAMPLES), 0.0f));
		}
	}
	else {
		display.displayError();
	}

	digitalWrite(BLINK_PIN, LOW);
	delay(25);
}
