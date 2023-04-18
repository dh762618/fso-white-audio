/*
 * Transmission Teensy
 * 
 * Takes input from the 3.5mm and XLR audio jacks and passes the 
 * signal though a pre-amplification gain and low pass filter. The 
 * signal is then transmitted via laser diode. The signal strength is 
 * displayed via Reverse Feather Display.
 * 
 */

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>
#include <TeensyTimerTool.h>

#define  DEFAULT_I2C_ADDR 0x30
#define  ANALOGIN   18
#define  NEOPIXELOUT 14 

// Declaring the constructor for the NeoSlider gain
Adafruit_seesaw seesaw;
seesaw_NeoPixel pixels = seesaw_NeoPixel(4, NEOPIXELOUT, NEO_GRB + NEO_KHZ800);

// Declaration for test code 1's and 0's
IntervalTimer mytimer;
int laserState = LOW;
TeensyTimerTool::PeriodicTimer t1(TeensyTimerTool::GPT2);

// Global Variables
int counter = 0; // Counter for test code

// From Teensy Audio Library
// GUItool: begin automatically generated code4
AudioFilterBiquad        biquad1;
AudioInputI2S            i2s1;           //xy=200,69
AudioOutputSPDIF3        spout1;
AudioAmplifier           amp1;
AudioOutputI2S           i2sOut;
AudioOutputPWM           pwm1(36,37);
AudioConnection          patchCord1(i2s1, 0, amp1, 0);
AudioConnection          patchCord2(i2s1, 1, amp1, 1);
//AudioConnection        patchCord3(amp1, 0, biquad1, 0);
AudioConnection          patchCord7(amp1, 0, spout1, 0);
AudioConnection          patchCord8(amp1, 0, i2sOut, 0);
AudioConnection          patchCord9(amp1, 0, pwm1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=302,184

// GUItool: end automatically generated code

// possibly touch buttons for sound effects

// Declare that the input will constantly be read from the line in ports
const int myInput = AUDIO_INPUT_LINEIN;


// Declare neoslider function (taken from example)
uint32_t Wheel(byte WheelPos);

// Declare gain regulation function
void GainRegulation(double gainValue);
void callback();
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(20);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.7);
  // test code pin mode
  pinMode(39, OUTPUT);

  // pin modes from PAD's
  pinMode(26, INPUT); // 3.5 pad
  pinMode(27, INPUT); // XLR pad

  // pin modes to LED's
  pinMode(28, OUTPUT); // 3.5 LED
  pinMode(29, OUTPUT); // XLR LED

  // pin modes to PAD status checking
  pinMode(30, OUTPUT); // 3.5 LED
  pinMode(31, OUTPUT); // XLR LED

  // Default Pin Values
  digitalWrite(28, LOW);
  digitalWrite(29, LOW);
  digitalWrite(30, HIGH);
  digitalWrite(31, HIGH);
  
  
  // set default gain
  int gain = 1;
  amp1.gain(gain);
  
  // Implement filter
  // biquad1.setLowpass(0, 10000, 0.707);

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
  pixels.setBrightness(255);  // half bright
  pixels.show(); // Initialize all pixels to 'off'

  // Test code that pulses 1s and 0s repeatedly at ~50 kHz
  //t1.begin(callback, 156.25ns);
  pinMode(39, OUTPUT);
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// This code is borrowed from the Adafruit neoslider example
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
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// used for test code
void callback(){
  switch(counter){
    case 0:
      laserState = HIGH;
      break;
    case 2:
      laserState = LOW;
      break;
    case 3:
      laserState = HIGH;
      break;
    case 5:
      laserState = LOW;
      break;
    case 6:
      laserState = HIGH;
      break;
    case 8:
      laserState = LOW;
      break;
    case 9:
      laserState = HIGH;
      break;
    case 11:
      laserState = LOW;
      break;
    case 12:
      laserState = HIGH;
      break;
    case 16:
      laserState = LOW;
      break;
    case 17:
      laserState = HIGH;
      break;
    case 18:
      laserState = LOW;  
      counter = 0;  
  }
  counter++;
  digitalWriteFast(39, laserState);
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void loop() {
  // read the potentiometer
  double slide_val = seesaw.analogRead(ANALOGIN);
  Serial.println(slide_val);

  for (uint8_t i=0; i< pixels.numPixels(); i++) {
    pixels.setPixelColor(i, Wheel(slide_val / 4));
  }
  pixels.show();

  //Update the gain
  GainRegulation(slide_val);

  // Read the voltage on the 'L' PAD from the 3.5mm headphone port to determine if it is on
  // Allows for LED indicator 
  int headphonePad = digitalRead(26);
  Serial.print("Headphone Pad: ");
  Serial.println(headphonePad);
  // Check 3.5mm PAD voltage and change LED status
  if (headphonePad){
    digitalWrite(28, HIGH);
  } else{
    digitalWrite(28, LOW);
  }

  // Read the voltage on the 'L' PAD from the XLR port to determine if it is on
  // Allows for LED indicator 
  int xlrPad = digitalRead(27);
  Serial.print("XLR Pad: ");
  Serial.println(xlrPad);
  // Check XLR PAD voltage and change LED status
  if (xlrPad){
    digitalWrite(29, HIGH);
  } else{
    digitalWrite(29, LOW);
  }

  // serial prints for debugging
  Serial.print("Voltage on XLR PAD: ");
  Serial.println(xlrPad);

  Serial.print("Voltage on 3.5mm PAD: ");
  Serial.println(headphonePad);

  delay(50);
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void GainRegulation(double gainValue)
{
  double actualGain = gainValue + 1;
  amp1.gain(actualGain);
  Serial.print("Actual Gain: ");
  Serial.println(actualGain);
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////