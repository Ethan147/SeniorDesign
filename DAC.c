// DAC.c
// File to initialize and define a DAC driver

#include "DAC.h"

void DAC_Init(uint8_t data)
{
	// Function to intialize the DAC on a specific port
	// Currently uses port D0-3
// Old Init	
//	SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate port D
//  while((SYSCTL_PRGPIO_R&0x08)==0){};   // allow time for clock to stabilize
//                                    // 2) no need to unlock PD3-0
//  GPIO_PORTD_AMSEL_R &= ~0x0F;      // 3) disable analog functionality on PD3-0
//  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // 4) GPIO
//  GPIO_PORTD_DIR_R |= 0x0F;         // 5) make PD3-0 out
//  GPIO_PORTD_AFSEL_R &= ~0x0F;      // 6) regular port function 
//  GPIO_PORTD_DEN_R |= 0x0F;         // 7) enable digital I/O on PD3-0
		
	
  SYSCTL_RCGCSSI_R |= 0x02;  // activate SSI1
  SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate port D
	while((SYSCTL_PRGPIO_R&0x08)==0){};   // allow time for clock to stabilize
                                    // 2) no need to unlock PD3-0
  GPIO_PORTD_AFSEL_R |= 0x0F;           // enable alt funct on PD3-0
  GPIO_PORTD_DEN_R |= 0x0F;             // enable digital I/O on PD3-0
                                        // configure PD3-0 as SSI
		// TODO: Please note I am not sure about this line. Ask TA.
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF0F00)+0x00002022;
  GPIO_PORTD_AMSEL_R = 0;               // disable analog functionality on PA
  SSI1_CR1_R &= ~SSI_CR1_SSE;           // disable SSI
  SSI1_CR1_R &= ~SSI_CR1_MS;            // master mode
                                        // clock divider for 8 MHz SSIClk (assumes 16 MHz PIOSC)
  SSI1_CPSR_R = (SSI1_CPSR_R&~SSI_CPSR_CPSDVSR_M)+4;
  SSI1_CR0_R &= ~(SSI_CR0_SCR_M |       // SCR = 0 (8 Mbps data rate)
                  SSI_CR0_SPH |         // SPH = 0
                  SSI_CR0_SPO);         // SPO = 0
                                        // FRF = Freescale format
  SSI1_CR0_R = (SSI1_CR0_R&~SSI_CR0_FRF_M)+SSI_CR0_FRF_MOTO;
                                        // DSS = 16-bit data
  SSI1_CR0_R = (SSI1_CR0_R&~SSI_CR0_DSS_M)+SSI_CR0_DSS_16;
  SSI1_DR_R = data;                     // load 'data' into transmit FIFO
  SSI1_CR1_R |= SSI_CR1_SSE;            // enable SSI		
		
}

//void DAC_Out(uint16_t outA)
//{
//	//Function to output DAC information on a specific port.
//	while((SSI1_SR_R&SSI_SR_TNF)==0){};// wait until room in FIFO
//  //SSI1_DR_R = 0x100+outA;              // data out OutA
//		SSI1_DR_R = outA;
//}
