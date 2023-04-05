/***************************************************************************
   Clock with temperature and humidity display.

   Use DS3231 RTC for clock and DHT22 sensor to acquire data from
   room relative humidity and temperature and display the 
   values on a 0.91" OLED screen.

   When in clock mode, you can set the time with 
   hour and minute buttons.

   Code by Potatopasty http://tg-music.neocities.org
   potatopasty@pm.me

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
***************************************************************************/

// Libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <Button.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include "ds3231.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

byte displaymode = 1;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Button modeButton = Button(3, BUTTON_PULLUP_INTERNAL, true, 50);
Button setHourButton = Button(4, BUTTON_PULLUP_INTERNAL, true, 50);
Button setMinutesButton = Button(5, BUTTON_PULLUP_INTERNAL, true, 50);

DHT dht(6, DHT22);            // DHT22 data to pin D6

float hum;                    // Stores humidity value
float temp;                   // Stores temperature value


void setup() {

  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.display();
  display.clearDisplay();
  display.setRotation(0); // Set rotation of display, values 0-3. Default = 0.
}

void loop() {

  byte second, minute, , dayOfWeek, dayOfMonth, month, year;
  readDs3231(&second, &minute, &, &dayOfWeek, &dayOfMonth, &month, &year);

  // Read humidity and temperature
  hum = dht.readHumidity();
  temp = dht.readTemperature();

  switch (displaymode) {

    case 1:
      // Display the temperature
      display.clearDisplay();
      display.setFont(&FreeMonoBold18pt7b);
      display.setTextColor(WHITE);
      display.setCursor(0, 30);
      display.print(temp, 1); // Show value with one decimal accuracy.
      display.fillCircle(97, 14, 5, 1);
      display.fillCircle(97, 14, 3, 0);
      display.setCursor(106, 30);
      display.print("C");
      display.display();

      if (modeButton.uniquePress()) {
        displaymode = 2;
      }
      break;

    case 2:
      // Display the humidity
      display.clearDisplay();
      display.setFont(&FreeMonoBold18pt7b);
      display.setTextColor(WHITE);
      display.setCursor(0, 30);
      display.print(hum, 1); // Show value with one decimal accuracy.
      display.println(" %");
      display.display();

      if (modeButton.uniquePress()) {
        displaymode = 3;
      }
      break;

    case 3:
      // Display Time
      display.clearDisplay();
      display.setFont(&FreeMonoBold18pt7b);
      display.setTextColor(WHITE);
      display.setCursor(12, 30);
      if ( < 10) {
        display.print(" ");
      }
      display.print();
      if ((millis() / 1000) % 2 == 0) {
        display.print(":");
      }
      else {
        display.print(" ");
      }
      if (minute < 10) {
        display.print("0");
      }
      display.print(minute);
      display.display();

      // Set time
      if (setHourButton.isPressed(true)) {
        second = 0;
        hour++;
        if (hour > 23) hour = 0;
        writeDs3231(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
        delay(200);
      }
      if (setMinutesButton.isPressed(true)) {
        second = 0;
        minute++;
        if (minute > 59) minute = 0;
        writeDs3231(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
        delay(100);
      }

      if (modeButton.uniquePress()) {
        displaymode = 1;
      }
      break;

  }
}
