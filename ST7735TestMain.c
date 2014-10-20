// ST7735TestMain.c
// Runs on LM4F120/TM4C123
// Test the functions in ST7735.c by printing basic
// patterns to the LCD.
// Daniel Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

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

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdio.h>
#include <stdint.h>

#include "ST7735.h"
#include "PLL.h"
#include "inc/tm4c123gh6pm.h"
//#include "timer.h"
#include "lcd.h"
#include "switch.h"
#include "DAC.h"
#include "motor.h"
#include "ADC.h"

void DelayWait10ms(uint32_t n);
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08

#define M_PI 3.14159265358979323846
#define INTPERIOD              80000000         // interrupt period (80MHz cycles)
#define PTB6									 0x40025100

/* Externs */

/* Globals */

unsigned long AdcIn1, AdcIn2, AdcIn3, AdcIn4;
uint32_t* 	songPointer;
unsigned long ADCvalue;

/* Implementation */

void Producer (unsigned long ADCdata)
{
}
int main(void)
{
	
/* Initialization */
	// Initialization of Hardware
	PLL_Init();
	//Switch_Init();
	//uint8_t data = 0;
	//DAC_Init(data);
	//PWM0A_Init();
	//ADC0_InitSWTriggerSeq3_Ch9();
	
	// Initialization of Software
	
	// Timer Interrupt Initialization
  ADC0_InitSWTriggerSeq3(0);            // allow time to finish activating
	//Timer0A_Init(INTPERIOD>>6);
	//Timer1A_Init(INTPERIOD>>4);		//65 ms
	//Initializes interrupt for first note's frequency
	//Initializes interrupt for changing to next note
	//Timer2A_Init(notes[songPtr.curSong][0][LENGTH]);
	
	//Timer3A_Init(notes[songPtr.curSong][0][LENGTH]);
	// Activate Interrupts
	Output_Init();
	EnableInterrupts(); 
	
	ST7735_InitR(INITR_REDTAB);

/* Main program loop */

ST7735_SetCursor(0, 0);
printf("Switch 1: Play/Pause");

int counter = 0;
char number[10];

	while(1)
	{
		void WaitForInterrupt(void);
		counter += 1;
		if ( !(counter % 1000000 ) )
		{
			long result = ADC0_InSeq3();
			sprintf(number, "ADC: %05d", (int)result);
			ST7735_SetCursor(0, 1);
			printf(number);
		}			
  }
}

// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
	  	time--;
    }
    n--;
  }
}
	
