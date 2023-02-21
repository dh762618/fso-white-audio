/*
 * A simple hardware test which receives audio from the audio shield
 * Line-In pins and send it to the Line-Out pins and headphone jack.
 *
 * This example code is in the public domain.
 */

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>

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


const int myInput = AUDIO_INPUT_LINEIN;
//const int myInput = AUDIO_INPUT_MIC;


void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);
  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.7);
  amp1.gain(2);
  biquad1.setLowpass(0, 8900, 0.707);
}

elapsedMillis volmsec=0;

void loop() {
  // every 50 ms, adjust the volume
  if (volmsec > 50) {
    float vol = analogRead(15);
    vol = vol / 1023.0;
    //audioShield.volume(vol); // <-- uncomment if you have the optional
    volmsec = 0;               //     volume pot on your audio shield
  }
}