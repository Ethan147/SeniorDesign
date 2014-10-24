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
void PWM0A_Init(void){
  SYSCTL_RCGCPWM_R |= 0x01;             				// 1) activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            				// 2) activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_AFSEL_R |= 0x40;           				// enable alt funct on PB6
  GPIO_PORTB_PCTL_R &= ~0x0F000000;     				// configure PB6 as PWM0
  GPIO_PORTB_PCTL_R |= 0x04000000;
  GPIO_PORTB_AMSEL_R &= ~0x40;          				// disable analog functionality on PB6
  GPIO_PORTB_DEN_R |= 0x40;             				// enable digital I/O on PB6
  SYSCTL_RCC_R = SYSCTL_RCC_USEPWMDIV | 				// 3) use PWM divider
      (SYSCTL_RCC_R & (~SYSCTL_RCC_PWMDIV_M));   
	SYSCTL_RCC_R+=SYSCTL_RCC_PWMDIV_M;						//  configure for no divider
  //PWM0_0_CTL_R = 0;                     			// 4) re-loading down-counting mode
	PWM0_0_CTL_R |= 0x00000001;
	PWM0_ENABLE_R |= 0x00000001;									//Enable PWM
  PWM0_0_GENA_R = 0xC8;                 				// low on LOAD, high on CMPA down
																								// PB6 goes low on LOAD
																								// PB6 goes high on CMPA down
  PWM0_0_LOAD_R = PWM_PERIOD - 1;           		// 5) cycles needed to count down to 0
  PWM0_0_CMPA_R = (PWM_PERIOD >> 1) - 1;        // 6) Start at 50% duty cycle
		
}


// change duty cycle of PB6
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0A_Duty(uint16_t duty){
  PWM0_0_CMPA_R = duty - 1;             // 6) count value when output rises
}

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// period for PB6 and PB7 must be the same
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/2 
//                = 80 MHz/2 = 40 MHz (in this example)
// Output on PB7/M0PWM1
void PWM1A_Init(void){
  SYSCTL_RCGCPWM_R |= 0x04;             				// 1) activate PWM1
  SYSCTL_RCGCGPIO_R |= 0x02;            				// 2) activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_AFSEL_R |= 0x10;           				// enable alt funct on PB4
  GPIO_PORTB_PCTL_R &= ~0x000F0000;     				// configure PB4 as PWM0
  GPIO_PORTB_PCTL_R |= 0x00040000;
  GPIO_PORTB_AMSEL_R &= ~0x10;          				// disable analog functionality on PB4
  GPIO_PORTB_DEN_R |= 0x10;             				// enable digital I/O on PB4
  SYSCTL_RCC_R = SYSCTL_RCC_USEPWMDIV | 				// 3) use PWM divider
      (SYSCTL_RCC_R & (~SYSCTL_RCC_PWMDIV_M));   
	SYSCTL_RCC_R+=SYSCTL_RCC_PWMDIV_M;						//  configure for no divider
  //PWM0_0_CTL_R = 0;                     			// 4) re-loading down-counting mode
	PWM0_1_CTL_R |= 0x00000001;
	PWM0_ENABLE_R |= 0x00000001;									//Enable PWM
  PWM0_1_GENA_R = 0xC8;                 				// low on LOAD, high on CMPA down
																								// PB5 goes low on LOAD
																								// PB5 goes high on CMPA down
  PWM0_1_LOAD_R = PWM_PERIOD - 1;           		// 5) cycles needed to count down to 0
  PWM0_1_CMPA_R = (PWM_PERIOD >> 1) - 1;        // 6) Start at 50% duty cycle
		
}


// change duty cycle of PB5
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM1A_Duty(uint16_t duty){
  PWM0_1_CMPA_R = duty - 1;             // 6) count value when output rises
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
