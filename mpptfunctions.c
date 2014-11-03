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
 
 uint8_t index;
 float ivCurve[24][2] = {{0.0, 89.3}, {1.0, 89.3}, {2.0, 87.9}, {3.0, 87.5}, {4.0, 87.5},
			{5.0, 86.6}, {6.0, 86.5}, {7.0, 86.4}, {8.0, 86.0}, {9.0, 85.0},
			{10.0, 84.0}, {11.0, 83.4}, {12.0, 82.9}, {13.0, 80.5}, {14.0, 78.5},
			{15.0, 76.3}, {16.0, 73.6}, {17.0, 69.3}, {18.0, 63.8}, {19.0, 56.4},
			{20.0, 47.5}, {21.0, 36.2}, {22.0, 20.8}, {23.6, 0.0}};

 // P&O algorithm
 signed int po ( unsigned int P , unsigned int Pold , signed int step_old )
 {
     if (( P > Pold && step_old > 0) | (P < Pold && step_old < 0) )
            return 1;
     else
            return -1;
 }

 // Incremental Conductance Algorithm
 int32_t incond ( int V , int dV , int I , int dI )
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
 void read_V ( uint32_t *V )
 // Arguments   *V &     *dV are the address of the registers where the -
 //       measured values should be stored
 {
	  // data[0] is read from PE0
		//*V = data[0];
	 *V = ivCurve[index][0];

 }

 // Function to measure the current
 void read_I ( uint32_t *I )
 {
	 // data[1] is PE1
		//*I = data[1];
	 *I = ivCurve[index][1];
 }

 // Function to measure the voltage and calculate dV
 void read_dV ( uint32_t *V , int32_t* dV )
 // Arguments   * V &   *dV are the address of the registers where the measured values should be stored
 {
     unsigned int t = 0;
     read_V(&t ) ;
     // Calculate and return dV and I in &dI and &I
     * dV = t - *V ;		// Subtract old voltage from new voltage
     *V = t ;						// Set voltage parameter to new voltage
 }

 // Function to measure the current and calculate dI
 void read_dI ( uint32_t *I , int32_t* dI )
 // Arguments   *  I &  *dI are the address of the registers where the -
     //   measured values should be stored
 {
     unsigned int t = 0;
     read_I(&t ) ;
     // Calculate and return dI and I in &dI and &I
     * dI = t - *I ;
     *I = t ;
 }
