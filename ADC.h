// ADCSWTrigger.h
// Runs on LM4F120/TM4C123
// Provide functions that sample ADC inputs PE4, PE5 using SS2
// to be triggered by software and trigger two conversions,
// wait for them to finish, and return the two results.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
 
#include <stdint.h>

// There are many choices to make when using the ADC, and many
// different combinations of settings will all do basically the
// same thing.  For simplicity, this function makes some choices
// for you.  When calling this function, be sure that it does
// not conflict with any other software that may be running on
// the microcontroller.  Particularly, ADC0 sample sequencer 2
// is used here because it takes up to four samples, and two
// samples are needed.  Sample sequencer 2 generates a raw
// interrupt when the second conversion is complete, but it is
// not promoted to a controller interrupt.  Software triggers
// the ADC0 conversion and waits for the conversion to finish.
// If somewhat precise periodic measurements are required, the
// software trigger can occur in a periodic interrupt.  This
// approach has the advantage of being simple.  However, it does
// not guarantee real-time.
//
// A better approach would be to use a hardware timer to trigger
// the ADC conversion independently from software and generate
// an interrupt when the conversion is finished.  Then, the
// software can transfer the conversion result to memory and
// process it after all measurements are complete.

// Initializes ADC0 sampling
// 125k max sampling
// SS0 triggering event: hardware trigger, hardware interrupt when conversion finished sampling
// SS0 1st sample source: PE0
// SS0 2nd sample source: PE1
// ...
// SS0 7th sample source: PD2
// SS0 8th sample source: PD3
void ADC0_Init(void);

// Initializes ADC1 sampling
// 125k max sampling
// SS0 triggering event: hardware trigger, hardware interrupt when conversion finished sampling
// SS0 1st sample source: PE0
// SS0 2nd sample source: PE1
// ...
// SS0 7th sample source: PD2
// SS0 8th sample source: PD3
void ADC1_Init(void);

//------------ADC0Seq0_Handler------------
// Analog to digital conversion ready
// Input: none
// Output: eight 12-bit result of ADC conversions
// Samples Sequencer 0 on ADC0
// 125k max sampling
// data returned by reference to global
void ADC0Seq0_Handler(void);

//------------ADC1Seq0_Handler------------
// Analog to digital conversion ready
// Input: none
// Output: eight 12-bit result of ADC conversions
// Samples Sequencer 0 on ADC1
// 125k max sampling
// data returned by reference to global
void ADC1Seq0_Handler(void);
