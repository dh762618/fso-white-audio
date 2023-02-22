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
AudioConnection          patchCord4(amp1, 0, i2s1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=106,226
// GUItool: end automatically generated code


// LCD Character Declarations
byte zero[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte one[] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};
byte two[] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
};

byte three[] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
};

byte four[] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};

byte five[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
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

void OutputLCD(double voltage, double vol);

void setup() {
  AudioMemory(12);
  amp1.gain(1);
  amp2.gain(1);
  Serial.begin(57600);
  pinMode(PIN_A12, INPUT);
  pinMode(34, INPUT_PULLDOWN);
  pinMode(33, INPUT_PULLDOWN);
  delayMicroseconds(10);
  // Make custom characters
  lcd.init();
  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);
  lcd.createChar(6, volume);
  lcd.createChar(7, receiving);

  // Splash Screen
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Voltage Test");
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
  // Check Volume
  analogReadResolution(12);
  double volRead = 0;
  volRead = analogRead(A12);
  double volume = (volRead / 1023.0) * 10;
  lcd.setCursor(0,0);
  int val = digitalRead(33);
  int val2 = digitalRead(34);
  Serial.print("Val1: ");
  Serial.println(val);
  Serial.print("Val2: ");
  Serial.println(val2);
  if (val == HIGH){
    amp1.gain(0);
    volume = 0;
  }
  else if (val2 == HIGH){
    amp1.gain(1);
    volume = 10;
  }
  else{
    amp1.gain(1);
    volume = 10;
  }
  OutputLCD(voltage, volume);

  delay(250);
}

void OutputLCD(double voltage, double volume){
  // double count = 1.4;
  // double factor = voltage/count;
  // int percent = (voltage+1)/factor;
  // int number = percent / 5;
  // int remainder = percent % 5;
  // if (number > 0){
  //     for(int j = 0; j < number; j++)
  //     {
  //       lcd.setCursor(j, 0);
  //       lcd.write(5);
  //     }
  // }
  // lcd.setCursor(number,0);
  // lcd.write(remainder); 
  // if(number < 16)	
  //   {
  //     for(int j = number+1; j <= 16; j++){ 
  //       lcd.setCursor(j, 0);
  //      lcd.write(0);
  //     }
  //   } 

  lcd.setCursor(15, 0);
  double inputFreq = spdifIn.getInputFrequency();
  if (inputFreq > 43000){
    lcd.write(7);
  } else{
    lcd.print(" ");
  }
  lcd.setCursor(0, 1);
  lcd.write(6);
  lcd.print(int(volume));
  lcd.setCursor(10, 1);
  lcd.print(voltage);
  lcd.print(" V");
}