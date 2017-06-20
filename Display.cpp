#include "Display.h"
#include "LiquidCrystal.h"

Display display;

LiquidCrystal lcd(PIN_RS, PIN_ENABLE, PIN_DB4, PIN_DB5, PIN_DB6, PIN_DB7);

void Display::init() {
  lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  lcd.print("hello");
}

void Display::test() {
  delay(100);
  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);
}
