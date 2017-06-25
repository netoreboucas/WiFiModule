#include "Display.h"
Display display;

LiquidCrystal lcd(PIN_RS, PIN_ENABLE, PIN_DB4, PIN_DB5, PIN_DB6, PIN_DB7);

void Display::init() {
  lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  lcd.clear();
}

void Display::clear() {
  lcd.clear();
}

void Display::setText(String line1, String line2) {
  clear();
  if (line1 != NULL && line1 != NULL_STR) lcd.print(line1);
  lcd.setCursor(0, 1);
  if (line2 != NULL && line2 != NULL_STR) lcd.print(line2);
}

void Display::append(String line) {
  if (line != NULL && line != NULL_STR) {
    line.trim();
    if (line.length() > 0) {
      lineOne = lineTwo;
      lineTwo = line;
      setText(lineOne, lineTwo);
    }
  }
}

