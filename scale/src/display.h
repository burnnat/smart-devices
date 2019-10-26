#include <Arduino.h>
#include "../lib/MAX7219LedMatrix/LedMatrix.h"

void displayLoading(LedMatrix matrix);
void displayText(LedMatrix matrix, String text);
void displayValue(LedMatrix matrix, float value);