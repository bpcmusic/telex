/*
 * TELEXo Eurorack Module
 * (c) 2016 Brendon Cassidy
 * MIT License
 */

// debug flag turns on serial debugging over USB
// this can drastically affect performance of the DAC
// depending on where you output to serial - so take care when using it
// #define DEBUG 1

// i2c Wire Library (for Teensy)
#include <i2c_t3.h>

// DAC stuff
#include <SPI.h>
#include "DAC7565.h"

// support libraries
#include "telex.h"
#include "TriggerOutput.h"
#include "CVOutput.h"
#include "TxHelper.h"

// defines
#define WRITERATE 50.
#define LEDINTERVAL 100
#define LOGINTERVAL 10000

#define SAMPLINGRATE 15625
#define LEDRATE 100

/*
 * Ugly Globals
 */

// i2c pullup setting
bool enablePullups = false;

// config inputs
int configPins[] = { 15, 16, 17 };
int configID = TO;

// write timer and its local variables
IntervalTimer writeTimer;
int readerTemp = 0;
int p = 0;

// CV Outputs
DAC dac(-1, 10, -1, 11, 13);
int dacOutputs[] = { DAC_CHANNEL_D, DAC_CHANNEL_C, DAC_CHANNEL_B, DAC_CHANNEL_A };
int pwmLedPins[] = { 3,4,5,6 };
CVOutput *cvOutputs[4];
int writeRate = 100;

// Trigger Outputs
int trLedPins[] = { 0,1,2,7 };
int trPins[] = { 23, 22, 21, 20 };
TriggerOutput *triggerOutputs[4];

// target output
int targetOutput = 0;

// iterator values
int i = 0;
int q = 0;

// timing loop
unsigned long currentTime;
unsigned long nextTime;
unsigned long dacTime;
unsigned long kTime;

#ifdef DEBUG
// led status for tx/rx
int LED = 13;
unsigned long ledInterval;
bool ledOn = false;
// clocking variables
volatile unsigned long n = 0;
unsigned long l = 0;
unsigned long t = 0;
float persec = 0;
#endif

bool dacOn = true;

/*
 * Setup Function 
 */
void setup() {

  delay(1);

  // config
  int cfg = 0;
  for (i=0; i < 3; i++){
    pinMode(configPins[i], INPUT);
    delay(1);
    cfg += digitalRead(configPins[i]) << i;
  }
  configID += cfg;  
  
  writeRate = 1000000 / SAMPLINGRATE;

#ifdef DEBUG
  // set the behind-the-scenes LED output pin
  pinMode(LED, OUTPUT);
  // turn on serial
  Serial.begin(9600);
  // wait for debugging connection   
  while (!Serial);
  // kick off the logging
  t = millis() + LOGINTERVAL;
  Serial.printf("ConfigID: %d\n", configID);
#endif
    
  // initialize the DAC
  if (dacOn) {
    dac.init();
    dac.setReference(DAC_REFERENCE_ALWAYS_POWERED_UP);
    dac.writeChannel(DAC_CHANNEL_ALL, DAC_MAX_SCALE / 2);  
  }
  
  // initialize the outputs
  for (i=0; i < 4; i++) {
    // set up the trigger and cv outputs
    triggerOutputs[i] = new TriggerOutput(trPins[i], trLedPins[i]);
    cvOutputs[i] = new CVOutput(dacOutputs[i], pwmLedPins[i], dac, SAMPLINGRATE);
  }

  // start the write timer
  writeTimer.begin(writeOutputs, writeRate);
  kTime = millis() + LEDRATE;

  // initialize the teensy optimized wire library
  Wire.begin(I2C_SLAVE, configID, I2C_PINS_18_19, enablePullups ? I2C_PULLUP_INT : I2C_PULLUP_EXT, I2C_RATE_400); // I2C_RATE_2400
  Wire.onReceive(receiveEvent);

  
}

/*
 * Main Arduino Appliation Loop
 */
void loop() {

  currentTime = millis();

  // update the TRIGGERS
  for (i=0; i< 4; i++){
    // update the triggers
    triggerOutputs[i]->Update(currentTime);
  } 

  // update the CV LEDs
  if (currentTime >= kTime){
    for (i = 0; i < 4; i++)
      cvOutputs[i]->UpdateLED();
    kTime = currentTime + LEDRATE;
  }

#ifdef DEBUG
    // flash the back LED and log the timing
    if (currentTime >= t){
      noInterrupts();
      l = n;
      n = 0;
      interrupts();
      t = currentTime + LOGINTERVAL;
      persec = l / 10.;
      Serial.printf("per sec: %f\n", persec);    
    }  
    // turn off activity LED
    if (ledOn && currentTime > ledInterval) {
      ledOn = false;
      ledInterval = 0;
    } 
#endif

}

/*
 * Call the Update Function for the Outputs (removed FASTRUN)
 */
void writeOutputs() {
  
#ifdef DEBUG
  // counts the ops/sec
  n++;
#endif
  
  // iterate through the values  
  for (p=0; p< 4; p++){
  
    // update the cv
    cvOutputs[p]->Update();

  } 
}


/*
 * Wire Callback
 */
void receiveEvent(size_t len) {

#ifdef DEBUG
  // set the back LED active to indicate data transfer
  Serial.printf("Event received of size %d \n", len);
  digitalWrite(LED, HIGH);
  ledOn = true;
  ledInterval = millis() + LEDINTERVAL;
#endif

  // parse the response
  TxResponse response = TxHelper::Parse(len);

  // act on the command
  actOnCommand(response.Command, response.Output, response.Value);
  
}


/*
 * Act on the Commands Delivered to the TXo
 */
void actOnCommand(byte cmd, byte out, int value){
  
  // zero-adjust the output number
  targetOutput = out;

  if (targetOutput < 0) return;
  
#ifdef DEBUG
  Serial.printf("Action: %d, Output: %d, Value: %d\n", cmd, targetOutput, value);
#endif

  // noInterrupts();
  switch(cmd) {
    
    case TO_CV_SET:
      // set the value directly - no slew
      cvOutputs[targetOutput]->SetValue(value << 1);
      break;
    
    case TO_CV:     
      // set the target value and slew to it
      cvOutputs[targetOutput]->TargetValue(value << 1); 
      break;  

    case TO_CV_SLEW:
      // set the slew value
      cvOutputs[targetOutput]->SetSlew(value, 0);
      break;
      
    case TO_CV_SLEW_S:
      // set the slew value
      cvOutputs[targetOutput]->SetSlew(value, 1);
      break;
    
    case TO_CV_SLEW_M:
      // set the slew value
      cvOutputs[targetOutput]->SetSlew(value, 2);
      break;

    case TO_CV_OFF:
      // set the offset
      cvOutputs[targetOutput]->SetOffset(value);
      break;

    case TO_CV_QT:
      // Set Pulse Time Format Trigger
      cvOutputs[targetOutput]->TargetQuantizedValue(value);
      break;
      
    case TO_CV_QT_SET:
      // Set Pulse Time Format Trigger
      cvOutputs[targetOutput]->SetQuantizedValue(value);
      break;

    case TO_CV_N:
      // Set Pulse Time Format Trigger
      cvOutputs[targetOutput]->TargetNote(value);
      break;
      
    case TO_CV_N_SET:
      // Set Pulse Time Format Trigger
      cvOutputs[targetOutput]->SetNote(value);
      break;
      
    case TO_CV_SCALE:
      // Set Pulse Time Format Trigger
      cvOutputs[targetOutput]->SetQuantizationScale(value);
      break;

    case TO_OSC:
      cvOutputs[targetOutput]->TargetVOct(value);
      break;

    case TO_OSC_SET:
      cvOutputs[targetOutput]->SetVOct(value);
      break;

    case TO_OSC_QT:
      cvOutputs[targetOutput]->TargetQuantizedVOct(value);
      break;

    case TO_OSC_QT_SET:
      cvOutputs[targetOutput]->SetQuantizedVOct(value);
      break;
      
    case TO_OSC_FQ:
      // 
      cvOutputs[targetOutput]->TargetFrequency(value);
      break;
 
    case TO_OSC_FQ_SET:
      // 
      cvOutputs[targetOutput]->SetFrequency(value);
      break;

    case TO_OSC_N:
      cvOutputs[targetOutput]->TargetOscNote(value);
      break;
      
    case TO_OSC_N_SET:
      cvOutputs[targetOutput]->SetOscNote(value);
      break;
           
    case TO_OSC_LFO:
      // 
      cvOutputs[targetOutput]->TargetLFO(value);
      break;
      
    case TO_OSC_LFO_SET:
      // 
      cvOutputs[targetOutput]->SetLFO(value);
      break;
      
    case TO_OSC_SYNC:
      // 
      cvOutputs[targetOutput]->Sync();
      break;
      
    case TO_OSC_PHASE:
      // 
      cvOutputs[targetOutput]->SetPhaseOffset(value);
      break;
      
    case TO_OSC_WAVE:
      // 
      cvOutputs[targetOutput]->SetWaveform(value);
      break;
      
    case TO_OSC_WIDTH:
      // 
      cvOutputs[targetOutput]->SetWidth(value);
      break;
      
    case TO_OSC_RECT:
      // 
      cvOutputs[targetOutput]->SetRectify(value);
      break;
      
    case TO_OSC_SCALE:
      // 
      cvOutputs[targetOutput]->SetOscQuantizationScale(value);
      break;
      
    case TO_OSC_SLEW:
      // 
      cvOutputs[targetOutput]->SetFrequencySlew(value, 0);
      break;
      
    case TO_OSC_SLEW_S:
      // 
      cvOutputs[targetOutput]->SetFrequencySlew(value, 1);
      break;
      
    case TO_OSC_SLEW_M:
      // 
      cvOutputs[targetOutput]->SetFrequencySlew(value, 2);
      break;


    case TO_ENV_ACT:
      // 
      cvOutputs[targetOutput]->SetEnvelopeMode(value);
      break;
      
    case TO_ENV_ATT:
      // 
      cvOutputs[targetOutput]->SetAttack(value, 0);
      break;
      
    case TO_ENV_ATT_S:
      // 
      cvOutputs[targetOutput]->SetAttack(value, 1);
      break;
      
    case TO_ENV_ATT_M:
      // 
      cvOutputs[targetOutput]->SetAttack(value, 2);
      break;

    case TO_ENV_DEC:
      // 
      cvOutputs[targetOutput]->SetDecay(value, 0);
      break;

    case TO_ENV_DEC_S:
      // 
      cvOutputs[targetOutput]->SetDecay(value, 1);
      break;

    case TO_ENV_DEC_M:
      // 
      cvOutputs[targetOutput]->SetDecay(value, 2);
      break;
      
    case TO_ENV_TRIG:
      // 
      cvOutputs[targetOutput]->TriggerEnvelope();
      break;
   

    case TO_TR:
      // Set Trigger Value
      triggerOutputs[targetOutput]->SetState(value > 0);    
      break;

    case TO_TR_TOG:
      // Toggle Trigger State
      triggerOutputs[targetOutput]->ToggleState();    
      break;

    case TO_TR_TIME:
       // Set Pulse Time for Trigger
      triggerOutputs[targetOutput]->SetTime(value, 0);    
      break;
      
    case TO_TR_TIME_S:
       // Set Pulse Time for Trigger
      triggerOutputs[targetOutput]->SetTime(value, 1);    
      break;
      
    case TO_TR_TIME_M:
       // Set Pulse Time for Trigger
      triggerOutputs[targetOutput]->SetTime(value, 2);    
      break;

    case TO_TR_PULSE:
      // Pulse the Trigger
      triggerOutputs[targetOutput]->Pulse();
      break;

    case TO_TR_POL:
       // Set the Trigger's Polarity
      triggerOutputs[targetOutput]->SetPolarity(value != 0); 
      break;

    case TO_TR_PULSE_DIV:
      // Set Clock Divider
      triggerOutputs[targetOutput]->SetDivision(value);
      break;

    case TO_TR_M_ACT:
      // Set Clock Divider
      triggerOutputs[targetOutput]->SetMetro(value);
      break;

    case TO_TR_M:
       // Set Pulse Time for TR Metro
      triggerOutputs[targetOutput]->SetMetroTime(value, 0);    
      break;
      
    case TO_TR_M_S:
       // Set Pulse Time for TR Metro
      triggerOutputs[targetOutput]->SetMetroTime(value, 1);    
      break;
      
    case TO_TR_M_M:
       // Set Pulse Time for TR Metro
      triggerOutputs[targetOutput]->SetMetroTime(value, 2);    
      break;
        
    case TO_TR_M_BPM:
       // Set Pulse Time for TR Metro
      triggerOutputs[targetOutput]->SetMetroTime(value, 3);    
      break;
              
    case TO_TR_M_SYNC:
       // Sync Pulse Time for TR Metro
      triggerOutputs[targetOutput]->Sync();    
      break;

    case TO_KILL:
      for(int w=0; w<4; w++){
        
        // Kill Each Trigger
        triggerOutputs[w]->Kill();
        
        // stop all slewwing
        cvOutputs[w]->Kill();
        
      } 
      break;
    
  }
  // interrupts();
}


