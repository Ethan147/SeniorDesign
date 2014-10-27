// PWM.c
// Runs on TM4C123
// Use PWM0/PB6 and PWM1/PB7 to generate pulse-width modulated outputs.
// Daniel Valvano
// March 28, 2014

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
  Program 6.7, section 6.3.2

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

/********************** Includes ***********************/

#include "PWM.h"

/********************* Constants ***********************/

#define SYSCTL_RCGCPWM_PWM0       0x00000001  // PWM Clock Gating Control
#define SYSCTL_RCGCPWM_PWM1       0x00000002  // PWM Clock Gating Control

/********************** Prototypes *********************/

long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value

/********************** Externs ************************/

extern uint32_t tachPeriod;
extern uint32_t Speed;

/********************** Globals ************************/

int32_t E = 0;												//Integral of error
uint32_t U = PWM_PERIOD >> 1;					//Current Duty Cycle
uint32_t DesiredSpeed = DEFAULT_RPS;	//Desired Speed in 0.1 rps increments;

/******************* Implementation ********************/

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// period for PB6 and PB7 must be the same
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/2 
//                = 80 MHz/2 = 40 MHz (in this example)
// Output on PB6/M0PWM0
void PWM0_0_Init(void){
  SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_PWM0;  		// 1) activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            				// 2) activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_AFSEL_R |= 0xC0;           				// enable alt funct on PB6
  GPIO_PORTB_PCTL_R &= ~0xFF000000;     				// configure PB6 as PWM0
  GPIO_PORTB_PCTL_R |= 0x44000000;
  GPIO_PORTB_AMSEL_R &= ~0xC0;          				// disable analog functionality on PB6
  GPIO_PORTB_DEN_R |= 0xC0;             				// enable digital I/O on PB6
  SYSCTL_RCC_R = SYSCTL_RCC_USEPWMDIV | 				// 3) use PWM divider
      (SYSCTL_RCC_R & (~SYSCTL_RCC_PWMDIV_M));   
	SYSCTL_RCC_R+=SYSCTL_RCC_PWMDIV_M;						//  configure for no divider
  //PWM0_0_CTL_R = 0;                     			// 4) re-loading down-counting mode									//Enable PWM
  
		
		//PWM0, Generator A (PWM0/PB6) goes to 0 when count==reload and 1 when count==0
	PWM0_0_GENA_R = 0xC8;                 				// low on LOAD, high on CMPA down
																								// PB6 goes low on LOAD
																								// PB6 goes high on CMPA down
		
		
	PWM0_0_GENB_R = 0xC08;                 				// low on LOAD, high on CMPA down
																								// PB7 goes low on LOAD
																								// PB7 goes high on CMPA down
  PWM0_0_LOAD_R = PWM_PERIOD - 1;           		// 5) cycles needed to count down to 0
  PWM0_0_CMPA_R = (PWM_PERIOD >> 1) - 1;        // 6) Start at 50% duty cycle
	PWM0_0_CMPB_R = (PWM_PERIOD >> 1) - 1;        // 6) Start at 50% duty cycle
	
	PWM0_0_CTL_R |= 0x00000003;
	PWM0_ENABLE_R |= 0x00000003;
		
}


// change duty cycle of PB6
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0_0A_Duty(uint16_t duty){
  PWM0_0_CMPA_R = duty - 1;             // 6) count value when output rises
}

// change duty cycle of PB7
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0_0B_Duty(uint16_t duty){
  PWM0_0_CMPB_R = duty - 1;             // 6) count value when output rises
}

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// period for PB6 and PB7 must be the same
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/2 
//                = 80 MHz/2 = 40 MHz (in this example)
// Output on PB7/M0PWM1
void PWM1A_Init(void){
  SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_PWM0;  // 1) activate PWM0
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC; // 2) activate port C
  while((SYSCTL_PRGPIO_R&0x04) == 0){};
    GPIO_PORTC_AFSEL_R |= 0x10;           // enable alt funct on PC4
  GPIO_PORTC_PCTL_R &= ~0x000F0000;     // configure PC4 as PWM0
  GPIO_PORTC_PCTL_R |= 0x00040000;
  GPIO_PORTC_AMSEL_R &= ~0x10;          // disable analog functionality on PC4
  GPIO_PORTC_DEN_R |= 0x10;             // enable digital I/O on PC4
  GPIO_PORTC_DIR_R |= 0x10;             // set PC4 to output
	
//	  SYSCTL_RCC_R = SYSCTL_RCC_USEPWMDIV | 				// 3) use PWM divider
//      (SYSCTL_RCC_R & (~SYSCTL_RCC_PWMDIV_M)); 
//		SYSCTL_RCC_R+=SYSCTL_RCC_PWMDIV_M;						//  configure for no divider
	
//  SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // 3) use PWM divider
//  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; //    clear PWM divider field
//  SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_2;  //    configure for /2 divider
	
  PWM0_3_CTL_R = 0;                     // 4) re-loading mode
	
  PWM0_3_GENA_R = (PWM_3_GENA_ACTCMPAD_ONE|PWM_3_GENA_ACTLOAD_ZERO); //0xC8
	
  PWM0_3_LOAD_R = PWM_PERIOD - 1;           // 5) cycles needed to count down to 0
	
  PWM0_3_CMPA_R = (PWM_PERIOD >> 1) - 1;             // 6) count value when output rises
	
  PWM0_3_CTL_R |= PWM_3_CTL_ENABLE;     // 7) start PWM3
  //PWM0_0_CTL_R |= PWM_1_CTL_ENABLE;     // 7) start PWM3
  PWM0_ENABLE_R |= PWM_ENABLE_PWM6EN; // enable PWM0
}


// change duty cycle of PB5
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM1A_Duty(uint16_t duty){
  PWM0_3_CMPA_R = duty - 1;             // 6) count value when output rises
}


//Sets the desired speed of the motor
//Uses units of 0.1 rps
//e.g. 25 rps would require an input of 250
void setDesiredSpeed(uint32_t newSpeed)
{
	long sr;
  sr = StartCritical(); 
	DesiredSpeed = newSpeed;
	EndCritical(sr);
}
