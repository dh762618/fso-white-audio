/*
 * Transmission Teensy
 * 
 * Takes input from the 3.5mm and XLR audio jacks and passes the 
 * signal though a pre-amplification gain and low pass filter. The 
 * signal is then transmitted via laser diode. The signal strength is 
 * displayed via an LCD Display.
 * 
 */

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <../include/LiquidCrystal_I2C.h>
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>
#include <TeensyTimerTool.h>

#define  DEFAULT_I2C_ADDR 0x30
#define  ANALOGIN   18
#define  NEOPIXELOUT 14

// Declaring the constructor for the NeoSlider gain
Adafruit_seesaw seesaw;
seesaw_NeoPixel pixels = seesaw_NeoPixel(4, NEOPIXELOUT, NEO_GRB + NEO_KHZ800);
IntervalTimer mytimer;
int laserState = LOW;
TeensyTimerTool::PeriodicTimer t1(TeensyTimerTool::GPT2);

// Set up LCD
LiquidCrystal_I2C lcd(0x27, 16 ,2);

// From Teensy Audio Library
// GUItool: begin automatically generated code4
AudioFilterBiquad        biquad1;
AudioInputI2S            i2s1;           //xy=200,69
AudioOutputSPDIF3        spout1;
AudioOutputI2S           i2s2;           //xy=365,94
AudioAmplifier           amp1;
AudioConnection          patchCord1(i2s1, 0, amp1, 0);
AudioConnection          patchCord2(i2s1, 1, amp1, 1);
AudioConnection          patchCord3(amp1, 0, biquad1, 0);
AudioConnection          patchCord4(amp1, 1, biquad1, 1);
AudioConnection          patchCord5(biquad1, 0, i2s2, 0);
AudioConnection          patchCord6(biquad1, 1, i2s2, 1);
AudioConnection          patchCord7(biquad1, 0, spout1, 0);
AudioConnection          patchCord8(biquad1, 1, spout1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=302,184
// GUItool: end automatically generated code

// LCD Character Declarations
// output signal level
byte output[] = {
  B00000,
  B00100,
  B00110,
  B11111,
  B00110,
  B00100,
  B00000,
  B00000
};
// possibly touch buttons for sound effects

// Declare that the input will constantly be read from the line in ports
const int myInput = AUDIO_INPUT_LINEIN;

// Declare LCDOutput
void LCDOutput(double display_gain);

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
  
  // set default gain
  // int gain = 75;
  // amp1.gain(gain);
  
  // Implement filter
  biquad1.setLowpass(0, 10000, 0.707);

  // Implement characters to be displayed on LCD Screen
  lcd.init();
  lcd.createChar(0, output);

  //Create LCD Splash Screen
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Transmit Teensy");
  lcd.setCursor(0,1);
  lcd.print("FSO White");
  delay(2000);
  lcd.clear();

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

  t1.begin(callback, 500ns);
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

void blinkLaser(){
  if (laserState == LOW)
  {
    laserState = HIGH;
  } else{
    laserState = LOW;
  }
  digitalWrite(39, laserState);
}
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

  // Update the LCD Screen
  LCDOutput((slide_val / 512));

  delay(50);
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void LCDOutput(double display_gain)
{
  lcd.setCursor(15,0);
  lcd.write(0);
  lcd.setCursor(7, 1);
  lcd.print("+");
  lcd.print(display_gain);
  lcd.print(" dB");
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void GainRegulation(double gainValue)
{
  double actualGain = gainValue / 100;
  if (actualGain == 0)
    actualGain = 2;
  amp1.gain(actualGain);
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////