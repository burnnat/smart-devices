#include "display.h"

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