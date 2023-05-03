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
#define  d2 GPIO_NUM_2 // power button d2
#define  d1 GPIO_NUM_1 // Mute button for slider

// Declare Interfaces with Battery and Display
Adafruit_MAX17048 battery;
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// global variable to determine if the device is "off" or not
bool isOff = 0;
int slider_val = 0;
float prev_percent = 0;
bool muted = 0;

// Function declarations
void displayIntro();
void printDisplay();
void powerManagement();

void setup() {
  Serial.begin(9600);
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
  pinMode(d2, INPUT_PULLDOWN); // Power switch detection
  pinMode(A0, OUTPUT); // Teensy Power
  pinMode(GPIO_NUM_5, OUTPUT); // Mute

  // Turn Teensy on
  digitalWrite(A0, HIGH);

  displayIntro();
}
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void loop() {

  powerManagement();
  
  // Clear the battery percentage 
  if (battery.cellPercent() != prev_percent){
    display.fillRect(104, 15, 100, 15, ST77XX_BLACK);
  }

  if (digitalRead(d1)){
    if (!muted){
      digitalWrite(GPIO_NUM_5, HIGH);
    } else{
      digitalWrite(GPIO_NUM_5, LOW);
    }
  }

  // output display
  printDisplay();

  prev_percent = battery.cellPercent();
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void displayIntro()
{
  // display the team name in the middle of the display
  display.setCursor(0,45);
  display.setTextColor(ST77XX_WHITE);
  display.setTextWrap(false);
  display.println("   FSO White Team");
  display.println("Transmission Feather");
  display.println("    April 2023");
  delay(2000);
  display.fillScreen(ST77XX_BLACK);

  // void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
  // print Dan's name and picture then clear the screen after 2 seconds
  display.drawBitmap(0,0, epd_bitmap_Dan_Allwine_300x450, 240, 135, ST77XX_WHITE);
  display.setCursor(60, 120);
  display.print("Dan Allwine");
  delay(2000);
  display.fillScreen(ST77XX_BLACK);
  
  // print Daniel's name and picture
  display.drawBitmap(0,0, epd_bitmap_Daniel, 240, 120, ST77XX_WHITE);
  display.setCursor(60, 120);
  display.print("Daniel Cobb");
  delay(2000);
  display.fillScreen(ST77XX_BLACK);

  // print Remi's name and picture
  display.drawBitmap(0,0, epd_bitmap_Remi, 240, 120, ST77XX_WHITE);
  display.setCursor(60, 120);
  display.print("Remi Fordyce");
  delay(2000);
  display.fillScreen(ST77XX_BLACK);

  // print Devin's name and picture
  display.drawBitmap(0,0, epd_bitmap_Devin, 240, 120, ST77XX_WHITE);
  display.setCursor(60, 120);
  display.print("Devin Holt");
  delay(2000);
  display.fillScreen(ST77XX_BLACK);

  // print Carson's name and picture
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

  // if the battery is less than 20% change the battery and text to red
  if (battery.cellPercent() <= 20)
  {
    display.setTextColor(ST77XX_RED);
    display.drawBitmap(40, 0, epd_bitmap_battery, 64, 40, ST77XX_RED);
  } 
  // if the battery is greater than 50% change the battery and text to green
  else if (battery.cellPercent() >= 50)
  {
    display.setTextColor(ST77XX_GREEN);
    display.drawBitmap(40, 0, epd_bitmap_battery, 64, 40, ST77XX_GREEN);
  } 
  // if the battery is between 20-60% change the battery and text to yellow
  else if (battery.cellPercent() < 60 && battery.cellPercent() > 20)
  {
    display.setTextColor(ST77XX_YELLOW);
    display.drawBitmap(40, 0, epd_bitmap_battery, 64, 40, ST77XX_YELLOW);
  }

  // once the battery and text color is changed display the actual battery percent
  display.setCursor(104,15);
  display.print(battery.cellPercent());
  display.println("%");


  // display pre amp gain
  display.drawBitmap(45, 50, epd_bitmap_gain, 40, 40, ST77XX_WHITE);
  display.setCursor(95, 60);
  display.setTextColor(ST77XX_WHITE);
  display.print("+ ");
  display.print(slider_val);
  display.print(" dB");

  // separate power button from battery and gain functions
  display.drawLine(35, 0, 35, 135, ST77XX_WHITE);

  // power button
  display.drawBitmap(0,110, epd_bitmap_power, 25, 25, ST77XX_WHITE);
  
  // transmission signal indicator
  display.drawBitmap(200, 100, epd_bitmap_transmission, 40, 40, ST77XX_WHITE);
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void powerManagement()
{
  // Check to see if the physical switch has been pressed
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
      digitalWrite(A0, LOW);
      digitalWrite(TFT_BACKLITE, LOW);
      digitalWrite(TFT_I2C_POWER, LOW);
      isOff = 1;
    } else {
      // Turn TFT back on
      digitalWrite(TFT_BACKLITE, HIGH);
      digitalWrite(TFT_I2C_POWER, HIGH);
      digitalWrite(A0, HIGH);
      display.fillScreen(ST77XX_BLACK);
      displayIntro();
      isOff = 0;
    }
  }
}