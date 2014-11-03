/*
   * mpptfunctions.c
   *
   *  Created on : May 10 , 2012
   *  Author : Stefan Moring
   */

 // Define values for the PWM generator and timer0
 #define PWM TOP VALUE 0x00C7
 #define TIMER TOP VALUE 0xC2

 // P&O algorithm
 signed int po ( unsigned int P , unsigned int Pold , signed int step_old );

 // Incremental Conductance Algorithm
 signed int incond ( int V , int dV , int I , int dI );

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
 void read_V ( unsigned int *V );
 // Arguments   *V &     *dV are the address of the registers where the -
//        measured values should be stored


 // Function to measure the current
 void read_I ( unsigned int *I );

 // Function to measure the voltage and calculate dV
 void read_dV ( unsigned int *V , signed int* dV );
 // Arguments   * V &   *dV are the address of the registers where the measured values should be stored


 // Function to measure the current and calculate dI
 void read_dI ( unsigned int *I , signed int* dI );
 // Arguments   *  I &  *dI are the address of the registers where the -
 //       measured values should be stored

