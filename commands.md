#TELEX Command Reference

This document outlines the commands for the open source Teletype Expanders TELEXo and TELEXi. You can also use this [Printable Command Reference](extras/TELEX Command Reference.pdf).

####Changelog

The version history of this document.

**Revision v.13c** - 7 January 2017

* changed the extended commands for TIME and SLEW to use a period (.) to delimit the seconds and minutes extensions. For example: TO.TR.TIME.S
* reduced the size of the CALLIBRATE to CALIB command for the TXo

**Revision v.13b** - 16 December 2016

* first version of the command reference document


####Notes

* **Basic Commands** represent commands that have 1 to 1 equivalents in the Teletype command set
* **Extended Commands** represent commands that are new to the expanders but have close equivalents in the Teletype command set
* **Experimental Commands** are commands that are new functionality provided by the expanders; they push the expanders to the edge of performance and should be considered "dangerously fun"
* **n** represents the number of outputs or inputs that you have added to your Teletype (4 of each type for each expander)
* **x** represents the expander unit as opposed to an output (1 for each expander)
* **&alpha;** represents the value that you are supplying to an operator that takes a parameter
* **&alpha;** values for control voltages are *bipolar* on the TELEXo - they accept values between -16384 to 16383

##TELEXo (TXo)

####TXo Trigger Output (TR) Basic Commands

Command  | Description 
------------- | ------------- 
TO.TR 1-n &alpha;   | Set TR value to &alpha; (0/1)    
TO.TR.TOG 1-n  | Toggle TR
TO.TR.PULSE 1-n | Pulse TR using TO.TR.TIME/S/M as an interval
TO.TR.TIME 1-n &alpha; | time for TR.PULSE; &alpha; in milliseconds
TO.TR.POL 1-n &alpha; | polarity for TO.TR.PULSE set to &alpha; (0-1)

####TXo Trigger Output (TR) Extended Commands

Command  | Description 
------------- | ------------- 
TO.TR.TIME.S 1-n &alpha; | time for TR.PULSE; &alpha; in seconds
TO.TR.TIME.M 1-n &alpha; | time for TR.PULSE; &alpha; in minutes

<!---
####TXo Trigger Output (TR) Experimental Commands - Pulse Divider + Metronomes

Command  | Description 
------------- | ------------- 
TO.TR.PULSE.DIV 1-n &alpha; | pulse divider for TR output; &alpha; in # of pulses
TO.TR.M 1-n &alpha; | time for TR.M; &alpha; in milliseconds
TO.TR.M.S 1-n &alpha; | time for TR.M; &alpha; in seconds
TO.TR.M.M 1-n &alpha; | time for TR.M; &alpha; in minutes
TO.TR.M.ACT 1-n &alpha; | activates the metronome for the TR output; &alpha; (0 = off; 1 = on)
TO.TR.M.SYNC 1-n | synchronizes the metronome on the device #
--->

####TXo Control Voltage (CV) Basic Commands

Command  | Description 
------------- | ------------- 
TO.CV 1-n &alpha; | CV target &alpha; (bipolar)
TO.CV.SLEW 1-n &alpha; | CV slew time; &alpha; in milliseconds
TO.CV.SET 1-n &alpha; | set CV to &alpha; (bipolar); ignoring SLEW
TO.CV.OFF 1-n &alpha; | CV offset; &alpha; added at final stage 


####TXo Control Voltage (CV) Extended Commands

Command  | Description 
------------- | ------------- 
TO.CV.SLEW.S 1-n &alpha; | CV slew time; &alpha; in seconds
TO.CV.SLEW.M 1-n &alpha; | CV slew time; &alpha; in minutes
TO.CV.QT 1-n &alpha; | CV target &alpha;; quantized to output's current CV.SCALE
TO.CV.QT.SET 1-n &alpha; | set CV to &alpha;; quantized to output's current CV.SCALE; ignoring SLEW
TO.CV.N 1-n &alpha; | CV target note # &alpha; in output's current CV.SCALE
TO.CV.N.SET 1-n &alpha; | set CV to note # &alpha; in output's current CV.SCALE; ignoring SLEW
TO.CV.SCALE 1-n &alpha; | select scale # &alpha; for individual CV output; see quantization scale reference below

####TXo Control Voltage (CV) Experimental Commands - Oscillator Functions

Note: to start oscillation, set the frequency for the CV output to a value greater than zero. To return it to basic functionality, set it back to zero (0). When in oscillation mode, the current CV value sets the peak for the waveform - think of it like a volume or range control.

Command  | Description 
------------- | ------------- 
TO.OSC 1-n &alpha; | targets oscillation to &alpha; (1v/oct translated)
TO.OSC.SET 1-n &alpha; | sets oscillation to &alpha;; ignores OSC.SLEW
TO.OSC.QT 1-n &alpha; | targets oscillation to &alpha; (1v/oct translated); quantized to current OSC.SCALE
TO.OSC.QT.SET 1-n &alpha; | sets oscillation to &alpha; in current OSC.SCALE; ignores OSC.SLEW
TO.OSC.N 1-n &alpha; | targets oscillation to note # &alpha;; quantized to current OSC.SCALE
TO.OSC.N.SET 1-n &alpha; | sets oscillation to note # &alpha; in current OSC.SCALE; ignores OSC.SLEW
TO.OSC.FQ 1-n &alpha; | targets oscillation to frequency &alpha; in Hz
TO.OSC.FQ.SET 1-n &alpha; | sets oscillation to frequency &alpha; in Hz; ignores OSC.SLEW
TO.OSC.LFO 1-n &alpha; | targets oscillation to frequency &alpha; in mHz (millihertz: 10^-3 Hz)
TO.OSC.LFO.SET 1-n &alpha; | sets oscillation to frequency &alpha; in mHz (millihertz: 10^-3 Hz); ignores OSC.SLEW
TO.OSC.WAVE 1-n &alpha; | set the waveform to sine (0), triangle (1), saw (2), pulse (3), or noise (4)
TO.OSC.SYNC 1-n | resets the phase of the oscillator to zero
TO.OSC.WIDTH 1-n &alpha; | sets the width of the pulse  wave (3) to &alpha; (0 to 100)
TO.OSC.RECT 1-n &alpha; | rectifies the polarity of the oscillator to &alpha; (-2 to 2); see rectification reference
TO.OSC.SLEW 1-n &alpha; | sets the slew time for the oscillator (portamento) to &alpha; (milliseconds)
TO.OSC.SLEW.S 1-n &alpha; | sets the slew time for the oscillator (portamento) to &alpha; (seconds)
TO.OSC.SLEW.M 1-n &alpha; | sets the slew time for the oscillator (portamento) to &alpha; (minutes)
TO.OSC.SCALE 1-n &alpha; | sets the quantization scale for the oscillator to scale # &alpha; (listed below)

####TXo Control Voltage (CV) Experimental Commands - Envelope Generator

Note: when you activate the envelope (using ENV.ACT) your CV output will drop to zero. You need to trigger the envelope (ENV.TRIG) in order to get it to play. This will interact with your currently set CV value for the output making that the (bipolar capable) peak for the envelope. Also, this will interact with the oscillator as well and become a virtual VCA for its output.

Command  | Description 
------------- | ------------- 
TO.ENV.ACT 1-n &alpha; | activates the envelope generator for the CV output; &alpha; (0 = off; 1 = on)
TO.ENV.ATT 1-n &alpha; | attack time for the envelope; &alpha; in milliseconds
TO.ENV.ATT.S 1-n &alpha; | attack time for the envelope; &alpha; in seconds 
TO.ENV.ATT.M 1-n &alpha; | attack time for the envelope; &alpha; in minutes
TO.ENV.DEC 1-n &alpha; | decay time for the envelope; &alpha; in milliseconds 
TO.ENV.DEC.S 1-n &alpha; | decay time for the envelope; &alpha; in seconds
TO.ENV.DEC.M 1-n &alpha; | decay time for the envelope; &alpha; in minutes
TO.ENV.TRIG 1-n | triggers the envelope to play

####TXo Global Commands

This command affects both trigger (TR) and control voltage (CV) outputs.

Command  | Description 
------------- | ------------- 
TO.KILL | cancels TR pulses and CV slews 

#### Rectification Reference

There are several rectification modes available in the oscillator. They are listed below:

Mode | Behavior
--- | ---
-2 | inverts positive values making them negative
-1 | omits all positive values
0 | no rectification; wave is unaffected bipolar
1 | omits all negative values
2 | inverts negative values making them positive


## TELEXi (TXi)

####TXi Basic Commands

Command  | Description 
------------- | ------------- 
TI.IN 1-n | reads the value of the CV input jack (-16384 to 16383)
TI.PARAM 1-n | reads the value of the PARAM knob (0 to 16383) 

####TXi Extended Commands

Command  | Description 
------------- | ------------- 
TI.IN.QT 1-n | return the quantized value for the IN jack; uses input's IN.SCALE
TI.IN.N 1-n | return the quantized note number for the IN jack; uses the input's IN.SCALE
TI.IN.SCALE 1-n &alpha; | sets the current scale for the input to &alpha;; see scale reference below
TI.PARAM.QT 1-n | return the quantized value for the PARAM knob; uses knob's PARAM.SCALE
TI.PARAM.N 1-n | return the quantized note number for the PARAM knob; uses knob's PARAM.SCALE
TI.PARAM.SCALE 1-n &alpha; | sets the current scale for the param knob to &alpha;; see scale reference below

####TXi Experimental Commands

The calibration settings allow you to scale your input values for the IN jacks and the PARAM knobs to compensate for component tolerances. The calibration procedures are listed in the Calibration Details section.

*NOTE: Only the TXi currently has calibration capabilities; these features have not been implemented for the TXo for performance reasons - though, if they are desired, it would be possible to port them over. Go Open Source!*

Command  | Description 
------------- | ------------- 
TI.IN.CALIB 1-n &alpha; | calibrates the scaling for the IN jack; see calibration details below
TI.PARAM.CALIB 1-n &alpha; | calibrates the scaling for the PARAM knob; see calibration details below
TI.STORE 1-x | stores the calibration data for the expander to its flash memory
TI.RESET 1-x | resets the calibration data to factory defaults

####TXi Calibration Details

Calibration for the input module works as follows:

**IN Calibration**

1. Send a -10V signal to the input Z
2. Send the command 'TI.IN.CALIBRATE Z -1'
3. Send a 0V signal to the input Z
4. Send the command 'TI.IN.CALIBRATE Z 0'
5. Send a 10V signal to the input Z
6. Send the command 'TI.IN.CALIBRATE Z 1'

**PARAM Calibration**

1. Turn the PARAM knob Z all the way to the left
2. Send the command 'TI.PARAM.CALIBRATE Z 0'
3. Turn the PARAM knob Z all the way to the right
4. Send the command 'TI.PARAM.CALIBRATE Z 1'

**Save and Reset**

* You can save the calibration data for a device by sending 'TI.STORE N'. N is the number of the device - not the number of an input.

* You can reset the calibration data for a device by sending 'TI.RESET N'. N is the number of the device - not the number of an input.


## Quantization Scale Reference

Scale Number | Scale Name
--- | ---
0 | Standard 12 Tone Equal Temperament **[DEFAULT]** 
1 | 12-tone Pythagorean scale
2 | Vallotti & Young scale (Vallotti version) also known as Tartini-Vallotti (1754)
3 | Andreas Werckmeister's temperament III (the most famous one, 1681)
4 | Wendy Carlos' Alpha scale with perfect fifth divided in nine
5 | Wendy Carlos' Beta scale with perfect fifth divided by eleven
6 | Wendy Carlos' Gamma scale with third divided by eleven or fifth by twenty
7 | Carlos Harmonic & Ben Johnston's scale of 'Blues' from Suite f.micr.piano (1977) & David Beardsley's scale of 'Science Friction'
8 | Carlos Super Just
9 | Kurzweil "Empirical Arabic"
10 | Kurzweil "Just with natural b7th", is Sauveur Just with 7/4
11 | Kurzweil "Empirical Bali/Java Harmonic Pelog"
12 | Kurzweil "Empirical Bali/Java Slendro, Siam 7"
13 | Kurzweil "Empirical Tibetian Ceremonial"
14 | Harry Partch's 43-tone pure scale
15 | Partch's Indian Chromatic, Exposition of Monophony, 1933.
16 | Partch Greek scales from "Two Studies on Ancient Greek Scales" on black/white

