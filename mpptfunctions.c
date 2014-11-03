/*
   * mpptfunctions.c
   *
   *  Created on : May 10 , 2012
   *  Author : Stefan Moring
   */

 #include "mpptfunctions.h"

 // Define values for the PWM generator and timer0
 #define PWM TOP VALUE 0x00C7
 #define TIMER TOP VALUE 0xC2
 
 extern int data[8];

 // P&O algorithm
 signed int po ( unsigned int P , unsigned int Pold , signed int step_old )
 {
     if (( P > Pold && step_old > 0) | (P < Pold && step_old < 0) )
            return 1;
     else
            return -1;
 }

 // Incremental Conductance Algorithm
 signed int incond ( int V , int dV , int I , int dI )
 {
     if ( dV == 0 && dI == 0)
            return 0;
     else if (( dV == 0) && ( dI > 0) )
            return 1;
     else if ( dV == 0 && dI < 0)
            return -1;
     else if ( dI / dV == -I / V)
            return 0;
     else if ( dI / dV > -I / V)
            return 1;
     else if ( dI / dV < -I / V)
            return -1;
     else
            return 0; // should never happen
 }

 // Function to increase the duty cycle
 void set_duty ( int d)
 {
//     unsigned int temp = OCR1A + d ;
//     if ( temp > PWM_TOP_VALUE )
//            return ;
//     else
//            OCR1A = temp ;
 }


 // Function to measure the voltage
 void read_V ( unsigned int *V )
 // Arguments   *V &     *dV are the address of the registers where the -
 //       measured values should be stored
 {
	  // data[0] is read from PE0
		*V = data[0];

 }

 // Function to measure the current
 void read_I ( unsigned int *I )
 {
	 // data[1] is PE1
		*I = data[1];
 }

 // Function to measure the voltage and calculate dV
 void read_dV ( unsigned int *V , signed int* dV )
 // Arguments   * V &   *dV are the address of the registers where the measured values should be stored
 {
     unsigned int t = 0;
     read_V(&t ) ;
     // Calculate and return dV and I in &dI and &I
     * dV = t - *V ;		// Subtract old voltage from new voltage
     *V = t ;						// Set voltage parameter to new voltage
 }

 // Function to measure the current and calculate dI
 void read_dI ( unsigned int *I , signed int* dI )
 // Arguments   *  I &  *dI are the address of the registers where the -
     //   measured values should be stored
 {
     unsigned int t = 0;
     read_I(&t ) ;
     // Calculate and return dI and I in &dI and &I
     * dI = t - *I ;
     *I = t ;
 }
