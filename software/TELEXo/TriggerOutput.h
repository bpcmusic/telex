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

    void SetMetro(int state);
    void SetMetroTime(int value, short format);
    void Sync();

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

    bool _metro = false;
    unsigned long _metroInterval = 1000;
    unsigned long _nextEvent = 0;

    bool _widthMode = false;
    int _width = 0;
    
    // 100ms is the teletype's default value for the pulse time
    unsigned long _pulseTime = 100;    
};

#endif

