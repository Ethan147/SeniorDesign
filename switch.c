// Switch.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize a GPIO as an input pin and
// allow reading of switches on PF1, PF2 and PF3
// Use bit-banded I/O.
// Daniel and Jonathan Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013
   Section 4.2    Program 4.2

  "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Example 2.3, Program 2.9, Figure 2.36

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

// negative logic switches connected to PF0 and PF4 on the Launchpad
// red LED connected to PF1 on the Launchpad
// blue LED connected to PF2 on the Launchpad
// green LED connected to PF3 on the Launchpad
// NOTE: The NMI (non-maskable interrupt) is on PF0.  That means that
// the Alternate Function Select, Pull-Up Resistor, Pull-Down Resistor,
// and Digital Enable are all locked for PF0 until a value of 0x4C4F434B
// is written to the Port F GPIO Lock Register.  After Port F is
// unlocked, bit 0 of the Port F GPIO Commit Register must be set to
// allow access to PF0's control registers.  On the LM4F120, the other
// bits of the Port F GPIO Commit Register are hard-wired to 1, meaning
// that the rest of Port F can always be freely re-configured at any
// time.  Requiring this procedure makes it unlikely to accidentally
// re-configure the JTAG and NMI pins as GPIO, which can lock the
// debugger out of the processor and make it permanently unable to be
// debugged or re-programmed.

/* Includes */

#include "switch.h"

/* Constants */

#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define PF0                     (*((volatile uint32_t *)0x40025004))
#define PF4                     (*((volatile uint32_t *)0x40025040))
#define PA5                     (*((volatile uint32_t *)0x40004080))
#define SWITCHES                (*((volatile uint32_t *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control

#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04

/* Prototypes */

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

/* Globals */

/* Externs */

/* Implementation */

void Switch_Init(void){  
	#pragma diag_suppress 550 // supress #177-D function  was declared but never referenced
	uint32_t delay;
  SYSCTL_RCGC2_R |= 0x00000020; 		// (a) activate clock for port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
	GPIO_PORTF_CR_R = 0x1E;           // allow changes to PF4 - PF1
	GPIO_PORTF_DIR_R &= ~0x1E;    		// (c) make PF4 - PF1 input
  GPIO_PORTF_AFSEL_R &= ~0x1E;  		//     disable alt funct on PF4 - PF1
  GPIO_PORTF_DEN_R |= 0x1E;     		//     enable digital I/O on PF4 - PF1 
	GPIO_PORTF_PCTL_R = 0x0000FFF0; 	// configure PF4 - PF1 as GPIO 
	GPIO_PORTF_AMSEL_R = 0;       		//     disable analog functionality on PF
	GPIO_PORTF_AMSEL_R &= ~(0x00);
  GPIO_PORTF_IS_R &= ~0x1E;     		// (d) PF3-1 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x1E;    		//     PF3-1 is not both edges
  GPIO_PORTF_IEV_R |= 0x1E;    			//     PF4 rising edge event
  GPIO_PORTF_ICR_R = 0x1E;      		// (e) clear flags
  GPIO_PORTF_IM_R |= 0x1E;      		// (f) arm interrupt on PF4 - PF1
	
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
}

//Handles button pressing
void GPIOPortF_Handler(void)
{
	int32_t sr;
	
		if(0x02)	//PF1
		{
			//Switch 1 Handler
			GPIO_PORTF_ICR_R |= 0x01;      // acknowledge flag
			sr = StartCritical();
			GPIO_PORTF_DATA_R = 0x00;
			GPIO_PORTF_IM_R &= ~0x1E;
			Timer1A_Enable();
			EndCritical(sr);
		}
		if(0x04)	//PF2
		{
			//Switch 2 Handler
			GPIO_PORTF_ICR_R |= 0x02;      // acknowledge flag
			sr = StartCritical();
			GPIO_PORTF_DATA_R = 0x00;
			GPIO_PORTF_IM_R &= ~0x1E;
			Timer1A_Enable();
			EndCritical(sr);
		}
		if(0x08)	//PF3	
		{
			//Switch 3 Handler
			GPIO_PORTF_ICR_R |= 0x08;      // acknowledge flag
			sr = StartCritical();
			GPIO_PORTF_DATA_R = 0x00;
			GPIO_PORTF_IM_R &= ~0x1E;
			Timer1A_Enable();
			EndCritical(sr);
		}
		if(0x10)	//PF4	
		{
			//Switch 4 Handler
			GPIO_PORTF_ICR_R |= 0x10;      // acknowledge flag
			sr = StartCritical();
			GPIO_PORTF_DATA_R = 0x00;
			GPIO_PORTF_IM_R &= ~0x1E;
			Timer1A_Enable();
			EndCritical(sr);
		}		
}

