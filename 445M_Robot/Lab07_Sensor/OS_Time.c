#include "OS.h"

// gerneral timer definitions
#define TIMER_CFG_32_BIT_TIMER  0x00000000  // 32-bit timer configuration
#define TIMER_TAMR_TACDIR       0x00000010  // GPTM Timer A Count Direction
#define TIMER_TAMR_TAMR_PERIOD  0x00000002  // Periodic Timer mode
#define TIMER_CTL_TAEN          0x00000001  // GPTM TimerA Enable
#define TIMER_IMR_TATOIM        0x00000001  // GPTM TimerA Time-Out Interrupt Mask        
#define TIMER_ICR_TATOCINT      0x00000001  // GPTM TimerA Time-Out Raw Interrupt
#define TIMER_TAILR_M           0xFFFFFFFF  // GPTM Timer A Interval Load Register                     
#define SYSCTL_RCGC1_R          (*((volatile unsigned long *)0x400FE104))
#define SYSCTL_RCGC1_TIMER1     0x00020000  // timer 1 Clock Gating Control
#define SYSCTL_RCGC1_TIMER2     0x00040000  // timer 1 Clock Gating Control

// timer 1 definitions
#define NVIC_EN0_INT21          0x00200000  // Interrupt 21 enable
#define NVIC_EN0_R              (*((volatile unsigned long *)0xE000E100))  // IRQ 0 to 31 Set Enable Register
#define NVIC_PRI5_R             (*((volatile unsigned long *)0xE000E414))  // IRQ 20 to 23 Priority Register
#define TIMER1_CFG_R            (*((volatile unsigned long *)0x40031000))
#define TIMER1_TAMR_R           (*((volatile unsigned long *)0x40031004))
#define TIMER1_CTL_R            (*((volatile unsigned long *)0x4003100C))
#define TIMER1_IMR_R            (*((volatile unsigned long *)0x40031018))
#define TIMER1_MIS_R            (*((volatile unsigned long *)0x40031020))
#define TIMER1_ICR_R            (*((volatile unsigned long *)0x40031024))
#define TIMER1_TAILR_R          (*((volatile unsigned long *)0x40031028))
#define TIMER1_TAPR_R           (*((volatile unsigned long *)0x40031038))
#define TIMER1_TAR_R            (*((volatile unsigned long *)0x40031048))

// timer 2 definitions
#define NVIC_EN0_INT23          0x00800000  // Interrupt 23 enable
#define TIMER2_CFG_R            (*((volatile unsigned long *)0x40032000))
#define TIMER2_TAMR_R           (*((volatile unsigned long *)0x40032004))
#define TIMER2_CTL_R            (*((volatile unsigned long *)0x4003200C))
#define TIMER2_IMR_R            (*((volatile unsigned long *)0x40032018))
#define TIMER2_MIS_R            (*((volatile unsigned long *)0x40032020))
#define TIMER2_ICR_R            (*((volatile unsigned long *)0x40032024))
#define TIMER2_TAILR_R          (*((volatile unsigned long *)0x40032028))
#define TIMER2_TAPR_R           (*((volatile unsigned long *)0x40032038))
#define TIMER2_TAR_R            (*((volatile unsigned long *)0x40032048))
#define TIMER2_TAV_R            (*((volatile unsigned long *)0x40032050))


unsigned long OS_RunTime = 0;	//global that increments every second by the Timer1A handler
unsigned long OS_Milliseconds = 0;
unsigned long OS_MSSpins = 0;
unsigned long OS_MSSpinTarget = 0;

//====================================================================
//                        OS TIME FUNCTIONS 
//====================================================================

// ---------------------------- OS_Time ------------------------------
// return the system time 
// Inputs:  none
// Outputs: time in 12.5ns units, 0 to 4294967295
// The time resolution should be less than or equal to 1us, and the precision 32 bits
// It is ok to change the resolution and precision of this function as long as 
//   this function and OS_TimeDifference have the same resolution and precision 
unsigned long OS_Time(void){
	return TIMER2_TAV_R;
}

// ----------------------- OS_TimeDifference -------------------------
// Calculates difference between two times
// Inputs:  two times measured with OS_Time
// Outputs: time difference in 12.5ns units 
// The time resolution should be less than or equal to 1us, and the precision at least 12 bits
// It is ok to change the resolution and precision of this function as long as 
//   this function and OS_Time have the same resolution and precision 
unsigned long OS_TimeDifference(unsigned long start, unsigned long stop){
		return stop - start;
	
}

// ------------------------ OS_ClearMsTime ---------------------------
// sets the system time to zero (from Lab 1)
// Inputs:  none
// Outputs: none
// You are free to change how this works
void OS_ClearMsTime(void){
  long status;

  status = StartCritical();
	OS_Milliseconds = 0;
  EndCritical(status);
}

// ------------------------- OS_MsTime -------------------------------
// reads the current time in msec (from Lab 1)
// Inputs:  none
// Outputs: time in ms units
// You are free to select the time resolution for this function
// It is ok to make the resolution to match the first call to OS_AddPeriodicThread
unsigned long OS_MsTime(void){
   long status, time;
  
  status = StartCritical();
  time = OS_Milliseconds;
  EndCritical(status);
	
  return time; 
}

// -------------------- OS_ClearPeriodicTime -------------------------
// Clears the global running time off the program
// Inputs:  none
// Outputs: OS_RunTime is set to 0
void OS_ClearPeriodicTime(void){
  long status;
  
  status = StartCritical();
  OS_RunTime = 0;
  EndCritical(status);

}


// --------------------- OS_ReadPeridoicTime -------------------------
// Return the systems running time (in seconds)
// Inputs:   none
// Outputs:  current time (seconds)
unsigned long OS_ReadPeriodicTime(void){
  long status, time;
  
  status = StartCritical();
  time = OS_RunTime;
  EndCritical(status);

  return time;
}


// ----------------------- RunTimeUpdate -----------------------------
// Increments the global run time variable once a second
// Input:   none
// Ouptut:  none
void RunTimeUpdate(void){
  OS_Milliseconds++;
	OS_RunTime = OS_Milliseconds / 1000;
 
	OS_Milliseconds = OS_Milliseconds%0xFFFFFFFE; 
}

// --------------------- OS_InitSystemTime ---------------------------
void OS_InitSystemTime(void){
  long status = StartCritical();
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER2;   // 0) activate timer2
  TIMER2_CTL_R &= ~TIMER_CTL_TAEN;         // 1) disable timer2A during setup
  TIMER2_CFG_R = TIMER_CFG_32_BIT_TIMER;   // 2) configure for 32-bit timer mode
  TIMER2_TAMR_R = TIMER_TAMR_TAMR_PERIOD | // 3) periodic, up-count
          TIMER_TAMR_TACDIR;
  TIMER2_TAILR_R = 0xFFFFFFFF;             // 4) reload value
  TIMER2_CTL_R |= TIMER_CTL_TAEN;          // 6) enable timer2A
  EndCritical(status);
}


// -------------------- OS_InitPeriodicTimer -------------------------
void OS_InitPeriodicTimer(unsigned long period, unsigned long priority){
  long sr;
  sr = StartCritical(); 
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER1; // 0) activate timer1
  //PeriodicTask1 = task;             // user function
  TIMER1_CTL_R &= ~TIMER_CTL_TAEN; // 1) disable timer0A during setup
                                   // 2) configure for 32-bit timer mode
  TIMER1_CFG_R = TIMER_CFG_32_BIT_TIMER;
                                   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER1_TAILR_R = period-1;       // 4) reload value
                                   // 5) clear timer0A timeout flag
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;
  TIMER1_IMR_R |= TIMER_IMR_TATOIM;// 6) arm timeout interrupt
  //NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00004000; // 7) priority 2
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)| (priority << 13);
  NVIC_EN0_R = NVIC_EN0_INT21;     // 8) enable interrupt 21 in NVIC
  TIMER1_CTL_R |= TIMER_CTL_TAEN;  // 9) enable timer0A
  
  EndCritical(sr);
}
