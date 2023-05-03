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
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>
#include <TeensyTimerTool.h>

// Defining NeoSlider Values
#define  DEFAULT_I2C_ADDR 0x30 // Address of NeoSlider
#define  ANALOGIN   18
#define  NEOPIXELOUT 14

// Global Vars
TeensyTimerTool::PeriodicTimer t1(TeensyTimerTool::GPT2);

// Declaring Constructor for NeoSlider
Adafruit_seesaw seesaw;
seesaw_NeoPixel pixels = seesaw_NeoPixel(4, NEOPIXELOUT, NEO_GRB + NEO_KHZ800);

// Teensy Audio Library
// GUItool: begin automatically generated code
AudioInputI2S            i2s1;
AudioAmplifier           amp1;           //xy=540,337
AudioAmplifier           amp2;           //xy=572,378
AudioOutputI2S           pt8211_1;       //xy=769,343
AudioFilterBiquad        biquad1;
AudioConnection          patchCord5(i2s1,0,biquad1,0);
AudioConnection          patchCord6(biquad1,0,amp1,0);
AudioConnection          patchCord7(biquad1,0,amp2,0);
AudioConnection          patchCord3(amp1, 0, pt8211_1, 0);
AudioConnection          patchCord4(amp2, 0, pt8211_1, 1);
AudioControlSGTL5000     sgtl5000_1;
// GUItool: end automatically generated code

// Function Declarations
uint32_t Wheel(byte WheelPos);
bool CheckVolume(double volume);
void receiveSignal();
void sendVolume(double voltage);

void setup() {
  AudioMemory(12);
  // Setting default gains
  amp1.gain(1);
  amp2.gain(-1);
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.volume(0.7);

  // create surround sound from mono
  sgtl5000_1.audioPostProcessorEnable();
  sgtl5000_1.surroundSoundEnable();

  // Initializing Serial connection for debugging
  Serial.begin(9600);
  // Set up the pins
  pinMode(PIN_A13, INPUT); // Signal Read pin
  pinMode(33, INPUT_PULLDOWN); // Mute switch reading pin
  pinMode(PIN_A16, OUTPUT); // Volume sending pin
  pinMode(38, INPUT); // SPDIF IN Digital Read Pin
  pinMode(39, INPUT); // Laser Test Reading Pin
  delayMicroseconds(10);
  
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

  analogWriteResolution(16);
  Serial.println(F("seesaw started OK!"));

  pixels.setBrightness(255);  // half bright
  pixels.show(); // Initialize all pixels to 'off'
  // End NeoSlider Initialization

  // Digital Read from Laser
  // t1.begin(receiveSignal, 325ns);

  // Audio Filtering
  biquad1.setLowpass(0, 10000, 0.707);
  //biquad1.setBandpass(0, 18000, 0.707);
  //biquad1.setHighpass(0, 2700, 0.707);
  //biquad1.setNotch(1, 10000, 100);

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

void sendVolume(double volume){
  analogWrite(PIN_A16, volume);
  //Serial.print("Sending volume: ");
  //Serial.println(volume);
}

void loop() {
  // read the NeoSlider potentiometer
  int slide_val = seesaw.analogRead(ANALOGIN);
  sendVolume(slide_val / 1023.0);
  // Serial.println(slide_val);

  // Set the color of the LEDs 
  for (uint8_t i=0; i< pixels.numPixels(); i++) {
    pixels.setPixelColor(i, Wheel(slide_val / 4));
  }
  pixels.show();
  // Check the volume potentiometer for volume level
  CheckVolume(slide_val);
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
    return true;
  }
  else{
    // Mute switch is off, set gain to current amp modifier
    //Serial.print("Actual Gain Adj: ");
    //Serial.println(actualGain);
    amp1.gain(actualGain);
    amp2.gain(-actualGain);
    return false;
  }
}