#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_NeoPixel.h>
#include <Adafruit_TestBed.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_MAX1704X.h>
#include <SPI.h>
#include <./symbols.h>

// Definitions
#define d0 GPIO_NUM_0
#define d1 GPIO_NUM_1

// Declare Interfaces with Battery and Display
Adafruit_MAX17048 battery;
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Global Vars
int isMuted = 0;
// Function declarations
bool buttonPressed;
void buttonInterrupt();
void updateDisplay(int textColor);

void IRAM_ATTR wakeup(void* arg){}

void setup() {
  Serial.begin(9600);

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

  Serial.println(F("Initialized"));
  
  pinMode(d1, INPUT);

  display.setTextWrap(false);
  display.fillScreen(ST77XX_BLACK);
  display.setCursor(0, 30);
  display.setTextColor(ST77XX_RED);
  display.setTextSize(2);
  display.print("FSO");
  delay(1000);
  display.fillScreen(ST77XX_BLACK);

  // Sleep mode interrupt setup
  gpio_pad_select_gpio(d0);
  gpio_set_direction(d0, GPIO_MODE_INPUT);
  gpio_set_intr_type(d0, GPIO_INTR_POSEDGE);
  gpio_install_isr_service(0);
  gpio_isr_handler_add(d0, wakeup, NULL);
  esp_sleep_enable_ext0_wakeup(d0, 1);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!digitalRead(d0)){
    esp_deep_sleep_start();
  }
  if (digitalRead(d1)){
    isMuted = !isMuted;
  }
  updateDisplay(ST77XX_WHITE);
  delay(500);
  display.fillScreen(ST77XX_BLACK);
}

void updateDisplay(int textColor){
  display.setCursor(100, 0);
  display.setTextColor(textColor, ST77XX_BLACK);
  if (battery.cellPercent() <= 20){
    display.setTextColor(ST77XX_RED);
    display.drawBitmap(40, 0, epd_bitmap_battery, 64, 40, ST77XX_WHITE);
  } else if (battery.cellPercent() >= 50){
    display.setTextColor(ST77XX_GREEN);
    display.drawBitmap(40, 0, epd_bitmap_battery, 64, 40, ST77XX_GREEN);
  } else if (battery.cellPercent() < 50 && battery.cellPercent() > 20){
    display.setTextColor(ST77XX_YELLOW);
    display.drawBitmap(40, 0, epd_bitmap_battery, 64, 40, ST77XX_WHITE);
  }
  if (textColor == ST77XX_BLACK){display.setTextColor(ST77XX_BLACK);}
  display.setCursor(104,15);
  display.print(battery.cellPercent());
  display.println("%");

  // Button labels
  display.drawBitmap(0,0, epd_bitmap_power, 25, 25, ST77XX_WHITE);
  display.drawBitmap(0,60,epd_bitmap_volumemuted, 31, 24, ST77XX_WHITE);
  display.drawLine(35, 0, 35, 135, ST77XX_WHITE);
  // Volume level management
  if (isMuted){
    display.drawBitmap(55,60,epd_bitmap_volumemuted, 31, 24, ST77XX_RED);
    display.setTextColor(ST77XX_RED);
    analogWrite(A0, 3.3);
  } else{
    display.drawBitmap(55,60,epd_bitmap_volumeon, 31, 24, ST77XX_WHITE);
    display.setTextColor(ST77XX_WHITE);
    analogWrite(A0, 0);
  }
}
