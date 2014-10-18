// Timer0A.c
// Runs on LM4F120/TM4C123
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013
   Volume 1, Program 9.8

  "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013
   Volume 2, Program 7.5, example 7.6

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
 
/* Includes */

#include "inc/tm4c123gh6pm.h"
#include "timer.h"
#include "switch.h"
#include "DAC.h"
#include "PWM.h"

/* Constants */

#define NVIC_EN0_INT19          0x00080000  // Interrupt 19 enable
#define TIMER_CFG_16_BIT        0x00000004  // 16-bit timer configuration,
                                            // function is controlled by bits
                                            // 1:0 of GPTMTAMR and GPTMTBMR
#define TIMER_TAMR_TACDIR       0x00000010  // GPTM Timer A Count Direction
#define TIMER_TAMR_TAMR_PERIOD  0x00000002  // Periodic Timer mode
#define TIMER_CTL_TAEN          0x00000001  // GPTM TimerA Enable
#define TIMER_IMR_TATOIM        0x00000001  // GPTM TimerA Time-Out Interrupt
                                            // Mask
#define TIMER_ICR_TATOCINT      0x00000001  // GPTM TimerA Time-Out Raw
                                            // Interrupt
#define TIMER_TAILR_TAILRL_M    0x0000FFFF  // GPTM TimerA Interval Load
                                            // Register Low
																						
const unsigned short TempSinWave[32] = 
	{  
		1024,1122,1215,1302,1378,1440,1486,1514,1524,1514,1486,
		1440,1378,1302,1215,1122,1024,926,833,746,670,608,
		562,534,524,534,562,608,670,746,833,926
	}; 
	
	

#define MAX_CHECKS 3	// # checks before a switch is debounced
uint8_t Debounced_State = 0;	// Debounced state of the switches
uint8_t State[MAX_CHECKS];	// Array that maintains bounce status
uint8_t Index = 0;	// Pointer into State	
	

/* Externs */
extern bool debounce;
extern uint8_t timerCount;

/* Prototypes */	
	
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
bool DebounceSwitch2(void);

/* Implementation */	

// ***************** Timer0A_Init ****************
// Activate Timer0A interrupts to run user task periodically
// Used for outputing to the DAC
// Inputs:  task is a pointer to a user function
//          period in 12.5ns units
// Outputs: none
void Timer0A_Init(uint32_t period)
{
	long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x01;      // 0) activate timer0
  TIMER0_CTL_R &= ~0x00000001;     // 1) disable timer0A during setup
  TIMER0_CFG_R = 0x00000000;       // 2) configure for 32-bit timer mode
  TIMER0_TAMR_R = 0x00000002;      // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = period-1;       // 4) reload value
  TIMER0_TAPR_R = 0;               // 5) ??? I am not sure what this line does.
  TIMER0_ICR_R = 0x00000001;       // 6) clear timer0A timeout flag
  TIMER0_IMR_R |= 0x00000001;      // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // 8) priority 2
  NVIC_EN0_R = NVIC_EN0_INT19;     // 9) enable interrupt 19 in NVIC
  TIMER0_CTL_R |= 0x00000001;      // 10) enable timer0A.
	
  EndCritical(sr);
}

// ***************** Timer1A_Init ****************
// Activate Timer1A interrupts to run user task periodically
// Inputs:  Period value
// Outputs: none
void Timer1A_Init(uint32_t period)
{
	long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x02;      // 0) activate timer1
  TIMER1_CTL_R &= ~0x00000001;     // 1) disable timer1A during setup
  TIMER1_CFG_R = 0x00000000;       // 2) configure for 32-bit timer mode
  TIMER1_TAMR_R = 0x00000002;      // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = period-1;       // 4) reload value
  TIMER1_TAPR_R = 0;               // 5) ??? I am not sure what this line does. 12.5ns timer1A
  TIMER1_ICR_R = 0x00000001;       // 6) clear timer1A timeout flag
	TIMER1_IMR_R |= 0x00000001;    	 // 7) arm timeout interrupt
  
	NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
  NVIC_EN0_R = NVIC_EN0_INT21;     // 9) enable interrupt 21 in NVIC
  
  EndCritical(sr);
}

// ***************** Timer2A_Init ****************
// Activate Timer2A to change the note being played.
// Varies with note length
// Inputs:  Period length based on no idea at the moment. Combing files.
// Outputs: none
void Timer2A_Init(uint32_t period)
{
	long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x04;      // 0) activate timer2
  TIMER2_CTL_R &= ~0x00000001;     // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;       // 2) configure for 32-bit timer mode
  TIMER2_TAMR_R = 0x00000002;      // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;       // 4) reload value
  TIMER2_TAPR_R = 0;               // 5) ??? Not sure what this step is doing
  TIMER2_ICR_R = 0x00000001;       // 6) clear timer2A timeout flag
  TIMER2_IMR_R |= 0x00000001;      // 7) arm timeout interrupt
  
	NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
  NVIC_EN0_R = NVIC_EN0_INT23;     // 9) enable interrupt 23 in NVIC
  
	//TIMER2_CTL_R |= 0x00000001;      // 10) enable timer2A. Do not init on start.
  EndCritical(sr);
}

// ***************** Timer3A_Init ****************
// Activate TIMER3 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer3A_Init( uint32_t period)
{
	long sr;
  sr = StartCritical();
  SYSCTL_RCGCTIMER_R |= 0x08;   // 0) activate TIMER3
  TIMER3_CTL_R = 0x00000000;    // 1) disable TIMER3A during setup
  TIMER3_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER3_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER3_TAILR_R = period-1;    // 4) reload value
  TIMER3_TAPR_R = 0;            // 5) bus clock resolution
  TIMER3_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER3_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI8_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN1_R = 1<<3;           // 9) enable IRQ 35 in NVIC
  TIMER3_CTL_R = 0x00000001;    // 10) enable TIMER3A
  EndCritical(sr);
}

// ***************** Timer2B_Init ****************
// Activate Timer2B to change the note being played.
// Varies with note length
// Inputs:  Period length based on no idea at the moment. Combing files.
//// Outputs: none
//void Timer2B_Init(uint32_t period)
//{
//	long sr;
//  sr = StartCritical(); 
//  SYSCTL_RCGCTIMER_R |= 0x04;     	 // 0) activate timer2
//  TIMER2_CTL_R &= ~0x00000100;    	 // 1) disable timer2B during setup
//  TIMER2_CFG_R = 0x00000000;       	// 2) configure for 32-bit timer mode
//  TIMER2_TBMR_R = 0x00000002;     	 // 3) configure for periodic mode, default down-count settings
//  TIMER2_TBILR_R = period-1;       	// 4) reload value
//  TIMER2_TBPR_R = 0;               	// 5) ??? Not sure what this step is doing
//  TIMER2_ICR_R |= 0x00000100;       // 6) clear timer2B timeout flag
//  TIMER2_IMR_R |= 0x00000100;      	// 7) arm timeout interrupt
//  
//	NVIC_PRI6_R = (NVIC_PRI6_R&0xFFFFFF00)|0x00000080; // 8) priority 4
//  NVIC_EN0_R = NVIC_EN0_INT24;     // 9) enable interrupt 24 in NVIC

//  EndCritical(sr);
//}

// ***************** Timer0A_Handler ****************
// Fire Timer0A interrupt. Moves the SineWave output 
// and sets new duty cycle. Period is TODO ms.
// Inputs:  none
// Outputs: none
void Timer0A_Handler(void){
	int32_t sr;
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
	sr = StartCritical();
	
	phaseOne += 1;
	//phaseOne &= 0x0FF;
	if(phaseOne == 256)
	{
		phaseOne = 0;
	}
	
	PWM0A_Duty( (uint16_t) (sineTable[phaseOne] * 39800));
	EndCritical(sr);
}

// ***************** Timer1A_Handler ****************
// Fire Timer1A interrupt. Check value of switch. If value
// is the same for 2 consecutive interrupts consider it stable and
// re-enable switches. Period is 125 ms.
// Inputs:  none
// Outputs: none
uint32_t lastVal = 0;
void Timer1A_Handler(void)
{
	uint32_t curVal = GPIO_PORTF_DATA_R & 0x1E;
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;
	if(curVal == lastVal && curVal == 0)
	{
		//Enable Switch Interrupt
		GPIO_PORTF_IM_R |= 0x1E;
		//Disable Self
		Timer1A_Disable();
	}
	else
		lastVal = curVal;
}

//Moves to the next note
// ***************** Timer2A_Handler ****************
// Fire Timer2A interrupt. Moves the song pointer to
// the next note of the song.
// Inputs:  none
// Outputs: none
//Controler for PWM
//Executes once every 10 ms
void Timer2A_Handler(void)
{long sr; int32_t P;
	TIMER2_ICR_R = 0x01;
	sr = StartCritical();
	EndCritical(sr);
	PWM0A_Duty(P);
}

//Moves to the next note
// ***************** Timer2B_Handler ****************
// Fire Timer2B interrupt. Moves the song pointer to
// the next note of the song.
// Inputs:  none
// Outputs: none
void Timer3A_Handler(void)
{
	int32_t sr;
  TIMER3_ICR_R = 0x000000001;// acknowledge timer2B timeout
	sr = StartCritical();
//	songPtr.notePtr ++;
//	//Restarts song if at the end of the note
//	if(songPtr.notePtr >= songPtr.songLen)
//		songPtr.notePtr = 0;
//		//Music_NextSong();
//	//Sets time between updates of the DAQ
//	//Basically the frequency
//	TIMER0_TAILR_R = (uint32_t)notes[songPtr.curSong][songPtr.notePtr][NOTE];
//	//Sets number of cycles until the next note
//	TIMER2_TAILR_R = (uint32_t)notes[songPtr.curSong][songPtr.notePtr][LENGTH];
//	Music_SetEnvelope();
//	printf("Interrupt 2 Processed");
	EndCritical(sr);
}


// ***************** Timer0A_Enable ****************
// Enables Timer0A
// Inputs:  none
// Outputs: none
void Timer0A_Enable(void)
{
	TIMER0_CTL_R |= 0x00000001;      //enable timer0A
}

// ***************** Timer0A_Disable ****************
// Disables Timer0A
// Inputs:  none
// Outputs: none
void Timer0A_Disable(void)
{
	TIMER0_CTL_R &= ~0x00000001;      //Disable timer0A
}

// ***************** Timer1A_Enable ****************
// Enables Timer1A
// Inputs:  none
// Outputs: none
void Timer1A_Enable(void)
{
	TIMER1_CTL_R |= 0x00000001;      //enable timer1A
}

// ***************** Timer1A_Disable ****************
// Disables Timer1A
// Inputs:  none
// Outputs: none
void Timer1A_Disable(void)
{
	TIMER1_CTL_R &= ~0x00000001;      //Disable timer1A
}

// ***************** Timer2A_Enable ****************
// Enables Timer2A
// Inputs:  none
// Outputs: none
void Timer2A_Enable(void)
{
	TIMER2_CTL_R |= 0x00000001;      //enable timer2A
}

// ***************** Timer0A_Disable ****************
// Disables Timer0A
// Inputs:  none
// Outputs: none
void Timer2A_Disable(void)
{
	TIMER2_CTL_R &= ~0x00000001;      //Disable timer2A
}



