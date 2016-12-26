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

    // virtual implementations
    void Kill();
    
  protected:
    
  private:
    
    bool _state = false;
    bool _polarity = true;
    unsigned long _toggle = MAXTIME;
    
    // 100ms is the teletype's default value for the pulse time
    unsigned long _pulseTime = 100;    
};

#endif

