#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_NeoPixel.h>
#include <Adafruit_TestBed.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_MAX1704X.h>
#include <Adafruit_seesaw.h>
#include <SPI.h>
#include <./symbols.h>
#include <Adafruit_BusIO_Register.h>
#include <HardwareSerial.h>

// Definitions
#define d2 GPIO_NUM_2 // button d2 - power
#define d1 GPIO_NUM_1 // button d1 - volume

// Declare Interfaces with Battery and Display
Adafruit_MAX17048 battery;
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Global Vars
bool isMuted = 0;
bool isOff = 0;
float volume = 0.0;
float prev_percent = 0; 
float prev_volume = 0;
int prev_mute = 0;

// Function declarations
bool buttonPressed;
void buttonInterrupt();
void updateDisplay(int textColor);
double getVolume();
void testfastlines(uint16_t color);
void showIntro();
void powerManagement();

void setup() {
  Serial1.begin(9600);
  
  //Set up the battery that is connected to the feather
  if (!battery.begin()) {
    Serial.println("Couldnt find Adafruit MAX17048?\nMake sure a battery is plugged in!");
    while (1) delay(10);
  }
  Serial.print("Found MAX17048");
  Serial.print(" with Chip ID: 0x"); 
  Serial.println(battery.getChipID());
  // Set up the screen that is connected to the feather

  // turn on backlight
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // initialize TFT
  display.init(135, 240); // Init ST7789 240x135
  display.setRotation(3);
  display.fillScreen(ST77XX_BLACK);

  Serial.println(F("Initialized"));
  
  // Pin Mode Configuration
  pinMode(d1, INPUT_PULLDOWN); // Button D1
  pinMode(d2, INPUT_PULLDOWN); // Button D2
  pinMode(A0, OUTPUT); // Teensy Power

  // Turn Teensy on
  digitalWrite(A0, HIGH);
  showIntro();
}

// Run the intro animation
void showIntro(){
  display.setTextWrap(false);
  display.fillScreen(ST77XX_BLACK);
  testfastlines(ST77XX_GREEN);
  delay(1000);
  display.setTextColor(ST77XX_WHITE);
  display.setTextSize(2);
  display.setCursor(50, 60);
  display.println("FSO White Team");
  display.print("       Receiver");
  delay(1000);
  display.fillScreen(ST77XX_BLACK);
}

// Borrowed from Adafruit ST7735 example code
// Used to show animation for when Feather boots
void testfastlines(uint16_t color) {
    display.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < display.width(); x += 6) {
    display.drawLine(0, 0, x, display.height() - 1, color);
    delay(0);
  }
  for (int16_t y = 0; y < display.height(); y += 6) {
    display.drawLine(0, 0, display.width() - 1, y, color);
    delay(0);
  }

  display.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < display.width(); x += 6) {
    display.drawLine(display.width() - 1, 0, x, display.height() - 1, color);
    delay(0);
  }
  for (int16_t y = 0; y < display.height(); y += 6) {
    display.drawLine(display.width() - 1, 0, 0, y, color);
    delay(0);
  }

  display.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < display.width(); x += 6) {
    display.drawLine(0, display.height() - 1, x, 0, color);
    delay(0);
  }
  for (int16_t y = 0; y < display.height(); y += 6) {
    display.drawLine(0, display.height() - 1, display.width() - 1, y, color);
    delay(0);
  }

  display.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < display.width(); x += 6) {
    display.drawLine(display.width() - 1, display.height() - 1, x, 0, color);
    delay(0);
  }
  for (int16_t y = 0; y < display.height(); y += 6) {
    display.drawLine(display.width() - 1, display.height() - 1, 0, y, color);
    delay(0);
  }
}

void loop() {
  // Check to see if the power button was pressed
  powerManagement();
  // Check to see if mute button is pressed
  if (digitalRead(d1)){
    isMuted = !isMuted;
  }
  // Send mute status to Teensy so it mutes the audio
  if (isMuted){
    digitalWrite(GPIO_NUM_5, HIGH);
  } else{
    digitalWrite(GPIO_NUM_5, LOW);
  }
  // Clear screen for next print if needed
  // Volume Level Clear
  display.fillRect(104, 65, 100, 15, ST77XX_BLACK);
  // Battery Percentage Clear
  if (battery.cellPercent() != prev_percent) {display.fillRect(104, 15, 100, 15, ST77XX_BLACK);}
  prev_percent = battery.cellPercent();
  // Volume Logo Clear
  if (isMuted != prev_mute) {
    display.fillRect(104, 65, 100, 15, ST77XX_BLACK);
    display.fillRect(55, 60, 31, 30, ST77XX_BLACK);
    display.fillRect(0, 60, 31, 24, ST77XX_BLACK);
  }

  // Print information to display
  updateDisplay(ST77XX_WHITE);
  prev_mute = isMuted;
  prev_volume = volume;
  delay(100);
}

// Checks the power switch and changes the power state if it is pressed
void powerManagement(){
  // Check power switch
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
      // Show the power on intro again
      showIntro();
      isOff = 0;
    }
  }
}

// Updates the TFT display to show current information and battery assignments
void updateDisplay(int textColor){
  // Battery management
  display.setCursor(100, 0);
  display.setTextColor(textColor, ST77XX_BLACK);
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
  if (textColor == ST77XX_BLACK){display.setTextColor(ST77XX_BLACK);}
  display.setCursor(104,15);
  display.print(battery.cellPercent());
  display.println("%");

  // Button assignment labels
  display.drawBitmap(0,110, epd_bitmap_power, 25, 25, ST77XX_WHITE);
  if (!isMuted){
    display.drawBitmap(0,60,epd_bitmap_volumemuted, 31, 24, ST77XX_WHITE);
  }
  else {display.drawBitmap(0,60,epd_bitmap_volumeon, 31, 24, ST77XX_WHITE);}
  
  // separate buttons on display
  display.drawLine(35, 0, 35, 135, ST77XX_WHITE);
  
  // Volume level management
  if (isMuted){
    display.drawBitmap(55,60,epd_bitmap_volumemuted, 31, 24, ST77XX_RED);
    display.setTextColor(ST77XX_RED);
  } else{
    display.drawBitmap(55,60,epd_bitmap_volumeon, 31, 24, ST77XX_WHITE);
    display.setTextColor(ST77XX_WHITE);
  }

  // Get the volume from the Teensy
  volume = getVolume();
  display.setCursor(104,66);
  display.print(volume);

  // Receiving signal indicator
  display.drawBitmap(190, 85, epd_bitmap_Download, 50, 50, ST77XX_WHITE);
}

// Read the reported volume from the Teensy to be displayed on TFT
double getVolume(){
  int vol = 0;
  double retVol;
  // if (TeensyComms.available() > 0)
  // {
  //   vol = TeensyComms.parseInt();
  //   compSerial.print(vol);
  // }

  vol = analogRead(GPIO_NUM_8);
  retVol = vol/1023;
  //compSerial.println(vol);

  return retVol*10;
}