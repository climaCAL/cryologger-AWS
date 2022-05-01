#include <SAMD_AnalogCorrection.h>  // https://github.com/arduino/ArduinoCore-samd/tree/master/libraries/SAMD_AnalogCorrection

void setup() 
{
  Serial.begin(115200);
  while (!Serial);
 
  // Configure ADC
  ADC->CTRLA.bit.ENABLE = 0;                      // Disable ADC
  ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV512 |   // Divide Clock ADC GCLK by 512 (48MHz/512 = 93.7kHz)
                   ADC_CTRLB_RESSEL_16BIT;        // Set ADC resolution to 12-bit
  while (ADC->STATUS.bit.SYNCBUSY);               // Wait for synchronization
  ADC->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(64);   // Set Sampling Time Length (341.33 us)
  ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_512 |  // Configure multisampling
                     ADC_AVGCTRL_ADJRES(4);       // Configure averaging
  while (ADC->STATUS.bit.SYNCBUSY);               // Wait for synchronization
  ADC->CTRLA.bit.ENABLE = 1;                      // Enable ADC
  while (ADC->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  analogReadCorrection(30, 2064);
}

void loop() 
{
  float sensorValue1 = analogRead(A3); // HMP60 temperature (CH2)
  float sensorValue2 = analogRead(A4); // HMP60 humidity (CH1)
  
  float extTemperature = mapFloat(sensorValue1, 0, 1222, -40, 60);  // Map temperature from 0-1 V to -40-60°C
  float extHumidity = mapFloat(sensorValue2, 0, 1222, 0, 100);      // Map humidity 0-1 V to 0-100%

  Serial.print(F("extTemperature: ")); Serial.print(sensorValue1); Serial.print(F(",")); Serial.println(extTemperature, 4);
  Serial.print(F("extHumidity: ")); Serial.print(sensorValue2); Serial.print(F(",")); Serial.println(extHumidity, 4);
  delay(500);
}

// Map raw ADC values to floats
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
