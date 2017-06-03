/*
 * TELEXo Eurorack Module
 * (c) 2016 Brendon Cassidy
 * MIT License
 */
 
#ifndef TriggerOutput_h
#define TriggerOutput_h

#include "Arduino.h"
#include "Output.h"

#define MAXTIME 4294967295

class TriggerOutput : public Output
{
  public:
  
    TriggerOutput(int output);
    TriggerOutput(int output, int led);
    
    void Update(unsigned long currentTime);
    
    void SetState(bool state);
    void ToggleState();
    void Pulse();
    void SetPolarity(bool polarity);
    void SetTime(int value, short format);
    void SetWidth(int value);
    
    void SetDivision(int division);
    void SetMultiplier(int multiplier);

    void SetMetro(int state);
    void SetMetro(int state, unsigned long syncTime);
    void SetMetroTime(int value, short format);
    void SetMetroCount(int value);

    void SetMute(bool state);
    
    void Sync(unsigned long syncTime);
    void Sync();
    void Reset();

    // virtual implementations
    void Kill();
    
  protected:
    
  private:
    
    bool _state = false;
    bool _polarity = true;
    unsigned long _toggle = MAXTIME;

    bool _divide = false;
    unsigned short _division = 0;
    unsigned short _counter = 0;

    bool _multiply = false;
    unsigned short _multiplication = 1;
    unsigned short _tempMultiplication = 1;
    unsigned long _multiplyInterval = 1000;
    unsigned long _tempMultiplyInterval = 1000;
    unsigned long _nextNormal = 0;
    int _multiplyCount = 0;

    bool _metro = false;
    unsigned long _metroInterval = 1000;
    unsigned long _nextEvent = 0;

    int _metroCount = 0;
    int _actualCount = -1;

    bool _widthMode = false;
    int _width = 0;
    
    // 100ms is the teletype's default value for the pulse time
    unsigned long _pulseTime = 100;    

    bool _mutePulse = false;
};

#endif

