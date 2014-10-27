
#include "SysTick.h"
#include "OS.h"

#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define PE3  (*((volatile unsigned long *)0x40024020))

#define GPIO_PORTF_DATA_R 			(*((volatile unsigned long *)0x400253FC))

#define NVIC_EN0_INT30          0x40000000  // Interrupt 30 enable
#define NVIC_EN0_R              (*((volatile unsigned long *)0xE000E100))  // IRQ 0 to 31 Set Enable Register
#define NVIC_PRI7_R             (*((volatile unsigned long *)0xE000E41C))  // IRQ 28 to 31 Priority Register
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_IS_R         (*((volatile unsigned long *)0x40025404))
#define GPIO_PORTF_IBE_R        (*((volatile unsigned long *)0x40025408))
#define GPIO_PORTF_IEV_R        (*((volatile unsigned long *)0x4002540C))
#define GPIO_PORTF_IM_R         (*((volatile unsigned long *)0x40025410))
#define GPIO_PORTF_ICR_R        (*((volatile unsigned long *)0x4002541C))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_PDR_R        (*((volatile unsigned long *)0x40025514))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control
	
#define GPIO_PORTF_LOCK 				(*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR 					(*((volatile unsigned long *)0x40025524))

long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void main0(void);

void(*SW1Task)(void);
void(*SW2Task)(void);

unsigned long PF0_Initialized = 0;
unsigned long PF4_Initialized = 0;

void PortE_Init(void){ unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x10;       // activate port E
  delay = SYSCTL_RCGC2_R;        
  delay = SYSCTL_RCGC2_R;         
  GPIO_PORTE_DIR_R |= 0x0F;    // make PE3-0 output heartbeats
  GPIO_PORTE_AFSEL_R &= ~0x0F;   // disable alt funct on PE3-0
  GPIO_PORTE_DEN_R |= 0x0F;     // enable digital I/O on PE3-0
  GPIO_PORTE_PCTL_R = ~0x0000FFFF;
  GPIO_PORTE_AMSEL_R &= ~0x0F;;      // disable analog functionality on PF
}

void SW1_Init(void(*task)(void), unsigned long priority){
  long status;
  status = StartCritical();
                                // (a) activate clock for port F
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;

  GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x10;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x10;     //     enable digital I/O on PF4
                                //     configure PF4 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFF0FFFF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4
  //NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
    NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00400000; // (g) priority 5
	NVIC_EN0_R = NVIC_EN0_INT30;  // (h) enable interrupt 30 in NVIC
  SW1Task = task;
  EndCritical(status);           // (i) Program 5.3
}


// ---
// SW1 is PF4
// SW2 is PF0
void SW_Init(void(*task)(void), unsigned long priority, unsigned long sw){
  long status;
	volatile long delay;
	unsigned long switchMask;
  status = StartCritical();

  if(sw == 1)
      switchMask = 0x10;  //PF4
   else if(sw == 2)
      switchMask = 0x01;  //PF0


                                  // (a) activate clock for port F
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;

	delay = SYSCTL_RCGC2_R;	
	GPIO_PORTF_LOCK = 0x4C4F434B;	//unlock PF0
	GPIO_PORTF_CR = 0x1F;
	 
  GPIO_PORTF_DIR_R &= ~switchMask;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~switchMask;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= switchMask;     //     enable digital I/O on PF4
                                //     configure PF4 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFF0FFFF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= switchMask;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~switchMask;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~switchMask;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~switchMask;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = switchMask;      // (e) clear flag4
  GPIO_PORTF_IM_R |= switchMask;      // (f) arm interrupt on PF4
  //NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  
  if(sw == 1){	//switch 1
    NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)| ((priority&0x7) << 21); 
    NVIC_EN0_R = NVIC_EN0_INT30;  // (h) enable interrupt 30 in NVIC
    SW1Task = task;
		PF4_Initialized = 1;
	}
  else if(sw == 2){		//switch 2
    NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)| ((priority&0x7) << 21); 
    NVIC_EN0_R = NVIC_EN0_INT30;  // (h) enable interrupt 30 in NVIC
    SW2Task = task;
		PF0_Initialized = 1;
	}
  
  EndCritical(status);           // (i) Program 5.3
}

//PF4 is switch 1
//PF0 is switch 2
// TODO: change this so it doesnt hold up the system in a handler
void GPIOPortF_Handler(void){
	static char lastPF0 = 1;
	static char lastPF4 = 1;	
	static char currentPF0 = 1;
	static char currentPF4 = 1;
//PE3 ^=0x8;
//PE3 ^=0x8;	
	
	if(PF4_Initialized == 1){	//disarm interrupt PF4
		GPIO_PORTF_IM_R &= ~0x10;   
		lastPF4 = GPIO_PORTF_DATA_R & 0x10;
	}
	if(PF0_Initialized == 1){	//disarm interrupt PF0
		GPIO_PORTF_IM_R &= ~0x01;   
		lastPF0 = GPIO_PORTF_DATA_R & 0x01;
	}
	
	SysTick_Wait1ms(10);
	
	if(PF4_Initialized == 1){	
		currentPF4 = GPIO_PORTF_DATA_R & 0x10;
		if((lastPF4 == currentPF4) && (currentPF4 == 0))
			(*SW1Task)();	
	}
	if(PF0_Initialized == 1){	
		currentPF0 = GPIO_PORTF_DATA_R & 0x01;
		if((lastPF0 == currentPF0) && (currentPF0 == 0))
			(*SW2Task)();
	}
	//PE3 ^=0x8;
	if(PF4_Initialized == 1){	//arm interrupt PF4
		GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
		GPIO_PORTF_IM_R |= 0x10;  
	}
	if(PF0_Initialized == 1){	//arm interrupt PF0
		GPIO_PORTF_ICR_R = 0x01;      // acknowledge flag4	
		GPIO_PORTF_IM_R |= 0x01;
	}
}

