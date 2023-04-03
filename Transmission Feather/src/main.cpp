#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_TestBed.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_MAX1704X.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMono9pt7b.h>
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>

#define  ANALOGIN 18
#define  NEOPIXELOUT 14
#define  DEFAULT_I2C_ADDR 0x30

// Declare Interfaces with Battery and Display
Adafruit_MAX17048 battery;
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Declaring the constructor for the NeoSlider gain
Adafruit_seesaw seesaw;
seesaw_NeoPixel pixels = seesaw_NeoPixel(4, NEOPIXELOUT, NEO_GRB + NEO_KHZ800);
// Declare neoslider function (taken from example)
uint32_t Wheel(byte WheelPos);

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
  
  // Initialize the seesaw neopixel slider
  if (!seesaw.begin(DEFAULT_I2C_ADDR)) {
    Serial.println(F("seesaw not found!"));
    while(1) delay(10);
  }
  uint16_t pid;
  uint8_t year, mon, day;
  seesaw.getProdDatecode(&pid, &year, &mon, &day);
  Serial.print("seesaw found PID: ");
  Serial.print(pid);
  Serial.print(" datecode: ");
  Serial.print(2000+year); Serial.print("/");
  Serial.print(mon); Serial.print("/");
  Serial.println(day);
  if (pid != 5295) {
    Serial.println(F("Wrong seesaw PID"));
    while (1) delay(10);
  }
  if (!pixels.begin(DEFAULT_I2C_ADDR)){
    Serial.println("seesaw pixels not found!");
    while(1) delay(10);
  }
  Serial.println(F("seesaw started OK!"));
  pixels.setBrightness(255);  // half bright
  pixels.show(); // Initialize all pixels to 'off'

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

  display.setCursor(0,0);
  display.setTextColor(ST77XX_WHITE);
  display.setTextWrap(false);
  display.print("FSO White Team\n");
  display.print("Transmission Feather");
  delay(4000);
  display.fillScreen(ST77XX_BLACK);
}
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void loop() {
  // put your main code here, to run repeatedly:
  display.setCursor(0,0);
  display.setTextColor(ST77XX_WHITE);
  display.setTextWrap(true);
  display.print("Battery Level: ");
  display.println(battery.cellPercent());
  display.print("Battery Voltage: ");
  display.println(battery.cellVoltage());
  delay(2000);

  // read the adafruit neoslider
  double slide_val = seesaw.analogRead(ANALOGIN);
  Serial.println(slide_val);

  for (uint8_t i=0; i< pixels.numPixels(); i++) {
    pixels.setPixelColor(i, Wheel(slide_val / 4));
  }
  pixels.show();
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