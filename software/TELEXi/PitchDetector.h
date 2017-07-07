/*
 * TELEXi Eurorack Module
 * (c) 2016-2017 Brendon Cassidy
 * MIT License
 */
 
#ifndef PitchDetector_h
#define PitchDetector_h

#define FRAMESIZE 512

#define SAMPLERATE 44100.

class PitchDetector {

  public:

    float Detect();
    void LoadSample(int sample);

  private:

    int _frameBuffer[2][FRAMESIZE];
    bool _buffer = false;
    int _location = 0;

    void AnalyzeBuffer();

    float _freq = 0;
  
};


#endif
