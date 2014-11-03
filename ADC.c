// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that sample ADC inputs PE4, PE5 using SS2
// to be triggered by software and trigger two conversions,
// wait for them to finish, and return the two results.
// Team C
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
 
/********************** Includes ***********************/ 

#include "inc/tm4c123gh6pm.h"
#include "ADC.h"

extern uint32_t data[8];
extern uint8_t callMPPT;

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
void ADC0_Init(void){ 
  volatile uint32_t delay;                         
  SYSCTL_RCGCADC_R |= 0x00000001; 					// 1) activate ADC0.
//	SYSCTL_RCGCADC_R |= 0x00000002; 					// 1) activate ADC1. Currently not used.
  
// Initialization of Port E and Port D for analog input.
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4; 	// 1) activate clock for Port E
  delay = SYSCTL_RCGCGPIO_R;      					// 2) allow time for clock to stabilize
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTE_DIR_R &= ~0x3F;      					// 3) make PE0 - PE5 input
  GPIO_PORTE_AFSEL_R |= 0x3F;     					// 4) enable alternate function on PE0 - PE5
  GPIO_PORTE_DEN_R &= ~0x3F;      					// 5) disable digital I/O on PE0 - PE5
																						// 5a) configure PE4 as ?? (skip this line because PCTL is for digital only)
  GPIO_PORTE_PCTL_R &= 0xFF000000;
  GPIO_PORTE_AMSEL_R |= 0x3F;     					// 6) enable analog functionality on PE0 - PE5

  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3; 	// 1) activate clock for Port D
  delay = SYSCTL_RCGCGPIO_R;      					// 2) allow time for clock to stabilize
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTD_DIR_R &= ~0x0C;      					// 3) make PD2 PD3 input
  GPIO_PORTD_AFSEL_R |= 0x0C;     					// 4) enable alternate function on PD2 PD3
  GPIO_PORTD_DEN_R &= ~0x0C;      					// 5) disable digital I/O on PD2 PD3
																						// 5a) configure PD2 PD3 as ?? (skip this line because PCTL is for digital only)
  GPIO_PORTD_PCTL_R &= 0xFFFF00FF;
  GPIO_PORTD_AMSEL_R |= 0x0C;     					// 6) enable analog functionality on PD2 PD3
	
/***** NOTE: Timer initializations are localized in timer.c *****/

//  SYSCTL_RCGCTIMER_R |= 0x02;      								// 0) activate timer1
//	delay = SYSCTL_RCGCTIMER_R;   									// allow time to finish activating
//  TIMER1_CTL_R &= ~0x00000001;    								// 1) disable timer1A during setup
//	TIMER1_CTL_R |= 0x00000020;   	 								// enable timer1A trigger to ADC
//  TIMER1_CFG_R = 0x00000000;       								// 2) configure for 32-bit timer mode
//  TIMER1_TAMR_R = 0x00000002;      								// 3) configure for periodic mode, default down-count settings
//  TIMER1_TAPR_R = 0;               								// 5) ??? I am not sure what this line does. 12.5ns timer1A
//  TIMER1_TAILR_R = period-1;       								// 4) reload value
//	TIMER1_IMR_R &= ~0x00000001;    								// 7) arm timeout interrupt
//	TIMER1_CTL_R |= 0x00000001;							      	// 8) enable timer1A
//	NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 9) Consult NVIC table to adjust this line.
//	NVIC_EN0_R = NVIC_EN0_INT14;     								// 10) enable interrupt 14 (Seq0) in NVIC

/***** NOTE: Timer initializations are localized in timer.c *****/
	
	ADC0_PC_R = 0x01;            							// 2) configure for 125K samples/sec
  ADC0_SSPRI_R = 0x3210;        						// 3) sequencer 0 is highest, sequencer 3 is lowest  
  ADC0_ACTSS_R &= ~0x01;        									// 5) disable sample sequencer 0
  ADC0_EMUX_R = (ADC0_EMUX_R&0xFFFFFFF0)+0x0005; 	// timer trigger event for sequencer 0
  ADC0_SSMUX0_R = 0x45890123; 	        					// 12) set channels for SS0
  ADC0_SSCTL0_R = 0x60000000;         						// 13) no TS0 D0 IE0 END0 TS1 D1, yes IE1 END1               
  ADC0_IM_R |= 0x01;            									// 9) enable SS0 interrupts
  ADC0_ACTSS_R |= 0x01;         									// 10) enable sample sequencer 0	
}

// Initializes ADC1 sampling
// 125k max sampling
// SS0 triggering event: hardware trigger, hardware interrupt when conversion finished sampling
// SS0 1st sample source: PE0
// SS0 2nd sample source: PE1
// ...
// SS0 7th sample source: PD2
// SS0 8th sample source: PD3
void ADC1_Init(void){ 
  volatile uint32_t delay;                         
	SYSCTL_RCGCADC_R |= 0x00000002; 					// 1) activate ADC1. Currently not used.
	
/***** NOTE: Timer initializations are localized in timer.c *****/

//  SYSCTL_RCGCTIMER_R |= 0x04;      								// 0) activate timer2
//	delay = SYSCTL_RCGCTIMER_R;   									// allow time to finish activating
//  TIMER2_CTL_R &= ~0x00000001;    								// 1) disable timer1A during setup
//	TIMER2_CTL_R |= 0x00000020;   	 								// enable timer1A trigger to ADC
//  TIMER2_CFG_R = 0x00000000;       								// 2) configure for 32-bit timer mode
//  TIMER2_TAMR_R = 0x00000002;      								// 3) configure for periodic mode, default down-count settings
//  TIMER2_TAPR_R = 0;               								// 5) ??? I am not sure what this line does. 12.5ns timer1A
//  TIMER2_TAILR_R = period-1;       								// 4) reload value
//	TIMER2_IMR_R &= ~0x00000001;    								// 7) arm timeout interrupt
//	TIMER2_CTL_R |= 0x00000001;							      	// 8) enable timer1A
//	NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 9) Consult NVIC table to adjust this line.
//	NVIC_EN0_R = NVIC_EN0_INT14;     								// 10) enable interrupt 14 (Seq0) in NVIC

/***** NOTE: Timer initializations are localized in timer.c *****/
	
	ADC1_ACTSS_R &= ~0x01;        									// 5) disable sample sequencer 0
  ADC1_EMUX_R = (ADC1_EMUX_R&0xFFFFFFF0)+0x0005; 	// timer trigger event for sequencer 0
  ADC1_SSMUX0_R = 0x45890123; 	        					// 12) set channels for SS0
  ADC1_SSCTL0_R = 0x60000000;         						// 13) no TS0 D0 IE0 END0 TS1 D1, yes IE1 END1               
  ADC1_IM_R |= 0x01;           	 									// 9) enable SS0 interrupts
  ADC1_ACTSS_R |= 0x01;         									// 10) enable sample sequencer 0
}

//------------ADC0Seq0_Handler------------
// Analog to digital conversion ready
// Input: none
// Output: eight 12-bit result of ADC conversions
// Samples Sequencer 0 on ADC0
// 125k max sampling
// data returned by reference to global
void ADC0Seq0_Handler(void){
	
  data[0] = ADC0_SSFIFO0_R&0xFFF;  // PE0) read first result
  data[1] = ADC0_SSFIFO0_R&0xFFF;  // PE1) read second result
  data[2] = ADC0_SSFIFO0_R&0xFFF;  // PE2) read third result
  data[3] = ADC0_SSFIFO0_R&0xFFF;  // PE3) read fourth result
  data[4] = ADC0_SSFIFO0_R&0xFFF;  // PE4) read fifth result
  data[5] = ADC0_SSFIFO0_R&0xFFF;  // PE5) read sixth result
  data[6] = ADC0_SSFIFO0_R&0xFFF;  // PD2) read seventh result
  data[7] = ADC0_SSFIFO0_R&0xFFF;  // PD3) read eighth result	
  ADC0_ISC_R = 0x01;          		 // Last) acknowledge ADC sequence 0 completion
	
	callMPPT = 1;
	
}

// NOTE: ADC1 is not currently assigned a timer.
//			 This handler is dead code, but can be initialized
//			 to expand ADC input.

//------------ADC1Seq0_Handler------------
// Analog to digital conversion ready
// Input: none
// Output: eight 12-bit result of ADC conversions
// Samples Sequencer 0 on ADC1
// 125k max sampling
// data returned by reference to global
void ADC1Seq0_Handler(void)
{ 

  data[0] = ADC1_SSFIFO0_R&0xFFF;  // PE0) read first result
  data[1] = ADC1_SSFIFO0_R&0xFFF;  // PE1) read second result
  data[2] = ADC1_SSFIFO0_R&0xFFF;  // PE2) read third result
  data[3] = ADC1_SSFIFO0_R&0xFFF;  // PE3) read fourth result
  data[4] = ADC1_SSFIFO0_R&0xFFF;  // PE4) read fifth result
  data[5] = ADC1_SSFIFO0_R&0xFFF;  // PE5) read sixth result
  data[6] = ADC1_SSFIFO0_R&0xFFF;  // PD2) read seventh result
  data[7] = ADC1_SSFIFO0_R&0xFFF;  // PD3) read eighth result
  ADC1_ISC_R = 0x0001;             // Last) acknowledge ADC sequence 0 completion	
	
}

