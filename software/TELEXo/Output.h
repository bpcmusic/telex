#ifndef Output_h
#define Output_h

#include "Arduino.h"

#define MAXTIME 4294967295

class Output
{
  public:
  
    Output(int output);
    Output(int output, int led);

    // virtual functions
    virtual void Kill() = 0;   
    
  protected:
  
    int _output = -1;
    int _led = -1;
    

  private:
 

};

#endif

