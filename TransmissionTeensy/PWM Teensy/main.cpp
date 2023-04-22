#include "ADC.h"
#include "ADC_util.h"
#include <SerialFlash.h>

// Definitions of Constants
#define PINS 18
#define DIG_ADC_0_PINS 10
#define DIG_ADC_1_PINS 10
#define PINS_DIFF 0
#define ADC_READ_PIN 41 // Which pin the ADC will read
#define ADC_OUT_PIN 37 // Which pin PWM will output to
// Change these values to alter the audio
#define RESOLUTION 10 // Resolution of ADC and analogRead
#define FREQUENCY 44100 // Frequency of PWM

// Setting up the ADC Pins
uint8_t adc_pins_dig_ADC_0[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};
uint8_t adc_pins_dig_ADC_1[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};
uint8_t adc_pins_diff[] = {};
const uint32_t num_samples = 10000;

ADC *adc = new ADC();


void setup(){
    // Set up the ADC
    Serial.begin(9600);
    adc->adc0->setResolution(RESOLUTION);
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);

    // Pinmodes
    pinMode(ADC_READ_PIN, INPUT);
    pinMode(ADC_OUT_PIN, OUTPUT);

    analogWriteResolution(RESOLUTION);
    analogWriteFrequency(ADC_OUT_PIN, FREQUENCY);
}
void loop(){
    int adcVal = adc->adc0->analogRead(ADC_READ_PIN);
    if (adcVal){
        analogWrite(ADC_OUT_PIN, adcVal);
        Serial.println(adcVal);
    }
}