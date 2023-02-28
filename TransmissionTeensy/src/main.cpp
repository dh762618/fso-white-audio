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
//#include <../include/LiquidCrystal_I2C.h>

// Global Vars
// double voltage = 0;
// int loopCounter = 0; //Counts how many times the loop is entered - used to slow down the voltage reading

// Set up LCD
// LiquidCrystal_I2C lcd(0x27, 16 ,2);

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
// possibly touch buttons for sound effects

const int myInput = AUDIO_INPUT_LINEIN;
//const int myInput = AUDIO_INPUT_MIC;

// Declare LCDOutput
// void LCDOutput(double voltage);


void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.7);
  int gain = 1;
  amp1.gain(gain);
  
  // Implement filter
  biquad1.setLowpass(0, 8900, 0.707);

  // Implement characters to be displayed on LCD Screen
  // lcd.init();
  // lcd.createChar();

  // Create LCD Splash Screen
  // lcd.backlight();
  // lcd.setCursor(0,0);
  // lcd.print("Transmitting Teensy");
  // lcd.setCursor(0,1);
  // lcd.print("FSO White");
  // delay(1000);
  // lcd.clear();
}

elapsedMillis volmsec=0;

void loop() {
  // Every 50 ms, adjust the volume
  if (volmsec > 50) {
    float vol = analogRead(15);
    vol = vol / 1023.0;
    //audioShield.volume(vol); // <-- uncomment if you have the optional
    volmsec = 0;               //     volume pot on your audio shield
  }

  // Update the voltage of the signal
  // if (loopCounter == 5){
  //   analogReadResolution(12);
  //   double reading = 0;
  //   reading = analogRead(A13);
  //   voltage = reading / 1023.0;
  //   loopCounter = 0;
  // }

  // Update the LCD Screen
  // LCDOutput();

  delay(50);
}

// void LCDOutput(double voltage)
// {
//   lcd.setCursor(10, 1);
//   lcd.print(voltage);
//   lcd.print(" V");
// }