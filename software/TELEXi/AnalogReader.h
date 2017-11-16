/*
 * TELEXi Eurorack Module
 * (c) 2016 Brendon Cassidy
 * MIT License
 */
 
#ifndef AnalogReader_h
#define AnalogReader_h

#include "Arduino.h"
#include <ResponsiveAnalogRead.h>

#define TOP 16383
#define BOTTOM -16384

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
    
    ResponsiveAnalogRead *_analog;
    
    int volatile _readValue;
    int volatile _latestValue;
    
    int Scale(int value);
    int _calibrationData[3];
    bool _calibrated = false;

    bool _map = false;
    int _top = TOP;
    int _bottom = BOTTOM;
    
    int _i;
    
};


#endif

