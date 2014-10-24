// OS.c
// Runs on LM4F120
// Use Timer1 in 32-bit periodic mode to request interrupts at a particular period.
// Victor Vo and Alex Taft
// January 28, 2014

#include "PLL.h"
#include "OS.h"
#include "SysTick.h"
#include "GPIO.h"
#include "ADC.h"

// SysTick and PendSV definitions
#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))
#define NVIC_SYS_PRI3_R         (*((volatile unsigned long *)0xE000ED20))  // Sys. Handlers 12 to 15 Priority
#define NVIC_INT_CTRL_R         (*((volatile unsigned long *)0xE000ED04))
#define NVIC_SYS_PRI3_PENDSV_M	0x00E00000	//lowest priority (all bits set)

tcbType thread[NUMTHREADS]; // all threads that are still active
tcbType *RunPt;             // the running thread
tcbType *NextPt;

tcbList Actives;
tcbList Sleeping;

//====================================================================
//                    TCB PRIORITY LIST FUNCTIONS
//====================================================================

void linkTCB(tcbList* list, tcbPt target){
	tcbPt head = *list;
	tcbPt current;
	long status;
	
	status = StartCritical();
	
	if (head == 0){
		*list = target;
		target->next = target;
		target->prev = target;
	} else {
		current = head;
		
		if (target->priority < head->priority){
			*list = target;
		} else {
			current = current->next;
			while ((target->priority >= current->priority) && (current != head)){
				current = current->next;
			}
		}
		(current->prev)->next = target;
		target->prev = current->prev;  
		target->next = current;
		current->prev = target;
	}

	EndCritical(status);
}

int unlinkTCB(tcbList *list, tcbPt target){
	int headChanged = 0;
	tcbPt head = *list;
	long status;
	
	status = StartCritical();
	
	if (target == head){
		if (target == target->next){ //Notice, if target->next == target, 
			*list = 0;                 //then target->prev == target as well
		} else {
			*list = head->next;
		}
		
		headChanged = 1;
	}
	
	(target->prev)->next = target->next;
  (target->next)->prev = target->prev;
	
	EndCritical(status);
	return headChanged;
}

//====================================================================
//                    OS THREAD CONTEXT FUNCTIONS 
//====================================================================

// ----------------------------- OS_Id -------------------------------
// returns the thread ID for the currently running thread
// Inputs: none
// Outputs: Thread ID, number greater than zero 
unsigned long OS_Id(void){
	return RunPt->id;
}

// ----------------------- SetInitialStack ---------------------------
void SetInitialStack(unsigned long i, void(*task)(void)){
  thread[i].sp = &thread[i].stack[STACKSIZE - 16]; // thread stack pointer
  thread[i].stack[STACKSIZE - 1]  = 0x01000000; // thumb bit
  thread[i].stack[STACKSIZE - 2]  = (long)(task);  // PC
  thread[i].stack[STACKSIZE - 3]  = 0x14141414; // R14
  thread[i].stack[STACKSIZE - 4]  = 0x12121212; // R12
  thread[i].stack[STACKSIZE - 5]  = 0x03030303; // R3
  thread[i].stack[STACKSIZE - 6]  = 0x02020202; // R2
  thread[i].stack[STACKSIZE - 7]  = 0x01010101; // R1
  thread[i].stack[STACKSIZE - 8]  = 0x00000000; // R0
  thread[i].stack[STACKSIZE - 9]  = 0x11111111; // R11
  thread[i].stack[STACKSIZE - 10] = 0x10101010; // R10
  thread[i].stack[STACKSIZE - 11] = 0x09090909; // R9
  thread[i].stack[STACKSIZE - 12] = 0x08080808; // R8
  thread[i].stack[STACKSIZE - 13] = 0x07070707; // R7
  thread[i].stack[STACKSIZE - 14] = 0x06060606; // R6
  thread[i].stack[STACKSIZE - 15] = 0x05050505; // R5
  thread[i].stack[STACKSIZE - 16] = 0x04040404; // R4
}

// -------------------------- OS_AddThread ---------------------------
int OS_AddThread(void(*task)(void), unsigned long stackSize, unsigned long priority){
  long status;
	int i = 0;
	
  status = StartCritical();
  
	while((thread[i].id != 0) && (i < NUMTHREADS)){
		i++;
	};
	
	if (thread[i].id != 0){
		EndCritical(status);
		return 0;
	}
	
	SetInitialStack(i, task);
	thread[i].id = i + 1;
	thread[i].priority = priority;
	thread[i].blockSt = 0;
	thread[i].sleepSt = 0;
	
	linkTCB(&Actives, &thread[i]);
	
  EndCritical(status);
  return 1;   //successfully added thread
}

// ------------------------ OS_AddSW1Task ----------------------------
int OS_AddSW1Task(void(*task)(void), unsigned long priority){
	SW1_Init(task, priority);
	return 1;
}


int OS_AddSWTask(void(*task)(void), unsigned long priority, unsigned long sw){
	SW_Init(task, priority, sw);
	return 1;
}



#define PERIODIC_TASK_NUM 10
#define TIMER1_ICR_R            (*((volatile unsigned long *)0x40031024))
#define TIMER_ICR_TATOCINT      0x00000001  // GPTM TimerA Time-Out Raw Interrupt

void(*periodicTasks[PERIODIC_TASK_NUM])(void);
unsigned long periodicCurrentSpin[PERIODIC_TASK_NUM];
unsigned long periodicTriggerSpin[PERIODIC_TASK_NUM];
void (*PeriodicTask1)(void);  // user function

unsigned long OS_NumPeriodicThreads = 0;

//------------------------ Timer1A_Handler ---------------------------
//Periodic interrupt handler for Timer1
void Timer1A_Handler(void){
	int i = 0;
	
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer1A timeout
  
  for(i = 0; i < OS_NumPeriodicThreads; i++){
    if(periodicCurrentSpin[i] >= periodicTriggerSpin[i]){
      (*periodicTasks[i])();
      periodicCurrentSpin[i] = 0;
    } else {
      periodicCurrentSpin[i]++;
    }   
  }
}

// ----------------------- OS_AddPeriodicThread ----------------------
// Perioid is units of 500us
int OS_AddPeriodicThread(void(*task)(void), unsigned long period,
  unsigned long priority){

	if(OS_NumPeriodicThreads < PERIODIC_TASK_NUM){
		periodicTasks[OS_NumPeriodicThreads] = task;
		periodicTriggerSpin[OS_NumPeriodicThreads] = period-1;
		periodicCurrentSpin[OS_NumPeriodicThreads] = 0;
		OS_NumPeriodicThreads++;  
		return 1;
	}
	return 0;
}

// -------------------------- OS_Kill --------------------------------
void OS_Kill(void){
  long status;
	status = StartCritical();
  
	unlinkTCB(&Actives, RunPt);
  RunPt->id = 0;

  EndCritical(status);
  OS_Suspend();
}

// ----------------------- OS_InitSemaphore --------------------------
void OS_InitSemaphore(Sema4Type *semaPt, long value){
  long status; 
  status = StartCritical();
  (*semaPt).Value = value;
	semaPt->Blocked = 0;
  EndCritical(status);
}

// -------------------------- OS_Wait --------------------------------
void OS_Wait(Sema4Type *semaPt){
	DisableInterrupts();
	
	while (semaPt->Value <= 0){
		unlinkTCB(&Actives, RunPt);
		linkTCB(&(semaPt->Blocked), RunPt);
		RunPt->blockSt = semaPt;
		OS_Suspend();
		EnableInterrupts();
	}
	DisableInterrupts();
	(*semaPt).Value = (*semaPt).Value - 1;
	EnableInterrupts();
}
 
// ------------------------- OS_bWait --------------------------------
void OS_bWait(Sema4Type *semaPt){
  DisableInterrupts();
	
	while (semaPt->Value <= 0){
		unlinkTCB(&Actives, RunPt);
		linkTCB(&(semaPt->Blocked), RunPt);
		RunPt->blockSt = semaPt;
		OS_Suspend();
		EnableInterrupts();
	}
	DisableInterrupts();
	(*semaPt).Value = 0;
	EnableInterrupts();
}

// ------------------------- OS_Signal -------------------------------
void OS_Signal(Sema4Type *semaPt){
	long status;
	tcbPt blocked;
	
	status = StartCritical();
	
	(*semaPt).Value = (*semaPt).Value + 1;
	blocked = semaPt->Blocked;
	
	if (blocked != 0){
		unlinkTCB(&(semaPt->Blocked), blocked);
		linkTCB(&Actives, blocked);
		blocked->blockSt = 0;
	}
	
	EndCritical(status);
}

// ------------------------- OS_bSignal ------------------------------
void OS_bSignal(Sema4Type *semaPt){
	long status;
	tcbPt blocked;
	
	status = StartCritical();
	
	(*semaPt).Value = 1;
	blocked = semaPt->Blocked;
	
	if (blocked != 0){
		unlinkTCB(&(semaPt->Blocked), blocked);
		linkTCB(&Actives, blocked);
		blocked->blockSt = 0;
	}
	
	EndCritical(status);
}

/*
// ------------------------ OS_AddSW1Task ----------------------------
int OS_AddSW1Task(void(*task)(void), unsigned long priority){
	SW1_Init(task, priority);
	return 1;
}
*/


unsigned long SleepingThreadNum = 0;
tcbType* SleepingThreads[NUMTHREADS];
// --------------------------- OS_Sleep ------------------------------
void OS_Sleep(unsigned long sleepTime){
	DisableInterrupts();
	
	RunPt->sleepSt = sleepTime;
	unlinkTCB(&Actives, RunPt);
	SleepingThreads[SleepingThreadNum] = RunPt; 
	SleepingThreadNum++;
	
	OS_Suspend();
  EnableInterrupts();
}

// -------- DecrementSleep --------------------------
void DecrementSleep(void){
	int i = 0;
	int j = 0;
	
	for(i = 0; i < SleepingThreadNum; i++){
		SleepingThreads[i]->sleepSt--;
		if (SleepingThreads[i]->sleepSt <= 0){
			//Wake up
			linkTCB(&Actives, SleepingThreads[i]);
			
			for (j = i; j < SleepingThreadNum; j++){
				SleepingThreads[j] = SleepingThreads[j + 1];
			}
			
			SleepingThreadNum--;
			i--;
		}		
	}
}


// -------------------------- OS_Suspend -----------------------------
// A thread calls this to switch threads manually
// before the periodic interrupt switches threads
// inputs:  none
// outputs: none
void OS_Suspend(void){
	NVIC_ST_CURRENT_R = 0;        // clear counter
	NVIC_INT_CTRL_R = 0x04000000; // trigger SysTick
//  NVIC_INT_CTRL_R = 0x10000000; // trigger pendSV
}


// --------------------------- OS_Init -------------------------------
void OS_Init(void){
  DisableInterrupts();
  PLL_Init(); // 50 Mhz
  
	NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_PENDSV_M; // pend sv priority
  OS_InitSystemTime();
  OS_InitPeriodicTimer(TIME_500US, 1);
	OS_Display_Init();
	OS_Interpreter_Init();
  
	ADC_EnableTimer(20000); // ADC timer running at 10000 MHz

  RunPt = 0;
	Actives = 0;
	Sleeping = 0;
}

// --------------------------- NoCrash -------------------------------
void NoCrash(void){
	for(;;){}
}

// --------------------------- OS_Launch -----------------------------
void OS_Launch(unsigned long timeSlice){
	SysTick_InitInt(timeSlice);                     //For preemptive context switching
	OS_AddPeriodicThread(&RunTimeUpdate, 2, 128);  
	OS_AddPeriodicThread(&DecrementSleep, 2, 1);
  OS_AddThread(&NoCrash,128,7); //to guarantee there is always a thread in the actives list	
  OS_AddThread(&OS_Display,128,1);          
	OS_AddThread(&OS_ProcessInterpreter,128,3);
	
	RunPt = Actives;
	StartOS(); 
  EnableInterrupts();
}

// ---------------------- SysTick_Handler ------------------------
// Handles interrupts periodically. Period as defined by "period"
// in SysTick_InitInt.
// Input:   none	
// Output:  none
void SysTick_Handler(void){

	if ((RunPt->blockSt != 0) ||
			(Actives->priority < RunPt->priority) || 
			((RunPt->next)->priority > RunPt->priority)){ 
		NextPt = Actives;
	} else {
		NextPt = RunPt->next;
	}
	NVIC_INT_CTRL_R = 0x10000000; // trigger pendSV
	
}
