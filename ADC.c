// ADC.c
// Runs on LM4F120
// Provides functions to access ADC and sample. Uses code from Daniel Valvano's
// ADCT0ATrigger.c to do timeinterrupted samples.
// Victor Vo and Alex Taft
// January 28, 2014

#include "ADC.h"

//COPIED CODE FROM ADT0ATrigger.c

#define NVIC_EN_INT14           0x00004000  // Interrupt 14 enable
#define NVIC_EN_INT15           0x00008000  // Interrupt 15 enable
#define NVIC_EN_INT16           0x00010000  // Interrupt 16 enable
#define NVIC_EN_INT17           0x00020000  // Interrupt 17 enable
#define NVIC_EN_INT18           0x00040000  // Interrupt 18 enable
#define NVIC_EN_INT19           0x00080000  // Interrupt 19 enable
#define NVIC_EN1_R              (*((volatile unsigned long *)0xE000E104))
#define NVIC_EN0_R              (*((volatile unsigned long *)0xE000E100))  // IRQ 0 to 31 Set Enable Register
#define NVIC_PRI3_R             (*((volatile unsigned long *)0xE000E40C))
#define NVIC_PRI4_R             (*((volatile unsigned long *)0xE000E410))  // IRQ 16 to 19 Priority Register
#define NVIC_PRI12_R            (*((volatile unsigned long *)0xE000E430))
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTD_DIR_R        (*((volatile unsigned long *)0x40007400))
#define GPIO_PORTD_AFSEL_R      (*((volatile unsigned long *)0x40007420))
#define GPIO_PORTD_DEN_R        (*((volatile unsigned long *)0x4000751C))
#define GPIO_PORTD_AMSEL_R      (*((volatile unsigned long *)0x40007528))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define TIMER0_CFG_R            (*((volatile unsigned long *)0x40030000))
#define TIMER0_TAMR_R           (*((volatile unsigned long *)0x40030004))
#define TIMER0_CTL_R            (*((volatile unsigned long *)0x4003000C))
#define TIMER0_IMR_R            (*((volatile unsigned long *)0x40030018))
#define TIMER0_TAILR_R          (*((volatile unsigned long *)0x40030028))
#define TIMER0_TAPR_R           (*((volatile unsigned long *)0x40030038))
#define TIMER_CFG_16_BIT        0x00000004  // 16-bit timer configuration,
                                            // function is controlled by bits
                                            // 1:0 of GPTMTAMR and GPTMTBMR
#define TIMER_TAMR_TACDIR       0x00000010  // GPTM Timer A Count Direction
#define TIMER_TAMR_TAMR_PERIOD  0x00000002  // Periodic Timer mode
#define TIMER_CTL_TAOTE         0x00000020  // GPTM TimerA Output Trigger
                                            // Enable
#define TIMER_CTL_TAEN          0x00000001  // GPTM TimerA Enable
#define TIMER_IMR_TATOIM        0x00000001  // GPTM TimerA Time-Out Interrupt
                                            // Mask
#define TIMER_TAILR_TAILRL_M    0x0000FFFF  // GPTM TimerA Interval Load
                                            // Register Low
#define ADC0_ACTSS_R            (*((volatile unsigned long *)0x40038000))
#define ADC0_RIS_R              (*((volatile unsigned long *)0x40038004))
#define ADC0_IM_R               (*((volatile unsigned long *)0x40038008))
#define ADC0_ISC_R              (*((volatile unsigned long *)0x4003800C))
#define ADC0_EMUX_R             (*((volatile unsigned long *)0x40038014))
#define ADC0_SSPRI_R            (*((volatile unsigned long *)0x40038020))
#define ADC0_PSSI_R             (*((volatile unsigned long *)0x40038028))
#define ADC0_SSMUX0_R           (*((volatile unsigned long *)0x40038040))
#define ADC0_SSCTL0_R           (*((volatile unsigned long *)0x40038044))
#define ADC0_SSFIFO0_R          (*((volatile unsigned long *)0x40038048))
#define ADC0_SSMUX1_R           (*((volatile unsigned long *)0x40038060))
#define ADC0_SSCTL1_R           (*((volatile unsigned long *)0x40038064))
#define ADC0_SSFIFO1_R          (*((volatile unsigned long *)0x40038068))
#define ADC0_SSMUX2_R           (*((volatile unsigned long *)0x40038080))
#define ADC0_SSCTL2_R           (*((volatile unsigned long *)0x40038084))
#define ADC0_SSFIFO2_R          (*((volatile unsigned long *)0x40038088))
#define ADC0_SSMUX3_R           (*((volatile unsigned long *)0x400380A0))
#define ADC0_SSCTL3_R           (*((volatile unsigned long *)0x400380A4))
#define ADC0_SSFIFO3_R          (*((volatile unsigned long *)0x400380A8))
#define ADC0_PC_R               (*((volatile unsigned long *)0x40038FC4))
#define ADC1_ACTSS_R            (*((volatile unsigned long *)0x40039000))
#define ADC1_RIS_R              (*((volatile unsigned long *)0x40039004))
#define ADC1_IM_R               (*((volatile unsigned long *)0x40039008))
#define ADC1_ISC_R              (*((volatile unsigned long *)0x4003900C))
#define ADC1_EMUX_R             (*((volatile unsigned long *)0x40039014))
#define ADC1_SSPRI_R            (*((volatile unsigned long *)0x40039020))
#define ADC1_PSSI_R             (*((volatile unsigned long *)0x40039028))
#define ADC1_SSMUX0_R           (*((volatile unsigned long *)0x40039040))
#define ADC1_SSCTL0_R           (*((volatile unsigned long *)0x40039044))
#define ADC1_SSFIFO0_R          (*((volatile unsigned long *)0x40039048))
#define ADC1_SSMUX1_R           (*((volatile unsigned long *)0x40039060))
#define ADC1_SSCTL1_R           (*((volatile unsigned long *)0x40039064))
#define ADC1_SSFIFO1_R          (*((volatile unsigned long *)0x40039068))
#define ADC1_SSMUX2_R           (*((volatile unsigned long *)0x40039080))
#define ADC1_SSCTL2_R           (*((volatile unsigned long *)0x40039084))
#define ADC1_SSFIFO2_R          (*((volatile unsigned long *)0x40039088))
#define ADC1_SSMUX3_R           (*((volatile unsigned long *)0x400390A0))
#define ADC1_SSCTL3_R           (*((volatile unsigned long *)0x400390A4))
#define ADC1_SSFIFO3_R          (*((volatile unsigned long *)0x400390A8))
#define ADC1_PC_R               (*((volatile unsigned long *)0x40039FC4))
#define ADC_RIS_INR3            0x00000008  // SS3 Raw Interrupt Status
#define ADC_IM_MASK3            0x00000008  // SS3 Interrupt Mask
#define ADC_ISC_IN3             0x00000008  // SS3 Interrupt Status and Clear
#define ADC_ISC_IN2             0x00000004  // SS2 Interrupt Status and Clear
#define ADC_ISC_IN1             0x00000002  // SS1 Interrupt Status and Clear
#define ADC_ISC_IN0             0x00000001  // SS0 Interrupt Status and Clear
#define ADC_EMUX_EM3_M          0x0000F000  // SS3 Trigger Select mask
#define ADC_EMUX_EM3_TIMER      0x00005000  // Timer
#define ADC_SSPRI_SS3_4TH       0x00003000  // fourth priority
#define ADC_SSPRI_SS2_3RD       0x00000200  // third priority
#define ADC_SSPRI_SS1_2ND       0x00000010  // second priority
#define ADC_SSPRI_SS0_1ST       0x00000000  // first priority
#define ADC_PSSI_SS3            0x00000008  // SS3 Initiate
#define ADC_SSMUX3_MUX0_M       0x00000003  // 1st Sample Input Select mask
#define ADC_SSMUX3_MUX0_S       0           // 1st Sample Input Select lshift
#define ADC_SSCTL3_TS0          0x00000008  // 1st Sample Temp Sensor Select
#define ADC_SSCTL3_IE0          0x00000004  // 1st Sample Interrupt Enable
#define ADC_SSCTL3_END0         0x00000002  // 1st Sample is End of Sequence
#define ADC_SSCTL3_D0           0x00000001  // 1st Sample Diff Input Select
#define ADC_SSFIFO_DATA_M       0x00000FFF  // Conversion Result Data mask
#define ADC_PC_SR_M             0x0000000F  // ADC Sample Rate
#define ADC_PC_SR_125K          0x00000001  // 125 ksps

#define SYSCTL_RCGCADC_R1       0x00000002  // ADC Module 1 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCADC_R0       0x00000001  // ADC Module 0 Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCADC_R        (*((volatile unsigned long *)0x400FE638))
#define SYSCTL_RCGC0_R          (*((volatile unsigned long *)0x400FE100))
#define SYSCTL_RCGC1_R          (*((volatile unsigned long *)0x400FE104))
#define SYSCTL_RCGC0_ADC0       0x00010000  // ADC0 Clock Gating Control
#define SYSCTL_RCGC0_ADCSPD_M   0x00000300  // ADC Sample Speed mask
#define SYSCTL_RCGC1_TIMER0     0x00010000  // timer 0 Clock Gating Control
#define SYSCTL_RCGCGPIO_R       (*((volatile unsigned long *)0x400FE608))
#define SYSCTL_RCGCGPIO_R4      0x00000010  // GPIO Port E Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R3      0x00000008  // GPIO Port D Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R1      0x00000002  // GPIO Port B Run Mode Clock
                                            // Gating Control
//ORIGINAL CODE
#define NUM_OF_USABLE_ADCS 8
#define ADC0SEQ0           0
#define ADC0SEQ1           1
#define ADC0SEQ2           2
#define ADC0SEQ3           3
#define ADC1SEQ0           4
#define ADC1SEQ1           5
#define ADC1SEQ2           6
#define ADC1SEQ3           7

#define ADC_TIMER_TRIGGERED  5
#define ADC_SW_TRIGGERED     0
#define ADC_CONT_TRIGGERED 0xF

#define BUSCLOCK    50000000		//50Mhz

//static int ADC_StatusFlag = ADCStatus_Uninit;// Indicates the ADCs current status.
volatile static int delay;                   // Placeholder variable for hardcoded delays
volatile static int ADC_TimerFrequency;
unsigned short ADC_SampleBuffer[100];

extern unsigned long AdcIn1, AdcIn2, AdcIn3, AdcIn4;

//------------------ADC_EnableChannel-------------------------
//Enables a specific channel/port of the ADC.
//Code in this method was obtained from ADT0ATrigger.c and ADCSWTrigger.c
//written by Daniel Valvano.
//Input: channelNum   number of channel to enable
void ADC_EnableChannel(int channelNum){
	switch(channelNum){             // 1) activate clock
    case 0:
    case 1:
    case 2:
    case 3:
    case 8:
    case 9:                       //    these are on GPIO_PORTE
      SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4; break;
    case 4:
    case 5:
    case 6:
    case 7:                       //    these are on GPIO_PORTD
      SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3; break;
    case 10:
    case 11:                      //    these are on GPIO_PORTB
      SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; break;
    default: return;              //    0 to 11 are valid channels on the LM4F120
  }
  delay = SYSCTL_RCGCGPIO_R;      // 2) allow time for clock to stabilize
  switch(channelNum){
    case 0:                       //      Ain0 is on PE3
      GPIO_PORTE_DIR_R &= ~0x08;  // 3.0) make PE3 input
      GPIO_PORTE_AFSEL_R |= 0x08; // 4.0) enable alternate function on PE3
      GPIO_PORTE_DEN_R &= ~0x08;  // 5.0) disable digital I/O on PE3
      GPIO_PORTE_AMSEL_R |= 0x08; // 6.0) enable analog functionality on PE3
      break;
    case 1:                       //      Ain1 is on PE2
      GPIO_PORTE_DIR_R &= ~0x04;  // 3.1) make PE2 input
      GPIO_PORTE_AFSEL_R |= 0x04; // 4.1) enable alternate function on PE2
      GPIO_PORTE_DEN_R &= ~0x04;  // 5.1) disable digital I/O on PE2
      GPIO_PORTE_AMSEL_R |= 0x04; // 6.1) enable analog functionality on PE2
      break;
    case 2:                       //      Ain2 is on PE1
      GPIO_PORTE_DIR_R &= ~0x02;  // 3.2) make PE1 input
      GPIO_PORTE_AFSEL_R |= 0x02; // 4.2) enable alternate function on PE1
      GPIO_PORTE_DEN_R &= ~0x02;  // 5.2) disable digital I/O on PE1
      GPIO_PORTE_AMSEL_R |= 0x02; // 6.2) enable analog functionality on PE1
      break;
    case 3:                       //      Ain3 is on PE0
      GPIO_PORTE_DIR_R &= ~0x01;  // 3.3) make PE0 input
      GPIO_PORTE_AFSEL_R |= 0x01; // 4.3) enable alternate function on PE0
      GPIO_PORTE_DEN_R &= ~0x01;  // 5.3) disable digital I/O on PE0
      GPIO_PORTE_AMSEL_R |= 0x01; // 6.3) enable analog functionality on PE0
      break;
    case 4:                       //      Ain4 is on PD3
      GPIO_PORTD_DIR_R &= ~0x08;  // 3.4) make PD3 input
      GPIO_PORTD_AFSEL_R |= 0x08; // 4.4) enable alternate function on PD3
      GPIO_PORTD_DEN_R &= ~0x08;  // 5.4) disable digital I/O on PD3
      GPIO_PORTD_AMSEL_R |= 0x08; // 6.4) enable analog functionality on PD3
      break;
    case 5:                       //      Ain5 is on PD2
      GPIO_PORTD_DIR_R &= ~0x04;  // 3.5) make PD2 input
      GPIO_PORTD_AFSEL_R |= 0x04; // 4.5) enable alternate function on PD2
      GPIO_PORTD_DEN_R &= ~0x04;  // 5.5) disable digital I/O on PD2
      GPIO_PORTD_AMSEL_R |= 0x04; // 6.5) enable analog functionality on PD2
      break;
    case 6:                       //      Ain6 is on PD1
      GPIO_PORTD_DIR_R &= ~0x02;  // 3.6) make PD1 input
      GPIO_PORTD_AFSEL_R |= 0x02; // 4.6) enable alternate function on PD1
      GPIO_PORTD_DEN_R &= ~0x02;  // 5.6) disable digital I/O on PD1
      GPIO_PORTD_AMSEL_R |= 0x02; // 6.6) enable analog functionality on PD1
      break;
    case 7:                       //      Ain7 is on PD0
      GPIO_PORTD_DIR_R &= ~0x01;  // 3.7) make PD0 input
      GPIO_PORTD_AFSEL_R |= 0x01; // 4.7) enable alternate function on PD0
      GPIO_PORTD_DEN_R &= ~0x01;  // 5.7) disable digital I/O on PD0
      GPIO_PORTD_AMSEL_R |= 0x01; // 6.7) enable analog functionality on PD0
      break;
    case 8:                       //      Ain8 is on PE5
      GPIO_PORTE_DIR_R &= ~0x20;  // 3.8) make PE5 input
      GPIO_PORTE_AFSEL_R |= 0x20; // 4.8) enable alternate function on PE5
      GPIO_PORTE_DEN_R &= ~0x20;  // 5.8) disable digital I/O on PE5
      GPIO_PORTE_AMSEL_R |= 0x20; // 6.8) enable analog functionality on PE5
      break;
    case 9:                       //      Ain9 is on PE4
      GPIO_PORTE_DIR_R &= ~0x10;  // 3.9) make PE4 input
      GPIO_PORTE_AFSEL_R |= 0x10; // 4.9) enable alternate function on PE4
      GPIO_PORTE_DEN_R &= ~0x10;  // 5.9) disable digital I/O on PE4
      GPIO_PORTE_AMSEL_R |= 0x10; // 6.9) enable analog functionality on PE4
      break;
    case 10:                      //       Ain10 is on PB4
      GPIO_PORTB_DIR_R &= ~0x10;  // 3.10) make PB4 input
      GPIO_PORTB_AFSEL_R |= 0x10; // 4.10) enable alternate function on PB4
      GPIO_PORTB_DEN_R &= ~0x10;  // 5.10) disable digital I/O on PB4
      GPIO_PORTB_AMSEL_R |= 0x10; // 6.10) enable analog functionality on PB4
      break;
    case 11:                      //       Ain11 is on PB5
      GPIO_PORTB_DIR_R &= ~0x20;  // 3.11) make PB5 input
      GPIO_PORTB_AFSEL_R |= 0x20; // 4.11) enable alternate function on PB5
      GPIO_PORTB_DEN_R &= ~0x20;  // 5.11) disable digital I/O on PB5
      GPIO_PORTB_AMSEL_R |= 0x20; // 6.11) enable analog functionality on PB5
      break;
  }
}

//------------------ADC_EnableModule-----------------
//Enable an ADC Module with hardoded sequencer priority
//and samples/sec. Will not reinitialize if already init
//Input:  module  module to enable, if necessary
//Output: 1 if enabled correctly
//        0 if init before
//       -1 if unsupported module number
int ADC_EnableModule(short module){
	switch(module){
		case 0:
			if ((SYSCTL_RCGCADC_R&0x00000001) == 0){
				SYSCTL_RCGCADC_R |= 0x00000001; //Activate ADC0
				delay = SYSCTL_RCGCADC_R;
				delay = SYSCTL_RCGCADC_R;
				ADC0_PC_R &= ~0xF;              // 8) clear max sample rate field
				ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
				ADC0_SSPRI_R = 0x3210;          // 9) Sequencer 3 is lowest priority
				return 1;
			}
			return 0;
		case 1:
			if ((SYSCTL_RCGCADC_R&0x00000002) == 0){
				SYSCTL_RCGCADC_R |= 0x00000002; //Activate ADC1
				delay = SYSCTL_RCGCADC_R;
				delay = SYSCTL_RCGCADC_R;
				ADC1_PC_R &= ~0xF;              // 8) clear max sample rate field
				ADC1_PC_R |= 0x1;               //    configure for 125K samples/sec
				ADC1_SSPRI_R = 0x3210;          // 9) Sequencer 3 is lowest priority
				return 1;
			}
			return 0;
		default:
			return -1;
	}
}

//-----------------ADC_EnableSequence----------------
//Enables the ports/ADC channels that are requested by the sequence.
void ADC_EnableSequence(int sequence, short seqLen){
	int i = 0;
	while(i++ < seqLen){
		ADC_EnableChannel(sequence&0x0000000F);
		sequence = sequence >> 4;
	}
}

//------------------ADC_EnableTimer------------------
//Enables Timer0A to be associated with the ADC to a frequency specified
//by the user. For the purpose of this lab it will most likely be 10 kHz.
//Input: ft Frequency desired for the timer.
void ADC_EnableTimer(int ft){
	// Sample time: busPeriod*(prescale+1)*(period+1)=1/fs
	//prescaler*period = busClock/ft
	unsigned long divide = BUSCLOCK/ft;
	unsigned long prescale = divide % 255;
	unsigned long period = (divide/prescale) - 1;
	prescale -= 1;
	
	ADC_TimerFrequency = ft;
	
	// **** timer0A initialization ****
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER0;     // activate timer0 (legacy code)
	delay = SYSCTL_RCGC1_R;
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN;           // disable timer0A during setup
  TIMER0_CTL_R |= TIMER_CTL_TAOTE;           // enable timer0A trigger to ADC
  TIMER0_CFG_R = TIMER_CFG_16_BIT;           // configure for 16-bit timer mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;    // configure for periodic mode, default down-count settings
  TIMER0_TAPR_R = prescale;                  // prescale value for trigger
  TIMER0_TAILR_R = period;                   // start value for trigger
  TIMER0_IMR_R &= ~TIMER_IMR_TATOIM;         // disable timeout (rollover) interrupt
  TIMER0_CTL_R |= TIMER_CTL_TAEN;            // enable timer0A 16-b, periodic, no interrupts
}

//----------------ADC_SetSequencer-------------------
//Sets a sample sequencer with the correct parameters
int ADC_SetSequencer(short module, short ssNum, int sequence, short seqLen, short mode){
	unsigned short SSMask = 0x0001 << ssNum;
	unsigned short muxShift = ssNum << 2;
	unsigned short ctlShift = (seqLen - 1) << 2;
	
	switch (module){
		case 0:
			ADC0_ACTSS_R &= ~SSMask;		          // disable sample sequencer
			ADC0_EMUX_R &= ~(0x000F << muxShift);       // clear SS3 trigger select field
			ADC0_EMUX_R += mode << muxShift;            // configure for timer trigger event
		
		  // set channels for sequencer
		  // IE and END enable for last sample, disable rest
			switch (ssNum){                            // configure for sequence to be polled
				case 0:                                  // settings for all samples:
					ADC0_SSMUX0_R = sequence;              // read pin specified by ADCn_SSMUXm_R
					ADC0_SSCTL0_R = 0x00000006 << ctlShift;// raw interrupt asserted after last
				case 1:                                  // last sample is end of sequence (hardwired)
					ADC0_SSMUX1_R = sequence;              // differential mode not used
					ADC0_SSCTL1_R = 0x0006 << ctlShift;
				case 2:
					ADC0_SSMUX2_R = sequence;
					ADC0_SSCTL2_R = 0x0006 << ctlShift;
				case 3:
					ADC0_SSMUX3_R = sequence;
					ADC0_SSCTL3_R = 0x6; //SS3 FIFO Size is 1. Don't bother with it.
					break;
			}
			
			if (mode == ADC_TIMER_TRIGGERED){
				ADC0_IM_R |= SSMask;
			}else{
				ADC0_IM_R &= ~SSMask;           // 14) disable sequence interrupts
			}
			ADC0_ACTSS_R |= SSMask;         // 15) enable sample sequencer
		  return 1;
		case 1:
			ADC1_ACTSS_R &= ~SSMask;		          // disable sample sequencer
			ADC1_EMUX_R &= ~(0x000F << muxShift);       // clear SS3 trigger select field
			ADC1_EMUX_R += mode << muxShift;            // configure for timer trigger event
		
		  // set channels for sequencer
		  // IE and END enable for last sample, disable rest
			switch (ssNum){                            // configure for sequence to be polled
				case 0:                                  // settings for all samples:
					ADC1_SSMUX0_R = sequence;              // read pin specified by ADCn_SSMUXm_R
					ADC1_SSCTL0_R = 0x00000006 << ctlShift;// raw interrupt asserted after last
				case 1:                                  // last sample is end of sequence (hardwired)
					ADC1_SSMUX1_R = sequence;              // differential mode not used
					ADC1_SSCTL1_R = 0x0006 << ctlShift;
				case 2:
					ADC1_SSMUX2_R = sequence;
					ADC1_SSCTL2_R = 0x0006 << ctlShift;
				case 3:
					ADC1_SSMUX3_R = sequence;
					ADC1_SSCTL3_R = 0x6; //SS3 FIFO Size is 1. Don't bother with it.
					break;
			}
			
			if (mode == ADC_TIMER_TRIGGERED){
				ADC1_IM_R |= SSMask;
			}else{
				ADC1_IM_R &= ~SSMask;           // 14) disable sequence interrupts
			}
			ADC1_ACTSS_R |= SSMask;         // 15) enable sample sequencer
		  return 1;
	}
	
	return -1;
}

//------------------ADC_Open-------------------------
//Configures a sampling sequence for sampling via software triggers.
//Call ADC_In to sample the channel.
//Input:  module      the ADC module to enable (0 or 1)
//        ssNum       the sample sequencer to configure (0 to 3)
//        sequence    sequence sample, given in hex, eg. 0x0030ABC1
//        seqLen      how many channels to sample (including duplicates)
//Output: -1 if open failed, 1 otherwise
int ADC_Open(short module, short ssNum, int sequence, short seqLen){
	ADC_EnableModule(module);
	ADC_EnableSequence(sequence, seqLen);
	return ADC_SetSequencer(module, ssNum, sequence, seqLen, ADC_SW_TRIGGERED);
}

//----------------------ADC_In-----------------------
//Samples the ADC once using software triggers and returns it
//Input:  module the module to smaple from
//        ssNum  the sample sequencer to sample from
//Output: ADC sample from the channel requested
int ADC_In(short module, short ssNum){//, unsigned int *outputBuffer){
	unsigned int result;
	unsigned short SSMask = 0x0001 << ssNum;
	
	switch (module){
		case 0:
			ADC0_PSSI_R = SSMask;                           // 1) initiate SS3
			while((ADC0_RIS_R&SSMask)==0){};                // 2) wait for conversion done
				
			switch (ssNum){                                 // 3) read result
				case 0:
					result =ADC0_SSFIFO0_R&ADC_SSFIFO_DATA_M;
				  break;
				case 1:
					result =ADC0_SSFIFO1_R&ADC_SSFIFO_DATA_M;
				  break;
				case 2:
					result =ADC0_SSFIFO2_R&ADC_SSFIFO_DATA_M;
				  break;
				case 3:
					result =ADC0_SSFIFO3_R&ADC_SSFIFO_DATA_M;
				  break;
			}
			
			ADC0_ISC_R = SSMask;                            // 4) acknowledge completion
		  return result;
		case 1:
			ADC1_PSSI_R = SSMask;                           // 1) initiate SS3
			while((ADC1_RIS_R&SSMask)==0){};                // 2) wait for conversion done
				
			switch (ssNum){                                 // 3) read result
				case 0:
					result =ADC1_SSFIFO0_R&ADC_SSFIFO_DATA_M;
				  break;
				case 1:
					result =ADC1_SSFIFO1_R&ADC_SSFIFO_DATA_M;
				  break;
				case 2:
					result =ADC1_SSFIFO2_R&ADC_SSFIFO_DATA_M;
				  break;
				case 3:
					result =ADC1_SSFIFO3_R&ADC_SSFIFO_DATA_M;
				  break;
			}
			
			ADC1_ISC_R = SSMask;                      // 4) acknowledge completion
		  return result;
		default:
			return -1;
	}
}

static void(*callbacks[NUM_OF_USABLE_ADCS])(unsigned long);
static long spinTarget[NUM_OF_USABLE_ADCS];
static long spinCurrent[NUM_OF_USABLE_ADCS];
//-------------------ADC_Collect----------------------
//Initiates a hardware triggered ADC sampling and stores the
//measured values in the buffer passed. Monitor the progress
//of this method using ADC_Status.
//Input: module          the ADC module to use (0 or 1)
//       ssNum           the sample sequencer to use (0 to 3)
//       sequence        the order to sample channels, e.g. 0x0023
//       seqLen          how many channels are being sampled
//       fs              sampling frequency (ft >= fs > 0)
//       buffer          array of where to store sampled values
//       numberOfSamples how many samples to take before workng on data
int ADC_Collect(short module, short ssNum, int sequence, short seqLen,
    unsigned int fs, void(*producerTask)(unsigned long)){
    //unsigned short buffer[], unsigned int numberOfSamples){

    int index = (module << 2) + ssNum;
    callbacks[index] = producerTask;
    spinTarget[index] = ADC_TimerFrequency / fs;
    spinCurrent[index] = 0;
	
	ADC_EnableModule(module);
	ADC_EnableSequence(sequence, seqLen);
	ADC_SetSequencer(module, ssNum, sequence, seqLen, ADC_TIMER_TRIGGERED);

	// **** interrupt initialization ****
	switch (module){
		case 0:
			switch (ssNum){
				case 0:
					NVIC_PRI3_R = (NVIC_PRI3_R&0xFF00FFFF)|0x00400000; // bits 13-15
				  NVIC_EN0_R |= NVIC_EN_INT14;                       // enable interrupt 14 in NVIC
				  break;
				case 1:
					NVIC_PRI3_R = (NVIC_PRI3_R&0x00FFFFFF)|0x40000000; // bits 13-15
				  NVIC_EN0_R |= NVIC_EN_INT15;                       // enable interrupt 17 in NVIC
				  break;
				case 2:
					NVIC_PRI4_R = (NVIC_PRI4_R&0xFFFFFF00)|0x00000040; // bits 13-15
				  NVIC_EN0_R |= NVIC_EN_INT16;                       // enable interrupt 17 in NVIC
				  break;
				case 3:
					NVIC_PRI4_R = (NVIC_PRI4_R&0xFFFF00FF)|0x00004000; // bits 13-15
				  NVIC_EN0_R |= NVIC_EN_INT17;                       // enable interrupt 17 in NVIC
				  break;
			}
			break;			// ADC3=priority 2
		case 1:
			switch (ssNum){
				case 0:
					NVIC_PRI12_R = (NVIC_PRI12_R&0xFFFFFF00)|0x00000040; // bits 13-15
				 	NVIC_EN1_R |= NVIC_EN_INT16;                       // enable interrupt 17 in NVIC
				 	break;
				case 1:
					NVIC_PRI12_R = (NVIC_PRI12_R&0xFFFF00FF)|0x00004000; // bits 13-15
				 	NVIC_EN1_R |= NVIC_EN_INT17;                       // enable interrupt 17 in NVIC
					break;
				case 2:
					NVIC_PRI12_R = (NVIC_PRI12_R&0xFF00FFFF)|0x00400000; // bits 13-15
				 	NVIC_EN1_R |= NVIC_EN_INT18;                       // enable interrupt 17 in NVIC
				 	break;
				case 3:
					NVIC_PRI12_R = (NVIC_PRI12_R&0x00FFFFFF)|0x04000000; // bits 13-15
				 	NVIC_EN1_R |= NVIC_EN_INT19;                       // enable interrupt 17 in NVIC
				 	break;
			}
			break;
		default:
			return -1;
	}
	
	return 0;
}

//---------------------ADC_Status----------------------
// Returns the current state of the ADC.
// Output: ADC status flag
int ADC_Status(int module, int ssNum){	
	int index = (module << 2) + ssNum;
//	return maxSamples[index] - numSamples[index];
	return (callbacks[index] != 0);
}

//--------------ADC Sequencer Handlers----------------------
//The below code are interrupts for all samplers on all ADCs
//They will all acknowledge the completion of their sample,
//store their sample in the appropriate buffer, and increment
//the correct num of samples.
//These handlers should stop executing once they reach the
//desired number of samples.
void ADC0Seq0_Handler(void){
  ADC0_ISC_R = ADC_ISC_IN0;                 // acknowledge ADC0 sequence 0 completion
  if (spinCurrent[ADC0SEQ0]++ < spinTarget[ADC0SEQ0])
    return;
	
  (*callbacks[ADC0SEQ0])(ADC0_SSFIFO0_R&ADC_SSFIFO_DATA_M);
  spinCurrent[ADC0SEQ0] = 0;
	AdcIn1 = (ADC0_SSFIFO0_R&ADC_SSFIFO_DATA_M);
}

void ADC0Seq1_Handler(void){
  ADC0_ISC_R = ADC_ISC_IN1;                 // acknowledge ADC0 sequence 1 completion
  
  if (spinCurrent[ADC0SEQ1]++ < spinTarget[ADC0SEQ1])
    return;

  (*callbacks[ADC0SEQ1])(ADC0_SSFIFO1_R&ADC_SSFIFO_DATA_M);
  spinCurrent[ADC0SEQ1] = 0;
	AdcIn2 = (ADC0_SSFIFO1_R&ADC_SSFIFO_DATA_M);
}

void ADC0Seq2_Handler(void){
  ADC0_ISC_R = ADC_ISC_IN2;                 // acknowledge ADC0 sequence 2 completion
  
  if (spinCurrent[ADC0SEQ2]++ < spinTarget[ADC0SEQ2])
    return;

  (*callbacks[ADC0SEQ2])(ADC0_SSFIFO2_R&ADC_SSFIFO_DATA_M);
  spinCurrent[ADC0SEQ2] = 0;
	AdcIn3 = (ADC0_SSFIFO2_R&ADC_SSFIFO_DATA_M);
}

void ADC0Seq3_Handler(void){
  ADC0_ISC_R = ADC_ISC_IN3;                 // acknowledge ADC0 sequence 3 completion
  
  
  if (spinCurrent[ADC0SEQ3]++ < spinTarget[ADC0SEQ3])
    return;

  (*callbacks[ADC0SEQ3])(ADC0_SSFIFO3_R&ADC_SSFIFO_DATA_M);
  spinCurrent[ADC0SEQ3] = 0;
	AdcIn4 = (ADC0_SSFIFO3_R&ADC_SSFIFO_DATA_M);
}

void ADC1Seq0_Handler(void){
  ADC1_ISC_R = ADC_ISC_IN0;                 // acknowledge ADC1 sequence 0 completion
  
  if (spinCurrent[ADC1SEQ0]++ < spinTarget[ADC1SEQ0])
    return;

  (*callbacks[ADC1SEQ0])(ADC1_SSFIFO0_R&ADC_SSFIFO_DATA_M);
  spinCurrent[ADC1SEQ0] = 0;
}

void ADC1Seq1_Handler(void){
  ADC1_ISC_R = ADC_ISC_IN1;                 // acknowledge ADC1 sequence 1 completion
  
  if (spinCurrent[ADC1SEQ1]++ < spinTarget[ADC1SEQ1])
    return;

  (*callbacks[ADC1SEQ1])(ADC1_SSFIFO1_R&ADC_SSFIFO_DATA_M);
  spinCurrent[ADC1SEQ1] = 0;
}

void ADC1Seq2_Handler(void){
  ADC1_ISC_R = ADC_ISC_IN2;                 // acknowledge ADC1 sequence 2 completion
  
  if (spinCurrent[ADC1SEQ2]++ < spinTarget[ADC1SEQ2])
    return;

  (*callbacks[ADC1SEQ2])(ADC1_SSFIFO2_R&ADC_SSFIFO_DATA_M);
  spinCurrent[ADC1SEQ2] = 0;
}

void ADC1Seq3_Handler(void){
  ADC1_ISC_R = ADC_ISC_IN3;                 // acknowledge ADC1 sequence 3 completion
  
  if (spinCurrent[ADC1SEQ3]++ < spinTarget[ADC1SEQ3])
    return;
	
  (*callbacks[ADC1SEQ3])(ADC1_SSFIFO3_R&ADC_SSFIFO_DATA_M);
  spinCurrent[ADC1SEQ3] = 0;
}
