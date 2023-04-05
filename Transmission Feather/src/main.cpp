#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_TestBed.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_MAX1704X.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMono9pt7b.h>
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>
#include <bitMap.h>

#define  ANALOGIN 18
#define  NEOPIXELOUT 14
#define  DEFAULT_I2C_ADDR 0x30

// Declare Interfaces with Battery and Display
Adafruit_MAX17048 battery;
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


void setup() {
  Serial.begin(115200);

  display.setCursor(0,0);
  //display.setFont(&FreeMono9pt7b);

  //Set up the battery that is connected to the feather
  if (!battery.begin()) {
    Serial.println("Couldnt find Adafruit MAX17048?\nMake sure a battery is plugged in!");
    while (1) delay(10);
  }
  Serial.print("Found MAX17048");
  Serial.print(" with Chip ID: 0x"); 
  Serial.println(battery.getChipID());

  // Set up the screen that is connected to the feather

  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // initialize display
  display.init(135, 240); // Init ST7789 240x135
  display.setRotation(3);
  display.fillScreen(ST77XX_BLACK);
  display.setTextColor(ST77XX_WHITE);
  display.setTextSize(2);

  display.setCursor(0,0);
  display.setTextColor(ST77XX_WHITE);
  display.setTextWrap(false);
  display.print("FSO White Team\n");
  display.print("Transmission Feather");
  delay(2000);
  display.fillScreen(ST77XX_BLACK);

  display.drawBitmap(0,0, epd_bitmap_Dan_Allwine_300x450, 240, 135, ST77XX_WHITE);
  display.setCursor(60, 120);
  display.print("Dan Allwine");
  delay(2000);
  display.fillScreen(ST77XX_BLACK);

  display.drawBitmap(0,0, epd_bitmap_Daniel, 240, 120, ST77XX_WHITE);
  display.setCursor(60, 120);
  display.print("Daniel Cobb");
  delay(2000);
  display.fillScreen(ST77XX_BLACK);

  display.drawBitmap(0,0, epd_bitmap_Remi, 240, 120, ST77XX_WHITE);
  display.setCursor(60, 120);
  display.print("Remi Fordyce");
  delay(2000);
  display.fillScreen(ST77XX_BLACK);

  display.drawBitmap(0,0, epd_bitmap_Devin, 240, 120, ST77XX_WHITE);
  display.setCursor(60, 120);
  display.print("Devin Holt");
  delay(2000);
  display.fillScreen(ST77XX_BLACK);

  display.drawBitmap(0,0, epd_bitmap_Carson, 240, 120, ST77XX_WHITE);
  display.setCursor(60, 120);
  display.print("Carson Magni");
  delay(2000);
  display.fillScreen(ST77XX_BLACK);
}
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void loop() {
  // put your main code here, to run repeatedly:
  display.fillScreen(ST77XX_BLACK);
  display.setCursor(0,0);
  display.setTextColor(ST77XX_WHITE);
  display.setTextWrap(true);
  display.print("Batt Level: ");
  display.print(battery.cellPercent());
  display.println(" %");
  display.print("Batt Voltage: ");
  display.print(battery.cellVoltage());
  display.println(" V");

  //void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
  //display.drawBitmap(0, 50, epd_bitmap_Sound, 25, 25, ST77XX_WHITE);
  
  display.flush();

  delay(8);
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// allows for neosldier to change colors as the potentiometer is adjusted
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return seesaw_NeoPixel::Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return seesaw_NeoPixel::Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return seesaw_NeoPixel::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////