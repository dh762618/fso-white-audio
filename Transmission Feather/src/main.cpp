#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_TestBed.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_MAX1704X.h>
#include <Adafruit_GFX.h>
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>
#include <SPI.h>
#include <bitMap.h>

// definitions
#define  ANALOGIN 18
#define  NEOPIXELOUT 14
#define  DEFAULT_I2C_ADDR 0x30
#define  d2 GPIO_NUM_2

// Declare Interfaces with Battery and Display
Adafruit_MAX17048 battery;
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// global variable to determine if the device is "off" or not
bool isOff = 0;

// Function declarations
void displayIntro();
void printDisplay();

void setup() {
  Serial.begin(115200);

  display.setCursor(0,0);

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

  // Pin Mode Configuration
  pinMode(d2, INPUT_PULLDOWN); // Button D2
  pinMode(A0, OUTPUT); // Power switch detection

  // Turn Teensy on
  digitalWrite(A0, 3.3);

  displayIntro();

}
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void loop() {
  // put your main code here, to run repeatedly:
  // display.fillScreen(ST77XX_BLACK);
  // display.setCursor(0,0);
  // display.setTextColor(ST77XX_WHITE);
  // display.setTextWrap(true);
  // display.print("Batt Level: ");
  // display.print(battery.cellPercent());
  // display.println(" %");
  // display.print("Batt Voltage: ");
  // display.print(battery.cellVoltage());
  // display.println(" V");

  // Check power switch and buttons
  // Check to see if the physical switch has been pressed --
  // If D1 is pressed, only the Feather is turned off
  if (digitalRead(d2)){
    if (!isOff){
      // Print that we are turning the power off to the Feather
      display.fillScreen(ST77XX_BLACK);
      display.drawBitmap(5, 60, epd_bitmap_power, 25, 25, ST77XX_WHITE);
      display.setCursor(30, 65);
      display.setTextColor(ST77XX_WHITE);
      display.print(" Powering Off...");
      delay(4200);
      // Turn off TFT to save power
      digitalWrite(A0, 0);
      digitalWrite(TFT_BACKLITE, LOW);
      digitalWrite(TFT_I2C_POWER, LOW);
      isOff = 1;
    } else {
      // Turn TFT back on
      digitalWrite(TFT_BACKLITE, HIGH);
      digitalWrite(TFT_I2C_POWER, HIGH);
      digitalWrite(A0, 3.3);
      displayIntro();
      isOff = 0;
    }
  }
  // Print information to display
  printDisplay();
  delay(500);
  // Clear screen for next print
  display.fillScreen(ST77XX_BLACK);
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void displayIntro()
{
  display.setCursor(0,0);
  display.setTextColor(ST77XX_WHITE);
  display.setTextWrap(false);
  display.print("FSO White Team\n");
  display.print("Transmission Feather");
  delay(2000);
  display.fillScreen(ST77XX_BLACK);

  //void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
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
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void printDisplay(){
  // Battery management
  display.setCursor(100, 0);
  display.setTextColor(ST77XX_BLACK);
  if (battery.cellPercent() <= 20){
    display.setTextColor(ST77XX_RED);
    display.drawBitmap(40, 0, epd_bitmap_battery, 64, 40, ST77XX_RED);
  } else if (battery.cellPercent() >= 50){
    display.setTextColor(ST77XX_GREEN);
    display.drawBitmap(40, 0, epd_bitmap_battery, 64, 40, ST77XX_GREEN);
  } else if (battery.cellPercent() < 50 && battery.cellPercent() > 20){
    display.setTextColor(ST77XX_YELLOW);
    display.drawBitmap(40, 0, epd_bitmap_battery, 64, 40, ST77XX_YELLOW);
  }
  display.setCursor(104,15);
  display.print(battery.cellPercent());
  display.println("%");

  // display pre amp gain
  display.drawBitmap(45, 50, epd_bitmap_gain, 40, 40, ST77XX_WHITE);
  display.setCursor(95, 60);
  display.setTextColor(ST77XX_WHITE);
  display.print("+    dB");

  // separate power button from battery and gain functions
  display.drawLine(35, 0, 35, 135, ST77XX_WHITE);

  // power button
  display.drawBitmap(0,110, epd_bitmap_power, 25, 25, ST77XX_WHITE);
  
  // transmission signal indicator
  //display.drawBitmap(190, 85, epd_bitmap_, 50, 50, ST77XX_WHITE);
}