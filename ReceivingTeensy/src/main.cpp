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

// Global Variables
double vol = 1.0;

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

void OutputLCD(double voltage, double vol);
double CheckVolume();

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
}

void loop() {

  // Update LCD
  analogReadResolution(12);
  double reading = 0;
  reading = analogRead(A13);
  double voltage = reading / 1023.0;
  // Check the volume potentiometer for volume level
  double volume = CheckVolume();
  // Doing to LCD Update
  OutputLCD(voltage, volume);

  delay(250);
}

double CheckVolume(){
  // Check Volume
  analogReadResolution(12);
  double volRead = analogRead(PIN_A16);
  vol = (volRead / 1023.0) * 2.3;
  double volume = floor(vol);
  Serial.print("Volume Reading: ");
  Serial.println(vol);
  lcd.setCursor(0,0);
  int val = digitalRead(33);
  Serial.print("Mute: ");
  Serial.println(val);
  if (val == HIGH || volume == 0){
    amp1.gain(0);
    amp2.gain(0);
    volume = 0;
  }
  else{
    double actualGain = vol / 5;
    Serial.print("Actual Gain Adj: ");
    Serial.println(actualGain);
    amp1.gain(vol);
    amp2.gain(vol);
  }
  return volume;
}

void OutputLCD(double voltage, double volume){
  lcd.setCursor(15, 0);
  double inputFreq = spdifIn.getInputFrequency();
  if (inputFreq > 43000){
    lcd.write(7);
  } else{
    lcd.print(" ");
  }
  lcd.setCursor(0, 1);
  lcd.write(6);
  if (volume == 0){
    lcd.write(0);
  } else{
    lcd.write(1);
  }
  lcd.print(int(floor(vol)));
  lcd.setCursor(10, 1);
  lcd.print(voltage);
  lcd.print(" V");
}