/*
 * Receiving Teensy
 * 
 * A simple hardware test which receives the signal via photodetector.
 * Signal strength from detector is displayed on LCD.
 * Signal is output through the DAC.
 *
 * This example code is in the public domain.
 */

#include <Audio.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <../include/LiquidCrystal_I2C.h>
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>

#define  DEFAULT_I2C_ADDR 0x30
#define  ANALOGIN   18
#define  NEOPIXELOUT 14

// Global Vars
double voltage = 0;
int loopCounter = 0; //Counts how many times the loop is entered - used to slow down the voltage reading
// Declaring Constructor for NeoSlider
Adafruit_seesaw seesaw;
seesaw_NeoPixel pixels = seesaw_NeoPixel(4, NEOPIXELOUT, NEO_GRB + NEO_KHZ800);

// Set up LCD
LiquidCrystal_I2C lcd(0x27, 16 ,2);

// Teensy Audio Library
// GUItool: begin automatically generated code
AsyncAudioInputSPDIF3    spdifIn(false, false, 100, 20, 80);   //xy=307,222
AudioAmplifier           amp2;           //xy=473,247
AudioAmplifier           amp1;           //xy=477,211
AudioOutputPT8211        i2s1;           //xy=740,223
AudioConnection          patchCord1(spdifIn, 0, amp1, 0);
AudioConnection          patchCord2(spdifIn, 1, amp2, 0);
AudioConnection          patchCord3(amp2, 0, i2s1, 1);
AudioConnection          patchCord4(amp1, 0, i2s1, 0);   //xy=106,226
// GUItool: end automatically generated code


// LCD Character Declarations
byte muted[] = {
  B00000,
  B10001,
  B01010,
  B00100,
  B01010,
  B10001,
  B00000,
  B00000
};

byte unmuted[] = {
  B00010,
  B01001,
  B00101,
  B00101,
  B00101,
  B01001,
  B00010,
  B00000
};

byte volume[] = {
  B00001,
  B00011,
  B01111,
  B01111,
  B01111,
  B00011,
  B00001,
  B00000

};

byte receiving[] = {
  B00100,
  B00100,
  B10101,
  B01110,
  B00100,
  B11111,
  B11111,
  B11111
};
// End LCD Custom Characters

void OutputLCD(double voltage, double vol, bool muted);
uint32_t Wheel(byte WheelPos);
bool CheckVolume(double volume);

void setup() {
  AudioMemory(12);
  // Setting default gains
  amp1.gain(1);
  amp2.gain(1);
  // Initializing Serial connection for debugging
  Serial.begin(57600);
  pinMode(PIN_A13, INPUT); // Signal Read pin
  pinMode(33, INPUT_PULLDOWN); // Mute switch reading pin
  pinMode(PIN_A16, INPUT); // Volume reading pin
  delayMicroseconds(10);
  // Make custom characters
  lcd.init();
  lcd.createChar(0, muted);
  lcd.createChar(1, unmuted);
  lcd.createChar(6, volume);
  lcd.createChar(7, receiving);

  // Splash Screen
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Receiving Teensy");
  lcd.setCursor(0,1);
  lcd.print("FSO White");
  delay(1000);
  lcd.clear();

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
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
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

void loop() {
  // read the potentiometer
  loopCounter++;
  double slide_val = seesaw.analogRead(ANALOGIN);
  Serial.println(slide_val);

  for (uint8_t i=0; i< pixels.numPixels(); i++) {
    pixels.setPixelColor(i, Wheel(slide_val / 4));
  }
  pixels.show();
  // Update LCD
  if (loopCounter == 5){
    analogReadResolution(12);
    double reading = 0;
    reading = analogRead(A13);
    voltage = reading / 1023.0;
    loopCounter = 0;
  }
  // Check the volume potentiometer for volume level
  bool muted = CheckVolume(slide_val);
  // Doing to LCD Update
  OutputLCD(voltage, slide_val / 512, muted);

  delay(50);
}

void OutputLCD(double voltage, double volume, bool muted){
  lcd.setCursor(15, 0);
  double inputFreq = spdifIn.getInputFrequency();
  if (inputFreq > 43000){
    lcd.write(7);
  } else{
    lcd.print(" ");
  }
  lcd.setCursor(0, 1);
  lcd.write(6);
  if (muted){
    lcd.write(0);
  } else{
    lcd.write(1);
  }
  lcd.print(volume);
  lcd.setCursor(10, 1);
  lcd.print(voltage);
  lcd.print(" V");
}


bool CheckVolume(double volume){
  // Check Volume
  analogReadResolution(12);
  int val = digitalRead(33);
  Serial.print("Mute: ");
  Serial.println(val);
  double actualGain = volume / 512;
  if (val == HIGH || volume == 0){
    amp1.gain(0);
    amp2.gain(0);
    lcd.setCursor(0,0);
    // // :P
    if (volume >= 352 && volume <= 354){lcd.print("Poggers");}
    else{lcd.print("       ");}
    return true;
  }
  else{
    lcd.setCursor(0,0);
    lcd.print("       ");
    Serial.print("Actual Gain Adj: ");
    Serial.println(actualGain);
    amp1.gain(actualGain);
    amp2.gain(actualGain);
    return false;
  }
}