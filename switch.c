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
#define GetPeriod(x)					80000000 / (256*x)
/* Prototypes */

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

/* Globals */

/* Externs */

extern uint8_t continueFlag;
extern uint8_t printFlag;
extern uint32_t motorConstant;
extern uint32_t frequency;

extern uint8_t callMPPT;

/* Implementation */

void Switch_Init(void){  
	#pragma diag_suppress 550 // supress #177-D function  was declared but never referenced
	uint32_t delay;
  SYSCTL_RCGC2_R |= 0x00000020; 		// (a) activate clock for port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
	GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4 - PF0
	GPIO_PORTF_DIR_R &= ~0x1F;    		// (c) make PF4 - PF1 input
  GPIO_PORTF_AFSEL_R &= ~0x1F;  		//     disable alt funct on PF4 - PF1
  GPIO_PORTF_DEN_R |= 0x1F;     		//     enable digital I/O on PF4 - PF1 
	GPIO_PORTF_PCTL_R = 0x000FFFFF; 	// configure PF4 - PF1 as GPIO 
	GPIO_PORTF_AMSEL_R = 0;       		//     disable analog functionality on PF
	GPIO_PORTF_AMSEL_R &= ~(0x00);
  GPIO_PORTF_IS_R &= ~0x1F;     		// (d) PF3-1 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x1F;    		//     PF3-1 is not both edges
  GPIO_PORTF_IEV_R |= 0x1F;    			//     PF4 rising edge event
  GPIO_PORTF_ICR_R = 0x1F;      		// (e) clear flags
  GPIO_PORTF_IM_R |= 0x1F;      		// (f) arm interrupt on PF4 - PF0
	
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
}

//Handles button pressing
void GPIOPortF_Handler(void)
{
	int32_t sr;

		if( GPIO_PORTF_RIS_R & 0x01)	//PF0
		{
			int mask = 0x01;
			//Switch 1 Handler
			GPIO_PORTF_ICR_R |= 0x01;      // acknowledge flag
			sr = StartCritical();
			Timer1A_Enable();
			
			frequency = (frequency == 30) ? 0 : (frequency + 1);
			
			// set GPIO pins to communicate frequency to other board
			for(;mask < 0x0F ; mask *= 2){
				if(frequency & mask){
					GPIO_PORTB_DATA_R |= mask;
				}else{
					GPIO_PORTB_DATA_R &= ~(mask);
				}
	  	}
			if(frequency & mask){
				GPIO_PORTC_DATA_R |= 0x40;
			}else{
				GPIO_PORTC_DATA_R &= ~(0x40);
			}
			
			TIMER0_TAILR_R = GetPeriod(frequency) - 1;
			EndCritical(sr);
		}	
		if( GPIO_PORTF_RIS_R & 0x02)	//PF1
		{
			//Switch 1 Handler
			GPIO_PORTF_ICR_R |= 0x02;      // acknowledge flag
			sr = StartCritical();
			
			Timer1A_Enable();
			EndCritical(sr);
		}
		if( GPIO_PORTF_RIS_R & 0x04)	//PF2
		{
			//Switch 2 Handler
			GPIO_PORTF_ICR_R |= 0x04;      // acknowledge flag
			sr = StartCritical();

			Timer1A_Enable();
			if(motorConstant > 0)
				motorConstant -= 1;
			printFlag = 1;
			EndCritical(sr);
		}
		if( GPIO_PORTF_RIS_R & 0x08)	//PF3	
		{
			//Switch 3 Handler
			GPIO_PORTF_ICR_R |= 0x08;      // acknowledge flag
			sr = StartCritical();

			Timer1A_Enable();
			motorConstant += 1;
			printFlag = 1;
			EndCritical(sr);
		}
		if( GPIO_PORTF_RIS_R & 0x10)	//PF4	
		{
			//Switch 4 Handler
			GPIO_PORTF_ICR_R |= 0x10;      // acknowledge flag
			sr = StartCritical();
			
			Timer1A_Enable();
			continueFlag = 1;
			EndCritical(sr);
		}	

		callMPPT = 1;
		
}

void Comm_Init(){
	uint32_t delay;
   SYSCTL_RCGCGPIO_R |= 0x02; 		// (a) activate clock for port B
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
	GPIO_PORTB_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port B
	GPIO_PORTB_CR_R = 0x0F;           // allow changes to PB3 - PB0
	GPIO_PORTB_DIR_R |= 0x0F;    		// (c) make PB3 - PB0 output
  GPIO_PORTB_AFSEL_R &= ~0x0F;  		//     disable alt funct on PB3 - PB0
  GPIO_PORTB_DEN_R |= 0x0F;     		//     enable digital I/O on PB3 - PB0
	GPIO_PORTB_PCTL_R = 0x0000FFFF; 	// configure PB3 - PB0 as GPIO 
	GPIO_PORTB_AMSEL_R = 0;       		//     disable analog functionality on PB
	GPIO_PORTB_AMSEL_R &= ~(0x00);
	
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC; 		// 2) activate port C
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
	GPIO_PORTC_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port C
	GPIO_PORTC_CR_R = 0x40;           // allow changes to PC6
	GPIO_PORTC_DIR_R |= 0x40;    		// (c) make PC6 output
  GPIO_PORTC_AFSEL_R &= ~0x40;  		//     disable alt funct on PC6
  GPIO_PORTC_DEN_R |= 0x40;     		//     enable digital I/O on PC6
	GPIO_PORTC_PCTL_R = 0x0F000000; 	// configure PC5 as GPIO 
	GPIO_PORTC_AMSEL_R = 0;       		//     disable analog functionality on PC6
	GPIO_PORTC_AMSEL_R &= ~(0x00);
}

