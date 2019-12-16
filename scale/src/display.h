#include <Arduino.h>
#include "../lib/MAX7219LedMatrix/LedMatrix.h"

class Display {
	
	public:
		Display(byte matrixSelectPin, int userCount, byte userLedPins[], String userNames[]);

		void init();
		void clear();
		void shutdown();

		void displayLoading();
		void displayUser(int user);
		void displayValue(float value);
		void displaySelect();
		void displayBatteryWarning();
		void displayError();

	private:
		LedMatrix _matrix;
		int _userCount;
		byte* _userLedPins;
		String* _userNames;

		void displayText(String text);
};