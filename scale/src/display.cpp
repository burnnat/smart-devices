#include "display.h"

#define CHECKER_A 0xCC
#define CHECKER_B 0x33

void displayLoading(LedMatrix matrix) {
	matrix.clear();

	for (unsigned int i = 0; i < 32; i++) {
		matrix.setColumn(i, i % 4 < 2 ? CHECKER_A : CHECKER_B);
	}

	matrix.commit();
}

void displayText(LedMatrix matrix, String text) {
	matrix.clear();

	matrix.setTextAlignment(TEXT_ALIGN_CENTER);
	matrix.setText(text);
	matrix.drawText();

	matrix.commit();
}

void displayValue(LedMatrix matrix, float value) {
	String formatted = String(value, 1);
	int divider = formatted.indexOf('.');

	matrix.clear();

	// Draw whole number part
	String wholePart = formatted.substring(0, divider);

	matrix.setText(wholePart);
	matrix.setTextAlignmentOffset(21 - 7 * wholePart.length());
	matrix.drawText();

	// Draw decimal symbol
	matrix.setColumn(22, 0x60);
	matrix.setColumn(23, 0x60);

	// Draw fractional number part
	String fractionPart = formatted.substring(divider + 1, divider + 2);

	matrix.setTextAlignment(TEXT_ALIGN_RIGHT);
	matrix.setText(fractionPart);
	matrix.drawText();

	// Commit matrix buffer
	matrix.commit();
}