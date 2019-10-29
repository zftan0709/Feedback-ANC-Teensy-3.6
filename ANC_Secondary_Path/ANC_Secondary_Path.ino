/*
 * Feedback Active Noise Cancelling System
 * Secondary Acoustic Path Estimation
 * By ZhiFang Tan
 */

#include <arm_math.h>
#include <ADC.h>
#include <DMAChannel.h>

// Initialize ADC and DMA
ADC *adc = new ADC();
DMAChannel dma;

const int freq = 4000;  // Sampling Frequency
const int micPin = A0;  // Error Mic Pin
const int speakerPin = A21; // Speaker Pin
const int16_t taps = 300; //  Tap Length of Secondary Path Coefficient
uint16_t samples[1];  // Buffer for DMA
uint16_t r;

float32_t d;  // Error Mic Input
float32_t mu = 0.0000005; // Step Size
float32_t x[taps];  // Generated Noise Queue
float32_t w[taps];  // Secondary Path Coefficient
float32_t muEX[taps]; // mu*e*x Queue
float32_t y;  // Estimated Error Mic Input
float32_t e;  // e = d - y
float32_t mue;  // mu*e

void setup() {
  dmaInit();  // Initialize DMA source and interrupt
  pinMode(speakerPin,OUTPUT); // Initialize speaker output pin
  analogWriteResolution(12);  // 12 bit resolution on DAC(Digital to Analog Converter) 0 - 4095
  adc->setAveraging(4);
  adc->setResolution(12);
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED);
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);
  adc->adc0->startSingleRead(micPin);
  adc->enableDMA(ADC_0);
  adc->adc0->stopPDB();
  adc->adc0->startPDB(freq);  // Initialize PDB
  adc->enableInterrupts(ADC_0); // Must be included for PDB to work
  
  while(!Serial);
  delay(100);
  Serial.begin(9600);
}

void loop() {
}

void dmaInit(){
  // Initialize DMA trigger on ADC0
  dma.source(*(uint16_t*) &ADC0_RA);
  dma.destinationBuffer(samples,sizeof(samples));
  dma.attachInterrupt(dma_isr);
  dma.interruptAtCompletion();
  dma.triggerAtHardwareEvent(DMAMUX_SOURCE_ADC0);
  dma.enable();
}

void algorithm(){
  // Shift elements in Queue to the right and add latest element at the 0 position
  int k = taps;
  while(k > 1)
  {
    x[k-1] = x[k-2];
    k--;
  }
  r = random(0,3916);
  [0] = (r-1958)*25/1958;

  // Write signal to the speaker
  analogWrite(speakerPin,r);

  // Dot product
  arm_dot_prod_f32(x,w,tops,&y);

  adc->adc0->analogRead(micPin);

  /*
   * SparkFun MEMS Microphone Breakout - INMP401(ADMP401)
   * Sentivity of -42dBV
   * dbV = 20*log((Sensitivity_mV/Pa)/(Output_AREF)
   * Output_AREF = 1V/Pa
   * Sensitivity_V/Pa = 125.89254117941672 V/Pa
   * 
   */
   d = (samples[0]-2048)*1.65*1000/(125.892541*2048);

   e = d - y;

   mue = mu*e;

   arm_scale_f32(x,mue,muEX,taps);

   arm_add_f32(w,muEX,w,taps);
}

void adc0_isr(){
  adc->adc0->readSingle();
  algorithm();
  if(Serial.read()>0)
  {
    int i = 0;
    while(i < taps)
    {
      Serial.print(w[i],6);
      Serial.println(",");
      i++;
    }
  }
}

void dma_isr(){
  dma.clearInterrupt();
}

void pdb_isr(void)
{
  PDB0_SC %= ~PDB_SC_PDBIF;
}
