// timer.c
// Runs on LM4F120/TM4C123
// Use various timers to enact ADC, PWM, and switch management.
// Team C
// October 29, 2014

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
#define MAX_CHECKS 3	// # checks before a switch is debounced

#define MAX_AMPLITUDE 600 // 620.0

/* Sine Data LookUp Table */
//static const uint16_t sineTableOrig[256]={2884,2954,3025,3095,3165,3235,3305,3375,3444,3512,
//                                3581,3648,3715,3782,3848,3914,3978,4042,4105,4168,
//                                4229,4290,4349,4408,4466,4522,4578,4633,4686,4738,
//                                4789,4839,4887,4934,4980,5024,5067,5109,5149,5187,
//                                5224,5260,5294,5326,5357,5387,5414,5440,5465,5487,
//                                5508,5528,5545,5561,5575,5588,5598,5607,5614,5614,
//                                5614,5614,5614,5614,5614,5614,5607,5598,5588,5575,
//                                5561,5545,5528,5508,5487,5465,5440,5414,5387,5357,
//                                5326,5294,5260,5224,5187,5149,5109,5067,5024,4980,
//                                4934,4887,4839,4789,4738,4686,4633,4578,4522,4466,
//                                4408,4349,4290,4229,4168,4105,4042,3978,3914,3848,
//                                3782,3715,3648,3581,3512,3444,3375,3305,3235,3165,
//                                3095,3025,2954,2884,2813,2742,2672,2601,2531,2461,
//                                2391,2321,2251,2182,2114,2045,1978,1911,1844,1778,
//                                1712,1648,1584,1521,1458,1397,1336,1277,1218,1160,
//                                1104,1048,993,940,888,837,787,739,692,646,602,559,
//                                517,477,439,402,366,332,300,269,239,212,186,161,
//                                139,118,98,81,65,51,38,28,19,12,10,10,10,10,10,10,
//                                12,19,28,38,51,65,81,98,118,139,161,186,212,239,
//                                269,300,332,366,402,439,477,517,559,602,646,692,
//                                739,787,837,888,940,993,1048,1104,1160,1218,1277,
//                                1336,1397,1458,1521,1584,1648,1712,1778,1844,1911,
//                                1978,2045,2114,2182,2251,2321,2391,2461,2531,2601,
//                                2672,2742,2813,2884,2954,3025,3095,3165,3235,
//}; 

/* Externs */

/* Globals */

uint8_t Debounced_State = 0;	// Debounced state of the switches
uint8_t State[MAX_CHECKS];	// Array that maintains bounce status
uint8_t Index = 0;	// Pointer into State	
float sineOutput = 3.14;
uint16_t castFloat;

/* Prototypes */	

float FloatMultiply(float x, float y);
	
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value

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
// Activate Timer1A interrupts to check switch debouncing
// Inputs:  Period value
// Outputs: none
// NOTE: Timer disabled on startup.
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
// Activate Timer2A to request an ADC sample and conversion.
// Sequence interrupt is generated when the sample is ready.
// Inputs:  Period length based on no idea at the moment. Combing files.
// Outputs: none
void Timer2A_Init(uint32_t period)
{
	long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x04;      // 0) activate timer2
  TIMER2_CTL_R &= ~0x00000001;     // 1) disable timer2A during setup
	TIMER2_CTL_R |= 0x00000020;   	 // enable timer2A trigger to ADC
  TIMER2_CFG_R = 0x00000000;       // 2) configure for 32-bit timer mode
  TIMER2_TAMR_R = 0x00000002;      // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;       // 4) reload value
  TIMER2_TAPR_R = 0;               // 5) ??? Not sure what this step is doing
  TIMER2_ICR_R = 0x00000001;       // 6) clear timer2A timeout flag
  TIMER2_IMR_R &= ~0x00000001;     // 7) disarm timeout interrupt
  TIMER2_CTL_R |= 0x00000001;      //	8) enable timer2A
	
	NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
  NVIC_EN0_R = NVIC_EN0_INT14;     // 9) enable interrupt 23 in NVIC

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
  TIMER3_CTL_R &= ~0x00000001;     // 1) disable timer3A during setup
	TIMER3_CTL_R |= 0x00000020;   	 // enable timer2A trigger to ADC
  TIMER3_CFG_R = 0x00000000;       // 2) configure for 32-bit timer mode
  TIMER3_TAMR_R = 0x00000002;      // 3) configure for periodic mode, default down-count settings
  TIMER3_TAILR_R = period-1;       // 4) reload value
  TIMER3_TAPR_R = 0;               // 5) ??? Not sure what this step is doing
  TIMER3_ICR_R = 0x00000001;       // 6) clear timer3A timeout flag
  TIMER3_IMR_R &= ~0x00000001;     // 7) disarm timeout interrupt
  TIMER3_CTL_R |= 0x00000001;      //	8) enable timer3A
	
	NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
  NVIC_EN0_R = NVIC_EN0_INT14;     // 9) enable interrupt 23 in NVIC
  EndCritical(sr);
}

// ***************** Timer2B_Init ****************
// Activate Timer2B to change the note being played.
// Varies with note length
// Inputs:  Period length based on no idea at the moment. Combing files.
// Outputs: none
// NOTE: I've never gotten a B timer to work.
void Timer2B_Init(uint32_t period)
{
	long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x04;     	 // 0) activate timer2
  TIMER2_CTL_R &= ~0x00000100;    	 // 1) disable timer2B during setup
  TIMER2_CFG_R = 0x00000000;       	// 2) configure for 32-bit timer mode
  TIMER2_TBMR_R = 0x00000002;     	 // 3) configure for periodic mode, default down-count settings
  TIMER2_TBILR_R = period-1;       	// 4) reload value
  TIMER2_TBPR_R = 0;               	// 5) ??? Not sure what this step is doing
  TIMER2_ICR_R |= 0x00000100;       // 6) clear timer2B timeout flag
  TIMER2_IMR_R |= 0x00000100;      	// 7) arm timeout interrupt
  
	NVIC_PRI6_R = (NVIC_PRI6_R&0xFFFFFF00)|0x00000080; // 8) priority 4
  NVIC_EN0_R = NVIC_EN0_INT24;     // 9) enable interrupt 24 in NVIC

  EndCritical(sr);
}

// ***************** Timer0A_Handler ****************
// Fire Timer0A interrupt. Moves the SineWave output 
// and sets new duty cycle. Period is TODO ms.
// Inputs:  none
// Outputs: none
void Timer0A_Handler(void)
{
	int32_t sr;
	
	static uint32_t cycleCount = 0;
	float amplitude = MAX_AMPLITUDE; // We should manipulate this based on input voltage characteristics
	
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
	sr = StartCritical();
	
	phaseOne += 1;
	phaseOne &= 0x0FF;
	
	phaseTwo += 1;
	phaseTwo &= 0x0FF;
	
	phaseThree += 1;
	phaseThree &= 0x0FF;
	
	phaseOneSingle += 1;
	phaseOneSingle &= 0x0FF;
	
	phaseTwoSingle += 1;
	phaseTwoSingle &= 0x0FF;	
	
	sineOutput = FloatMultiply(sineTable[phaseOne], amplitude);
	castFloat = sineOutput;
	PWM0_1B_Duty( castFloat);
	
	sineOutput = FloatMultiply(sineTable[phaseTwo], amplitude);
	castFloat = sineOutput;
	PWM0_3A_Duty(castFloat);
	
	sineOutput = FloatMultiply(sineTable[phaseThree], amplitude);
	castFloat = sineOutput;
	PWM0_1A_Duty( castFloat);

// Single Phase Output

//	sineOutput = FloatMultiply(sineTable[phaseOneSingle], amplitude);
//	castFloat = sineOutput;
//	PWM0_1A_Duty( castFloat);
//	
//	sineOutput = FloatMultiply(sineTable[phaseTwoSingle], amplitude);
//	castFloat = sineOutput;
//	PWM0_1B_Duty( castFloat);	

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
	uint32_t curVal = GPIO_PORTF_DATA_R & 0x1F;
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;
	if(curVal == lastVal && curVal == 0)
	{
		//Enable Switch Interrupt
		GPIO_PORTF_IM_R |= 0x1F;
		//Disable Self
		Timer1A_Disable();
	}
	else
		lastVal = curVal;
}

// ***************** Timer2A_Handler ****************
// This timer is tied to ADC0 sampling. As such
// it does not have a timeout handler. It has
// an ADC sequence handler.

// ***************** Timer3A_Handler ****************
// This timer is reserved for ADC1 sampling. As such
// it does not have a timeout handler. It has
// an ADC sequence handler.

// ***************** Timer2B_Handler ****************
// Fire Timer2B interrupt. Moves the song pointer to
// the next note of the song.
// Inputs:  none
// Outputs: none
// NOTE: I've never gotten a B timer to successfully work.
void Timer2B_Handler(void)
{
	int32_t sr;
  TIMER2_ICR_R = 0x000000001;// acknowledge timer2B timeout. This is wrong.
	sr = StartCritical();

	EndCritical(sr);
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

