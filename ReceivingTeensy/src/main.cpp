#include <Audio.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <../include/LiquidCrystal_I2C.h>

// Set up LCD
LiquidCrystal_I2C lcd(0x27, 16 ,2);

// GUItool: begin automatically generated code
AsyncAudioInputSPDIF3    spdifIn(false, false, 100, 20, 80);   //xy=307,222
AudioAmplifier           amp2;           //xy=473,247
AudioAmplifier           amp1;           //xy=477,211
AudioOutputPT8211        i2s1;           //xy=740,223
AudioConnection          patchCord1(spdifIn, 0, amp1, 0);
AudioConnection          patchCord2(spdifIn, 1, amp2, 0);
AudioConnection          patchCord3(amp2, 0, i2s1, 1);
AudioConnection          patchCord4(amp1, 0, i2s1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=106,226
// GUItool: end automatically generated code


void setup() {
  AudioMemory(12);
  amp1.gain(1);
  amp2.gain(1);
  Serial.begin(57600);
  lcd.init();
  lcd.setCursor(0,0);
  lcd.backlight();
  lcd.print("Hello World!");
  delay(500);
  lcd.blink();
  while (!Serial);

}

void loop() {
  double bufferedTime=spdifIn.getBufferedTime();
  Serial.print("buffered time [micro seconds]: ");
  Serial.println(bufferedTime*1e6,2);
  
  
  Serial.print("locked: ");
  Serial.println(spdifIn.isLocked());
  Serial.print("input frequency: ");
  double inputFrequency=spdifIn.getInputFrequency();
  Serial.println(inputFrequency);
  Serial.print("anti-aliasing attenuation: ");
  Serial.println(spdifIn.getAttenuation());
  
  Serial.print("resampling goup delay [milli seconds]: ");
  Serial.println(spdifIn.getHalfFilterLength()/inputFrequency*1e3,2);
  
  Serial.print("half filter length: ");
  Serial.println(spdifIn.getHalfFilterLength()); 
  
  double pUsageIn=spdifIn.processorUsage(); 
  Serial.print("processor usage [%]: ");
  Serial.println(pUsageIn);

  Serial.print("max number of used blocks: ");
  Serial.println(AudioMemoryUsageMax()); 

  delay(500);
}