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
#define  

// Declaring the constructor for the NeoSlider gain
Adafruit_seesaw seesaw;
seesaw_NeoPixel pixels = seesaw_NeoPixel(4, NEOPIXELOUT, NEO_GRB + NEO_KHZ800);

// Declaration for test code 1's and 0's
IntervalTimer mytimer;
int laserState = LOW;
TeensyTimerTool::PeriodicTimer t1(TeensyTimerTool::GPT2);

// From Teensy Audio Library
// GUItool: begin automatically generated code4
AudioFilterBiquad        biquad1;
AudioInputI2S            i2s1;           //xy=200,69
AudioOutputSPDIF3        spout1;
AudioOutputI2S           i2s2;           //xy=365,94
AudioAmplifier           amp1;
AudioConnection          patchCord1(i2s1, 0, amp1, 0);
AudioConnection          patchCord3(amp1, 0, biquad1, 0);
AudioConnection          patchCord5(biquad1, 0, i2s2, 0);
AudioConnection          patchCord7(biquad1, 0, spout1, 0);
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
  AudioMemory(12);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.7);

  pinMode(39, OUTPUT);
  pinMode(38, INPUT_PULLUP);
  pinMode(37, OUTPUT);

  // Default Pin Values
  digitalWrite(37, LOW);
  
  // set default gain
  // int gain = 75;
  // amp1.gain(gain);
  
  // Implement filter
  biquad1.setLowpass(0, 10000, 0.707);

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

  // Test code that pulses 1s and 0s repeatedly at ~50 kHz
  //t1.begin(callback, 1us);
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
elapsedMillis volmsec=0;
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void callback(){
  if (laserState == LOW)
  {
    laserState = HIGH;
  } else{
    laserState = LOW;
  }
  digitalWriteFast(39, laserState);
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void loop() {
  // Every 50 ms, adjust the volume
  if (volmsec > 50) {
    float vol = analogRead(15);
    vol = vol / 1023.0;
    //audioShield.volume(vol); // <-- uncomment if you have the optional
    volmsec = 0;               //     volume pot on your audio shield
  }

  // read the potentiometer
  //loopCounter++;
  double slide_val = seesaw.analogRead(ANALOGIN);
  Serial.println(slide_val);

  for (uint8_t i=0; i< pixels.numPixels(); i++) {
    pixels.setPixelColor(i, Wheel(slide_val / 4));
  }
  pixels.show();

  //Update the gain
  GainRegulation(slide_val);

  // Output will be displayed on reverse TFT Feather
  // Code needs added here to interface with the feather instead of doing LCD stuff
  // // Update the LCD Screen
  // LCDOutput(slide_val);

  // Read the voltage on the 'L' PAD to determine if it is on
  // Allows for LED indicator 
  int LReading = analogRead(38);
  if (LReading > 400){
    digitalWrite(37, HIGH);
  } else{
    digitalWrite(37, LOW);
  }

  Serial.print("Voltage on L PAD: ");
  Serial.println(LReading);

  delay(50);
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// void LCDOutput(double display_gain)
// {
//   display_gain += 1.8;
//   lcd.setCursor(15,0);
//   lcd.write(0);
//   lcd.setCursor(7, 1);
//   lcd.print("+");
//   lcd.print(20*log10(display_gain), 2);
  
//   if (20*log10(display_gain) < 10){
//     lcd.setCursor(13, 1);
//     lcd.print(" ");
//   }
//   lcd.setCursor(13, 1);
//   lcd.print(" dB");
// }
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void GainRegulation(double gainValue)
{
  double actualGain = gainValue + 1.8;
  amp1.gain(actualGain);
  Serial.print("Actual Gain: ");
  Serial.println(actualGain);
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////