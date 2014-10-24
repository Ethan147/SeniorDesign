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

/********************** Constants **********************/

#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control
#define INTPERIOD              	80000000    // interrupt period (80MHz cycles)

/********************** Externs ************************/

/********************** Globals ************************/

/******************* Implementation ********************/

int main(void)
{
	
/* Initialization */
	
// Hardware
	PLL_Init();	
	Switch_Init();
	
	ADC_Init89();
	
	PWM0A_Init();
	PWM1A_Init();	
	
// Software	
	// Timer Interrupts
	Timer0A_Init(INTPERIOD>>6);
	//Timer1A_Init(INTPERIOD>>4);		//65 ms
	//Timer2A_Init();	
	//Timer3A_Init();
	
	// Activate Interrupts
	EnableInterrupts(); 
	
// Last: Init Screen Output
	Output_Init();

/* Main program loop */

int counter = 0;
char number[10];

	while(1)
	{
		void WaitForInterrupt(void);
		counter += 1;
		if ( !(counter % 1000000 ) )
		{			
			uint32_t data[6];
			ADC_In89(data);
			
			sprintf(number, "ADC1: %05d", data[0]);
			ST7735_SetCursor(0, 0);
			printf("%s", number);
			
			sprintf(number, "ADC2: %05d", data[1]);
			ST7735_SetCursor(0, 1);
			printf("%s", number);

			sprintf(number, "ADC3: %05d", data[2]);
			ST7735_SetCursor(0, 2);
			printf("%s", number);
			
			sprintf(number, "ADC4: %05d", data[3]);
			ST7735_SetCursor(0, 3);
			printf("%s", number);
			
//			sprintf(number, "ADC5: %05d", data[4]);
//			ST7735_SetCursor(0, 4);
//			printf("%s", number);
//			
//			sprintf(number, "ADC6: %05d", data[5]);
//			ST7735_SetCursor(0, 5);
//			printf("%s", number);
		}			
  }
}	
