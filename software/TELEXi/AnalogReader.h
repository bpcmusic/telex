#ifndef AnalogReader_h
#define AnalogReader_h

#include "Arduino.h"

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
    
    int volatile _readBuffer[16];
    int volatile _bufferIndex = 0;

    float Smooth(int value, float previousValue);
    float _c = .985;
    int volatile _smoothedValue = 0.;
    
    int Scale(int value);
    int _calibrationData[3];
    bool _calibrated = false;

    int _i;
    
};


#endif

