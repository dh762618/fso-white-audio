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
#include <iomanip>
#include <../include/LiquidCrystal_I2C.h>
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>
#include <TeensyTimerTool.h>

// Defining NeoSlider Values
#define  DEFAULT_I2C_ADDR 0x30 // Address of NeoSlider
#define  ANALOGIN   18
#define  NEOPIXELOUT 14

// Global Vars
double voltage = 0;
long long int loopCounter = 0; //Counts how many times the loop is entered - used to slow down the voltage reading
TeensyTimerTool::PeriodicTimer t1(TeensyTimerTool::GPT2);
// Declaring Constructor for NeoSlider
Adafruit_seesaw seesaw;
seesaw_NeoPixel pixels = seesaw_NeoPixel(4, NEOPIXELOUT, NEO_GRB + NEO_KHZ800);

// Set up LCD
LiquidCrystal_I2C lcd(0x27, 16 ,2);

// Teensy Audio Library
// GUItool: begin automatically generated code
AsyncAudioInputSPDIF3    spdifIn;   //xy=335,357
AudioAmplifier           amp1;           //xy=540,337
AudioAmplifier           amp2;           //xy=572,378
AudioOutputPT8211        pt8211_1;       //xy=769,343
AudioConnection          patchCord1(spdifIn, 0, amp1, 0);
AudioConnection          patchCord2(spdifIn, 0, amp2, 0);
AudioConnection          patchCord3(amp1, 0, pt8211_1, 0);
AudioConnection          patchCord4(amp2, 0, pt8211_1, 1);
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
void receiveSignal();

void setup() {
  AudioMemory(12);
  // Setting default gains
  amp1.gain(1);
  amp2.gain(-1);
  // Initializing Serial connection for debugging
  Serial.begin(250000);
  pinMode(PIN_A13, INPUT); // Signal Read pin
  pinMode(33, INPUT_PULLDOWN); // Mute switch reading pin
  pinMode(PIN_A16, INPUT); // Volume reading pin
  pinMode(38, INPUT); // SPDIF IN Digital Read Pin
  pinMode(39, INPUT_PULLDOWN); // Laser Test Reading Pin
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
  
  // NeoSlider Initialization
  // This code borrowed from AdaFruit NeoSlider example code
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
  // End NeoSlider Initialization

  // Digital Read from Laser
  t1.begin(receiveSignal, 20us);
}

void receiveSignal(){
  int analogVal = digitalReadFast(39);
  Serial.print(analogVal); 
}
/**
 * This code is borrowed from a AdaFruit NeoSlider Example
 * Sets the color of the NeoSlider LEDs based on the given value
 * Src: https://github.com/adafruit/Adafruit_Seesaw 
*/
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
  // read the NeoSlider potentiometer
  loopCounter++;
  double slide_val = seesaw.analogRead(ANALOGIN);
  // Serial.println(slide_val);

  // Set the color of the LEDs 
  for (uint8_t i=0; i< pixels.numPixels(); i++) {
    pixels.setPixelColor(i, Wheel(slide_val / 4));
  }
  pixels.show();
  // Update LCD every 5 loop iterations
  if (loopCounter % 5 == 0){
    analogReadResolution(12);
    double reading = 0;
    reading = analogRead(A14);
    voltage = reading / 1023.0;
    loopCounter = 0;
  }
  // Check the volume potentiometer for volume level
  bool muted = CheckVolume(slide_val);
  // Doing to LCD Update
  OutputLCD(voltage, slide_val, muted);
  
}

void OutputLCD(double voltage, double volume, bool muted){
  // Print receiving status on last character of first line
  lcd.setCursor(15, 0);
  double inputFreq = spdifIn.getInputFrequency();
  double actualVolume = volume / 320;
  if (inputFreq > 43000){
    lcd.write(7);
  } else{
    lcd.print(" ");
  }
  // Write Volume mute status and level
  lcd.setCursor(0, 1);
  lcd.write(6);
  if (muted){
    lcd.write(0);
  } else{
    lcd.write(1);
  }
  if (20*log10(actualVolume) <= 1){
    lcd.print(actualVolume, 2);
  }
  else{
    lcd.print(20*log10(actualVolume), 2);
    if (20*log10(actualVolume) < 10){
      lcd.setCursor(6, 1);
      lcd.print(" ");
    }
  }
  lcd.setCursor(10, 1);
  lcd.print(voltage);
  lcd.print(" V");
}


bool CheckVolume(double volume){
  // Check Volume
  int val = digitalRead(33);
  // Serial.print("Mute: ");
  // Serial.println(val);
  // Convert Analog Reading to usable Gain modifier
  // Goes from 0 to 2 on the potentiometer
  double actualGain = volume / 320;
  // Check status of Mute Switch
  if (val == HIGH || volume == 0){
    // Mute switch is on, mute output
    amp1.gain(0);
    amp2.gain(0);
    // Easter egg :P
    lcd.setCursor(0,0);
    if (volume >= 352 && volume <= 354){lcd.print("Poggers");}
    else{lcd.print("       ");}
    return true;
  }
  else{
    // Mute switch is off, set gain to current amp modifier
    lcd.setCursor(0,0);
    lcd.print("       ");
    // Serial.print("Actual Gain Adj: ");
    // Serial.println(actualGain);
    amp1.gain(actualGain);
    amp2.gain(-actualGain);
    return false;
  }
}