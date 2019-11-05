#include "display.h"

#define DISPLAY_NUMBER_OF_DEVICES 4

#define CHECKER_A 0xCC
#define CHECKER_B 0x33

Display::Display(byte matrixSelectPin, int userCount, byte userLedPins[], String userNames[]):
	_matrix(DISPLAY_NUMBER_OF_DEVICES, matrixSelectPin),
	_userCount(userCount),
	_userLedPins(userLedPins),
	_userNames(userNames)
{}

void Display::init() {
	for (int i = 0; i < _userCount; i++) {
		pinMode(_userLedPins[i], OUTPUT);
	}

	_matrix.init();
	_matrix.setRotation(true);
	_matrix.setIntensity(2); // range is 0-15
}

void Display::displayLoading() {
	_matrix.clear();

	for (unsigned int i = 0; i < 32; i++) {
		_matrix.setColumn(i, i % 4 < 2 ? CHECKER_A : CHECKER_B);
	}

	_matrix.commit();
}

void Display::displayText(String text) {
	_matrix.clear();

	_matrix.setTextAlignment(TEXT_ALIGN_CENTER);
	_matrix.setText(text);
	_matrix.drawText();

	_matrix.commit();
}

void Display::displayValue(float value) {
	String formatted = String(value, 1);
	int divider = formatted.indexOf('.');

	_matrix.clear();

	// Draw whole number part
	String wholePart = formatted.substring(0, divider);

	_matrix.setText(wholePart);
	_matrix.setTextAlignmentOffset(21 - 7 * wholePart.length());
	_matrix.drawText();

	// Draw decimal symbol
	_matrix.setColumn(22, 0x60);
	_matrix.setColumn(23, 0x60);

	// Draw fractional number part
	String fractionPart = formatted.substring(divider + 1, divider + 2);

	_matrix.setTextAlignment(TEXT_ALIGN_RIGHT);
	_matrix.setText(fractionPart);
	_matrix.drawText();

	// Commit matrix buffer
	_matrix.commit();
}

void Display::displayUser(int user) {
	for (int i = 0; i < _userCount; i++) {
		digitalWrite(_userLedPins[i], i == user ? HIGH : LOW);
	}

	displayText(_userNames[user]);
}

void Display::displaySelect() {
	displayText("USER");
}

void Display::displayError() {
	displayText("ERR");
}