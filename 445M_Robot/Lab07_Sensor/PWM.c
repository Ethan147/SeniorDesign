// PWM.c
// Runs on TM4C123
// Use PWM0/PB6 to generate pulse-width modulated outputs.
// Daniel Valvano
// September 3, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013
  Program 6.7, section 6.3.2

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

#define PWM0_ENABLE_R           (*((volatile unsigned long *)0x40028008))
#define PWM_ENABLE_PWM6EN       0x00000040  // PWM0 Output Enable
#define PWM_ENABLE_PWM7EN       0x00000080  // PWM0 Output Enable
#define PWM0_3_CTL_R            (*((volatile unsigned long *)0x40028100))
#define PWM_3_CTL_ENABLE        0x00000001  // PWM Block Enable
#define PWM0_3_LOAD_R           (*((volatile unsigned long *)0x40028110))
#define PWM0_3_CMPA_R           (*((volatile unsigned long *)0x40028118))
#define PWM0_3_GENA_R           (*((volatile unsigned long *)0x40028120))
#define PWM0_3_CMPB_R           (*((volatile unsigned long *)0x4002811C))
#define PWM0_3_GENB_R           (*((volatile unsigned long *)0x40028124))
#define PWM_3_GENA_ACTCMPAD_ONE 0x000000C0  // Set the output signal to 1
#define PWM_3_GENA_ACTLOAD_ZERO 0x00000008  // Set the output signal to 0
#define PWM_3_GENB_ACTCMPBD_ONE 0x00000C00  // Set the output signal to 1
#define PWM_3_GENB_ACTLOAD_ZERO 0x00000008  // Set the output signal to 0
#define GPIO_PORTC_DIR_R      (*((volatile unsigned long *)0x40006400))
#define GPIO_PORTC_AFSEL_R      (*((volatile unsigned long *)0x40006420))
#define GPIO_PORTC_DEN_R        (*((volatile unsigned long *)0x4000651C))
#define GPIO_PORTC_AMSEL_R      (*((volatile unsigned long *)0x40006528))
#define GPIO_PORTC_PCTL_R       (*((volatile unsigned long *)0x4000652C))
#define SYSCTL_RCC_R            (*((volatile unsigned long *)0x400FE060))
#define SYSCTL_RCC_USEPWMDIV    0x00100000  // Enable PWM Clock Divisor
#define SYSCTL_RCC_PWMDIV_M     0x000E0000  // PWM Unit Clock Divisor
#define SYSCTL_RCC_PWMDIV_2     0x00000000  // /2
#define SYSCTL_RCGCPWM_R        (*((volatile unsigned long *)0x400FE640))
#define SYSCTL_RCGCPWM_PWM0       0x00000001  // PWM Clock Gating Control
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOC      0x00000004  // Port B Clock Gating Control

#define PC6                     (*((volatile unsigned long *)0x0005A200))
#define PC7                     (*((volatile unsigned long *)0x0005A100))

void (*fps[100])(void);

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/2 
//                = 50 MHz/2 = 25 MHz (in this example)
void Motor_Init(unsigned short period, unsigned short duty){
  volatile unsigned long delay;
  SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_PWM0;  // 1) activate PWM0
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC; // 2) activate port D
  delay = SYSCTL_RCGC2_R;               // allow time to finish activating 
	GPIO_PORTC_AFSEL_R |= 0x30;           // enable alt funct on PC4-5
  GPIO_PORTC_PCTL_R &= ~0x00FF0000;     // configure PC4-5 as PWM0
  GPIO_PORTC_PCTL_R |= 0x00440000;
  GPIO_PORTC_AMSEL_R &= ~0xF0;          // disable analog functionality on PC4-7
  GPIO_PORTC_DEN_R |= 0xF0;             // enable digital I/O on PC4-7
	GPIO_PORTC_DIR_R |= 0xF0;             // set PC4-7 to output
  SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // 3) use PWM divider
  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; //    clear PWM divider field
  SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_2;  //    configure for /2 divider
  PWM0_3_CTL_R = 0;                     // 4) re-loading mode
  PWM0_3_GENA_R = (PWM_3_GENA_ACTCMPAD_ONE|PWM_3_GENA_ACTLOAD_ZERO);
  PWM0_3_GENB_R = (PWM_3_GENB_ACTCMPBD_ONE|PWM_3_GENB_ACTLOAD_ZERO);
  PWM0_3_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM0_3_CMPA_R = period / 2;             // 6) count value when output rises
  PWM0_3_CMPB_R = period / 2 ;             // 6) count value when output rises
  PWM0_3_CTL_R |= PWM_3_CTL_ENABLE;     // 7) start PWM3
  //PWM0_0_CTL_R |= PWM_1_CTL_ENABLE;     // 7) start PWM3
  PWM0_ENABLE_R |= PWM_ENABLE_PWM6EN | PWM_ENABLE_PWM7EN; // enable PWM0
}
// change duty cycle
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void Motor_Left_Duty(unsigned short duty){
  PWM0_3_CMPA_R = duty - 1;             // 6) count value when output rises
}

void Motor_Right_Duty(unsigned short duty){
  PWM0_3_CMPB_R = duty - 1;             // 6) count value when output rises
}

void Motor_Left_Direction(unsigned char dir){
	PC6 = dir << 6;
}

void Motor_right_Direction(unsigned char dir){
	PC7 = dir << 7;
}
