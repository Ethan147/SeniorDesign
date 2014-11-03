// Main.c
// Runs on LM4F120/TM4C123
// Team C
// October 23, 2014

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

/********************** Includes ***********************/

#include <stdio.h>
#include <stdint.h>

#include "inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"
#include "timer.h"
#include "switch.h"
#include "PWM.h"
#include "ADC.h"

/********************** Prototypes *********************/

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void DelayWait10ms(uint32_t n);
void PrintSplash(void);
void GrabCalibration(void);

/********************** Constants **********************/

#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control
#define INTPERIOD              	80000000    // interrupt period (80MHz cycles)

/********************** Externs ************************/

/********************** Globals ************************/

uint32_t data[8];

/******************* Implementation ********************/

int main(void)
{
	
/* Initialization */
	
// Hardware
	PLL_Init();		
	Switch_Init();	
	
	//PWM0_0_Init();
	PWM0_1_Init();
	PWM0_3_Init();					// This PWM fires at double the frequency of the others.
	
	ADC0_Init();
	
// Software	
	// Timer Interrupts
	Timer0A_Init(INTPERIOD>>6);		// time here. PWM shifting.
	Timer1A_Init(INTPERIOD>>4);		// 65 ms. Debouncing
	Timer2A_Init(INTPERIOD);			// time here. ADC0 sampling.
//	Timer3A_Init();								// time here. ADC1 sampling.
	
// Last: Init Screen Output
	Output_Init();
	PrintSplash();
	GrabCalibration();
	
	// Activate Interrupts
	EnableInterrupts(); 

/* Main program loop */

Output_Clear();

int counter = 0;
char number[10];
uint8_t flag = 0;

	while(1)
	{
		void WaitForInterrupt(void);
		
		counter += 1;
		if ( !(counter % 1000000 ) )
		{			
			
			if( !flag )
			{
				sprintf(number, "ADC0 PE0: %03d%% Voltage", (data[0] * 100) / 4095);
				ST7735_SetCursor(0, 0);
				printf("%s", number);
				
				sprintf(number, "ADC0 PE1: %05d", data[1]);
				ST7735_SetCursor(0, 1);
				printf("%s", number);
				
				sprintf(number, "ADC0 PE2: %05d", data[2]);
				ST7735_SetCursor(0, 2);
				printf("%s", number);
				
				sprintf(number, "ADC0 PE3: %05d", data[3]);
				ST7735_SetCursor(0, 3);
				printf("%s", number);

				sprintf(number, "ADC0 PE4: %05d", data[4]);
				ST7735_SetCursor(0, 4);
				printf("%s", number);
				
				sprintf(number, "ADC0 PE5: %05d", data[5]);
				ST7735_SetCursor(0, 5);
				printf("%s", number);
				
				sprintf(number, "ADC0 PD2: %05d", data[6]);
				ST7735_SetCursor(0, 6);
				printf("%s", number);
				
				sprintf(number, "ADC0 PD3: %05d", data[7]);
				ST7735_SetCursor(0, 7);
				printf("%s", number);
				
				flag = 1;
			}

			else
			{	
				/*
				sprintf(number, "ADC1 PE0: %05d", data[0]);
				ST7735_SetCursor(0, 0);
				printf("%s", number);
				
				sprintf(number, "ADC1 PE1: %05d", data[1]);
				ST7735_SetCursor(0, 1);
				printf("%s", number);
				
				sprintf(number, "ADC1 PE2: %05d", data[2]);
				ST7735_SetCursor(0, 2);
				printf("%s", number);
				
				sprintf(number, "ADC1 PE3: %05d", data[3]);
				ST7735_SetCursor(0, 3);
				printf("%s", number);

				sprintf(number, "ADC1 PE4: %05d", data[4]);
				ST7735_SetCursor(0, 4);
				printf("%s", number);
				
				sprintf(number, "ADC1 PE5: %05d", data[5]);
				ST7735_SetCursor(0, 5);
				printf("%s", number);
				
				sprintf(number, "ADC1 PD2: %05d", data[6]);
				ST7735_SetCursor(0, 6);
				printf("%s", number);
				
				sprintf(number, "ADC1 PD3: %05d", data[7]);
				ST7735_SetCursor(0, 7);
				printf("%s", number);
				*/
				flag = 0;
			}
		}			
  }
}

void PrintSplash(void)
{
	ST7735_SetRotation(1);
	ST7735_DrawBitmap(49, 95, Logo, 40, 68);	
	ST7735_SetRotation(0);
	ST7735_DrawCharS(18, 1, 'P', ST7735_Color565(192, 87, 0), 0, 2);
	ST7735_DrawCharS(30, 1, 'R', ST7735_Color565(255, 255, 255), 0, 2);
	ST7735_DrawCharS(42, 1, 'O', ST7735_Color565(192, 87, 0), 0, 2);
	ST7735_DrawCharS(54, 1, 'G', ST7735_Color565(255, 255, 255), 0, 2);
	ST7735_DrawCharS(66, 1, 'R', ST7735_Color565(192, 87, 0), 0, 2);
	ST7735_DrawCharS(78, 1, 'E', ST7735_Color565(255, 255, 255), 0, 2);
	ST7735_DrawCharS(90, 1, 'S', ST7735_Color565(192, 87, 0), 0, 2);
	ST7735_DrawCharS(102, 1, 'S', ST7735_Color565(255, 255, 255), 0, 2);

	ST7735_SetCursor(2, 2);
	printf("%s", "Solar-Powered");
	ST7735_SetCursor(5, 3);
	printf("%s", "Three-Phase");
	ST7735_SetCursor(8, 4);
	printf("%s", "Motor Output");

	ST7735_SetCursor(0, 9);
	printf("%s", "2014 Honors Team C");
	ST7735_SetCursor(0, 10);
	printf("%s", "Alexander Morgan");
	ST7735_SetCursor(0, 11);
	printf("%s", "Colin Hickman");
	ST7735_SetCursor(0, 12);
	printf("%s", "Nick Ehrmann");
	ST7735_SetCursor(0, 13);
	printf("%s", "John Poole");
	ST7735_SetCursor(0, 14);
	printf("%s", "Matt Ross");

	char loadMenu[7] = "LOADING";
	for( int i = 0; i < 9; i += 1)
	{
		if(i & 0x01)
		{
			// We are printing letters
			for( int j = 0; j < 7; j += 1)
			{
				if(j & 0x01)
					ST7735_DrawCharS(42 + (j*6), 151, loadMenu[j], ST7735_Color565(192, 87, 0), 0, 1);
				else
					ST7735_DrawCharS(42 + (j*6), 151, loadMenu[j], ST7735_Color565(255, 255, 255), 0, 1);
				DelayWait10ms(40);
			}
		}
		else
		{
			// We are erasing letters
			for( int j = 0; j < 7; j += 1)
			{
				ST7735_SetCursor(j + 7, 15);
				printf("%s", " ");
				DelayWait10ms(40);
			}
		}
	}	
}

uint8_t continueFlag = 0;
uint8_t printFlag = 0;
uint32_t motorConstant = 0;

void GrabCalibration(void)
{
	Output_Clear();
	ST7735_SetCursor(0, 3);
	printf("%s", "Please input a motor");
	ST7735_SetCursor(5, 4);
	printf("%s", "calibration");
	ST7735_SetCursor(6, 5);
	printf("%s", "constant:");
	
	ST7735_SetCursor(8, 8);
	printf("%04d", motorConstant);
	
	while ( ! continueFlag )
	{
		// Wait for continue flag from user
		if(printFlag)
		{
			// Update the number on the screen
			ST7735_SetCursor(8, 8);
			printf("%04d", motorConstant);
			printFlag = 0;
		}
	}
	
	GPIO_PORTF_IM_R &= ~(0x1F);      		// (f) disarm interrupt on PF4 - PF0
	
}

// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n)
{
	uint32_t volatile time;
  while(n)
	{
    time = 727240*2/91;  // 10msec
    while(time)
		{
			time--;
		}
    n--;
  }
}

