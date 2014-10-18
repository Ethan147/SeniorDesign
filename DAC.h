// DAC.h
// File to initialize and define a DAC driver

#include <stdint.h>
#include <stdio.h>

#include "inc/tm4c123gh6pm.h"

void DAC_Init(uint8_t data);
//void DAC_Out(uint16_t outA);

//Made a macro to speed it up
//No need to JSR
#define DAC_Out(outA) {SSI1_DR_R = outA;}
