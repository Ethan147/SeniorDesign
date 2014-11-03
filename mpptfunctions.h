/*
   * mpptfunctions.c
   *
   *  Created on : May 10 , 2012
   *  Author : Stefan Moring
   */

#include <stdint.h>

 // Define values for the PWM generator and timer0
 #define PWM TOP VALUE 0x00C7
 #define TIMER TOP VALUE 0xC2

 // P&O algorithm
 signed int po ( unsigned int P , unsigned int Pold , signed int step_old );

 // Incremental Conductance Algorithm
 int32_t incond ( int V , int dV , int I , int dI );

 // Function to sweep through the spectrum , not tested !
 void sweep (void);

 // Function to increase the duty cycle
 void set_duty ( int d);

 // Function to initialize timer0
 void init_timer (void);

 // Function to initialize the ADC channels
 void init_adc (void);

 // Function to initialize the PWM generator
 void init_pwm (void);

 // Function to measure the voltage
 void read_V ( uint32_t *V );
 // Arguments   *V &     *dV are the address of the registers where the -
//        measured values should be stored


 // Function to measure the current
 void read_I ( uint32_t *I );

 // Function to measure the voltage and calculate dV
 void read_dV ( uint32_t *V , int32_t* dV );
 // Arguments   * V &   *dV are the address of the registers where the measured values should be stored


 // Function to measure the current and calculate dI
 void read_dI ( uint32_t *I , int32_t* dI );
 // Arguments   *  I &  *dI are the address of the registers where the -
 //       measured values should be stored

