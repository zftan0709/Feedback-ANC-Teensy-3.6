/*
 * Feedback Active Noise Cancelling System
 * Main ino file
 * By ZhiFang Tan
 */

#include <arm_math.h>
#include <ADC.h>
#include <DMAChannel.h>

ADC *adc = new ADC();
DMAChannel dma;

const int freq = 4000;
const int micPin = A0;
const int speakerPin = A21;
const int32_t taps = 300;
uint16_t samples[1];

float32_t mu = 0.0001;  // Step Size
float32_t w[taps] ={  // Secondary Path
  0.000263,
  ...,
  0.000048
};

float32_t out;  // Filter Coefficient
float32_t c[taps];
float32_t muedf[taps];
float32_t yQueue[taps];
float32_t dQueue[taps];
float32_t dfQueue[taps];

float32_t y;  // Speaker Output
float32_t yw; // Speaker Output Signal After Secondary Acoustic Path
float32_t d;  // Estimated Reference Signal
float32_t e;  
float32_t mue;  // mu*e
float32_t df; // Filtered Reference Signal

void setup() {
  dmaInit();
  analogWriteResolution(12);
  adc->setAveraging(0);
  adc->setResolution(12);
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED);
  adc->adc0->StartSingleRead(micPin);

  adc->enableDMA(ADC_0);
  adc->adc0->stopPDB();
  adc->adc0->startPDB(freq);
  adc->enableInterupts(ADC_0);

  while(!Serial); // wait for Arduino Serial Monitor
  delay(100);
  Serial.begin(9600);
}

void loop() {
}

void adc0_isr(){
  adc->adc0->readSingle();
  algorithm();
}

void dma_isr(){
  dma.clearInterrupt();
}

void pdb_isr(void){
  PDB0_SC &= ~PDB_SC_PDBIF; // Clear Interrupt
}

void dmaInit(){
  dma.source(*(uint16_t*)&ADC0_RA);
  dma.destinationBuffer(samples,sizeof(samples));
  dma.attachInterrupt(dma_isr);
  dma.interruptAtCompletion();
  dma.triggerAtHardwareEvent(DMAMUX_SOURCE_ADC0);
  dma.enable();
}

void algorithm(){
  
  arm_dot_prod_f32(w,yQueue,taps,&yw);
  
  d = e + yw;

  int i = taps;
  while(i > 1)
  {
    dQueue[i-1] = dQueue[i-2];
    i--;
  }
  dQueue[0] = d;

  arm_dot_prod_f32(dQueue,c,taps,&y);

  out = -y*1958/25 + 1958;
  analogWrite(speakerPin,out);

  i = taps;
  while(i > 1)
  {
    yQueue[i-1] = yQueue[i-2];
    i--;
  }
  yQueue[0] = y;

  arm_dot_prod_f32(yQueue,w,taps,&yw);

  adc->adc0->analogRead(micPin);
  mic = (samples[0]-2048)*1000*1.65/(125.892541*2048);
  e = mic - yw;

  arm_dot_prod_f32(dQueue,w,taps,&df);

  i = taps;
  while(i >1)
  {
    dQueue[i-1] = dfQueue[i-2];
    i--;
  }

  dfQueue[0] = df;
  
  mue = mu * e;

  arm_scale_f32(dfQueue,mue,muedf,taps);
  
  arm_add_f32(c,muedf,c,taps);
}
