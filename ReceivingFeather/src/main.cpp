#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_TestBed.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_MAX1704X.h>

// Declare Interfaces with Battery and Display
Adafruit_MAX17048 battery;
Adafruit_ST7789 display = Adafruit_ST7789(10, 8, 9);

void setup() {
  Serial.begin(115200);

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

  // initialize TFT
  display.init(135, 240); // Init ST7789 240x135
  display.setRotation(3);
  display.fillScreen(ST77XX_BLACK);
}

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
}