#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C
    lcd(0x27, 16,
        2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  lcd.init(); // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Hello World!");
  lcd.setCursor(0, 1);
  lcd.print("aprendiendoarduino!");
  delay(5000);
}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(millis()/1000);
  delay(1000);
}