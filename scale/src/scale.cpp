#include "../lib/HX711/HX711.h"
#include "display.h"
#include "network.h"
#include "secrets.h"

#define CALIBRATE false
#define CALIBRATE_SAMPLES 25

#define DISPLAY_CS_PIN 1

#define LOADCELL true
#define LOADCELL_DOUT_PIN 2
#define LOADCELL_SCK_PIN 12
#define LOADCELL_SCALE -10970.0
#define LOADCELL_TARE_SAMPLES 20
#define LOADCELL_WEIGHT_SAMPLES 5

#define LED_BLUE_PIN 15
#define LED_YELLOW_PIN 16

#define BUTTON_BLUE_PIN 4
#define BUTTON_YELLOW_PIN 5

#define ENABLE_PIN 0

// See battery voltage calculations here: https://learn.adafruit.com/using-ifttt-with-adafruit-io/arduino-code-2
#define BATT_MAX_LEVEL 774
#define BATT_LOW_LEVEL 600
#define BATT_MIN_LEVEL 580

#define WEIGHT_SUBMIT_COUNT 10
#define IDLE_SLEEP_COUNT 100

String usernames[] = { "NAT", "ANNA" };
byte ledPins[] = { LED_BLUE_PIN, LED_YELLOW_PIN };
int buttonPins[] = { BUTTON_BLUE_PIN, BUTTON_YELLOW_PIN };

int userCount = 2;
int currentUser = -1;
volatile int setUser = -1;

int idleCount = 0;
int weightCount = 0;
float currentWeight = 0.0f;

ICACHE_RAM_ATTR void user1Select() {
	setUser = 0;
}

ICACHE_RAM_ATTR void user2Select() {
	setUser = 1;
}

typedef void (*Callback)();
Callback interrupts[] = { user1Select, user2Select };

HX711 loadcell = HX711();
Display display = Display(DISPLAY_CS_PIN, userCount, ledPins, usernames);
Network network = Network();

void setup() {
	// Set up enable pin first, to prevent reset on button press
	pinMode(ENABLE_PIN, OUTPUT);
	digitalWrite(ENABLE_PIN, LOW);

	// Set up buttons
	for (int i = 0; i < userCount; i++) {
		int pin = buttonPins[i];
		pinMode(pin, INPUT_PULLUP);
		attachInterrupt(digitalPinToInterrupt(pin), interrupts[i], RISING);
	}

	// Set up display
	if (CALIBRATE) {
		Serial.begin(9600);
		Serial.println();
		Serial.println("Starting calibration...");
	}
	else {
		display.init();
		display.displayLoading();

		network.connect(SECRET_WIFI_SSID, SECRET_WIFI_PASSWORD);
	}

	// Set up load cell
	if (LOADCELL) {
		loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
		loadcell.tare(LOADCELL_TARE_SAMPLES);

		if (CALIBRATE) {
			loadcell.set_scale();
		}
		else {
			loadcell.set_scale(LOADCELL_SCALE);
		}
	}
	else {
		delay(1000);
	}

	if (!CALIBRATE) {
		net_status_t status;
		
		status = network.waitConnected(60000);

		if (status != NET_SUCCESS) {
			display.displayError(status);
			return;
		}

		status = network.connectMqtt(SECRET_MQTT_HOST, SECRET_MQTT_PORT, SECRET_MQTT_USERNAME, SECRET_MQTT_PASSWORD, 2000);

		if (status != NET_SUCCESS) {
			display.displayError(status + 4);
			return;
		}
	}

	// Check battery level
	int level = analogRead(A0);
	network.submitBattery(level);

	if (level < BATT_LOW_LEVEL) {
		display.displayBatteryWarning();
		delay(1000);
	}

	display.displaySelect();
}

void loop() {
	if (setUser > -1) {
		bool update = setUser != currentUser;
		currentUser = setUser;
		setUser = -1;

		if (update) {
			weightCount = 0;
			idleCount = 0;

			display.displayUser(currentUser);
			delay(2500);
		}
	}
	else if (currentUser > -1) {
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
				float nextWeight = max(loadcell.get_units(LOADCELL_WEIGHT_SAMPLES), 0.0f);
				display.displayValue(nextWeight);

				float diff = currentWeight - nextWeight;
				if (abs(diff) < 0.1f) {
					weightCount++;
				}
				else {
					weightCount = 0;
				}

				currentWeight = nextWeight;
			}
		}
		else {
			display.displayError(3);
			delay(100);
			weightCount++;
		}

		if (weightCount >= WEIGHT_SUBMIT_COUNT) {
			network.submitWeight(usernames[currentUser], currentWeight);

			// Blink display to indicate complete
			display.clear();
			delay(500);
			display.displayValue(currentWeight);
			delay(1000);
			display.clear();
			delay(500);
			display.displayValue(currentWeight);
			delay(2000);

			// Reset to select user
			currentUser = -1;
			display.displaySelect();
			delay(1000);
		}
	}
	else {
		idleCount++;

		if (idleCount >= IDLE_SLEEP_COUNT) {
			display.clear();
			display.shutdown();
			ESP.deepSleep(0);
		}
		else {
			delay(100);
		}
	}
}
