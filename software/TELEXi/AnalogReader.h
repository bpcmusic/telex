/*
 * TELEXi Eurorack Module
 * (c) 2016 Brendon Cassidy
 * MIT License
 */
 
#ifndef AnalogReader_h
#define AnalogReader_h

#include "Arduino.h"

#define TOP 16383
#define BOTTOM -16384

// shifted by one to hanndle the equals case
#define LOWTRIGGER 1639
#define HIGHTRIGGER 4914

#define PULSEROUNDING 500


/*
 * helper class created for the TELEXi to read and scale inputs
 */
class AnalogReader
{
  public:

    AnalogReader(int address);
    AnalogReader(int address, bool reverse);
    
    int Read();
    int GetLatest();

    void SetTop(int top);
    void SetBottom(int bottom);
    void SetMap (int top, int bottom);
    
    void Calibrate(int measure);
    bool GetCalibrated();
    void SetCalibrated(bool calibrated);
    void GetCalibrationData(int data[3]);
    void SetCalibrationData(int measure, int value);

  private:
  
    int _address;
    bool _reverse = false;
    
    int volatile _readValue;
    int volatile _latestValue;

    bool _pulsePolarity = true;
    bool _state = false;
    unsigned long _pulseDuration = 0;
    unsigned long _pulseDistance = 0;
    elapsedMicros _timeSinceLastPulse;

    void TriggerTracker();
    
    int volatile _readBuffer[16];
    int volatile _bufferIndex = 0;

    float Smooth(int value, float previousValue);
    float _c = .985;
    float volatile _smoothedValue = 0.;
    
    int Scale(int value);
    int _calibrationData[3];
    bool _calibrated = false;

    bool _map = false;
    int _top = TOP;
    int _bottom = BOTTOM;
    
    int _i = 0;
    int _k = 0;
    
};


#endif

