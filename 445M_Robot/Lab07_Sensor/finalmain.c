#include "OS.h"
#include "GPIO.h"
#include "ADC.h"
#include "main.h"


#define PE0  (*((volatile unsigned long *)0x40024004))
#define PE1  (*((volatile unsigned long *)0x40024008))
#define PE2  (*((volatile unsigned long *)0x40024010))
#define PE3  (*((volatile unsigned long *)0x40024020))

#define PERIOD TIME_500US // DAS 2kHz sampling period in system time units
//#define DEBUG 0

//*********Prototype for FFT in cr4_fft_64_stm32.s, STMicroelectronics
void cr4_fft_64_stm32(void *pssOUT, void *pssIN, unsigned short Nbin);
//*********Prototype for PID in PID_stm32.s, STMicroelectronics
short PID_stm32(short Error, short *Coeff);

unsigned long NumCreated;   // number of foreground threads created
unsigned long PIDWork;      // current number of PID calculations finished
unsigned long FilterWork;   // number of digital filter calculations finished
unsigned long FilterWork2;   // number of digital filter calculations finished
unsigned long NumSamples;   // incremented every ADC sample, in Producer
#define FS 400            // producer/consumer sampling
#define RUNLENGTH (20*FS) // display results and quit when NumSamples==RUNLENGTH
long x[64],y[64];         // input and output arrays for FFT

//---------------------User debugging-----------------------
unsigned long DataLost;     // data sent by Producer, but not received by Consumer
long MaxJitter;             // largest time jitter between interrupts in usec
long MaxJitter2;             // largest time jitter between interrupts in usec
#define JITTERSIZE 64
unsigned long const JitterSize=JITTERSIZE;
unsigned long JitterHistogram[JITTERSIZE]={0,};
unsigned long JitterHistogram2[JITTERSIZE]={0,};


//------------------Task 1--------------------------------
// 2 kHz sampling ADC channel 1, using software start trigger
// background thread executed at 2 kHz
// 60-Hz notch high-Q, IIR filter, assuming fs=2000 Hz
// y(n) = (256x(n) -503x(n-1) + 256x(n-2) + 498y(n-1)-251y(n-2))/256 (2k sampling)
// y(n) = (256x(n) -476x(n-1) + 256x(n-2) + 471y(n-1)-251y(n-2))/256 (1k sampling)
long Filter(long data){
static long x[6]; // this MACQ needs twice
static long y[6];
static unsigned long n=3;   // 3, 4, or 5
  n++;
  if(n==6) n=3;     
  x[n] = x[n-3] = data;  // two copies of new data
  y[n] = (256*(x[n]+x[n-2])-503*x[n-1]+498*y[n-1]-251*y[n-2]+128)/256;
  y[n-3] = y[n];         // two copies of filter outputs too
  return y[n];
} 
//******** DAS *************** 
// background thread, calculates 60Hz notch filter
// runs 2000 times/sec
// samples channel 4, PD3,
// inputs:  none
// outputs: none
unsigned long DASoutput;
void DAS(void){ 
unsigned long input;  
unsigned static long LastTime;  // time at previous ADC sample
unsigned long thisTime;         // time at current ADC sample
long jitter;                    // time between measured and expected, in us 
	if(NumSamples < RUNLENGTH){   // finite time run
		#ifdef DEBUG
		PE0 ^= 0x01;
		#endif
    input = ADC_In(0,3);           // channel set when calling ADC_Init
		
		#ifdef DEBUG
		PE0 ^= 0x01;
		#endif
    thisTime = OS_Time();       // current time, 12.5 ns
    DASoutput = Filter(input);
    FilterWork++;        // calculation finished
    
		if(FilterWork>1){    // ignore timing of first interrupt
      unsigned long diff = OS_TimeDifference(LastTime,thisTime);
      if(diff>PERIOD){
        jitter = (diff-PERIOD+4)/8;  // in 0.1 usec
      }else{
        jitter = (PERIOD-diff+4)/8;  // in 0.1 usec
      }
      if(jitter > MaxJitter){
        MaxJitter = jitter; // in usec
      }       // jitter should be 0
      if(jitter >= JitterSize){
        jitter = JITTERSIZE-1;
      }
      JitterHistogram[jitter]++; 
    }
    LastTime = thisTime;
		
		#ifdef DEBUG
    PE0 ^= 0x01;
		#endif
  }
}
	
//******** DAS2 *************** 
// background thread, calculates 60Hz notch filter
// runs 2000 times/sec
// samples channel 4, PD3,
// inputs:  none
// outputs: none
unsigned long DASoutput2;
void DAS2(void){ 
unsigned long input2;  
unsigned static long LastTime2;  // time at previous ADC sample
unsigned long thisTime2;         // time at current ADC sample
long jitter2;                    // time between measured and expected, in us 
	if(NumSamples < RUNLENGTH){   // finite time run
		#ifdef DEBUG
		PE0 ^= 0x01;
		#endif
    input2 = ADC_In(1,3);           // channel set when calling ADC_Init
		
		#ifdef DEBUG
		PE0 ^= 0x01;
		#endif
    thisTime2 = OS_Time();       // current time, 12.5 ns
    DASoutput2 = Filter(input2);
    FilterWork2++;        // calculation finished
    
		if(FilterWork2>1){    // ignore timing of first interrupt
      unsigned long diff = OS_TimeDifference(LastTime2,thisTime2);
      if(diff>PERIOD){
        jitter2 = (diff-PERIOD+4)/8;  // in 0.1 usec
      }else{
        jitter2 = (PERIOD-diff+4)/8;  // in 0.1 usec
      }
      if(jitter2 > MaxJitter){
        MaxJitter2 = jitter2; // in usec
      }       // jitter should be 0
      if(jitter2 >= JitterSize){
        jitter2 = JITTERSIZE-1;
      }
      JitterHistogram2[jitter2]++; 
    }
    LastTime2 = thisTime2;
		
		#ifdef DEBUG
    PE0 ^= 0x01;
		#endif
  }
	
}
//--------------end of Task 1-----------------------------

//------------------Task 2--------------------------------
// background thread executes with SW1 button
// one foreground task created with button push
// foreground treads run for 2 sec and die
// ***********ButtonWork*************
void ButtonWork(void){
unsigned long myId = OS_Id(); 
	#ifdef DEBUG
	PE1 ^= 0x02;
	#endif
	OS_DisplayMessage(1,0,"NumCreated =",NumCreated); 
  #ifdef DEBUG
	PE1 ^= 0x02;
	#endif
  OS_Sleep(50);     // set this to sleep for 50msec
  OS_DisplayMessage(1,1,"PIDWork      =",PIDWork);
  OS_DisplayMessage(1,2,"DataLost     =",DataLost);
  OS_DisplayMessage(1,3,"Jitter 0.1us =",MaxJitter);
	OS_DisplayMessage(1,4,"Jitter2 0.1us=",MaxJitter2);
	#ifdef DEBUG
	PE1 ^= 0x02;
	#endif
  OS_Kill();  // done, OS does not return from a Kill
} 

//************SW1Push*************
// Called when SW1 Button pushed
// Adds another foreground task
// background threads execute once and return
void SW1Push(void){
//  if(OS_MsTime() > 20){ // debounce
    if(OS_AddThread(&ButtonWork,100,2)){
      NumCreated++; 
    }
   // OS_ClearMsTime();  // at least 20ms between touches
  //}
}
//************SW2Push*************
// Called when SW2 Button pushed, Lab 3 only
// Adds another foreground task
// background threads execute once and return
void SW2Push(void){
  if(OS_MsTime() > 20){ // debounce
    if(OS_AddThread(&ButtonWork,100,2)){
      NumCreated++; 
    }
   // OS_ClearMsTime();  // at least 20ms between touches
  }
}
//--------------end of Task 2-----------------------------

//------------------Task 3--------------------------------
// hardware timer-triggered ADC sampling at 400Hz
// Producer runs as part of ADC ISR
// Producer uses fifo to transmit 400 samples/sec to Consumer
// every 64 samples, Consumer calculates FFT
// every 2.5ms*64 = 160 ms (6.25 Hz), consumer sends data to Display via mailbox
// Display thread updates LCD with measurement

//******** Producer *************** 
// The Producer in this lab will be called from your ADC ISR
// A timer runs at 400Hz, started by your ADC_Collect
// The timer triggers the ADC, creating the 400Hz sampling
// Your ADC ISR runs when ADC data is ready
// Your ADC ISR calls this function with a 12-bit sample 
// sends data to the consumer, runs periodically at 400Hz
// inputs:  none
// outputs: none
void Producer(unsigned long data){  
  if(NumSamples < RUNLENGTH){   // finite time run
    NumSamples++;               // number of samples
    if(OS_Fifo_Put(data) == 0){ // send to consumer
      DataLost++;
    } 
  } 
}
void Display(void); 

//******** Consumer *************** 
// foreground thread, accepts data from producer
// calculates FFT, sends DC component to Display
// inputs:  none
// outputs: none
void Consumer(void){ 
unsigned long data,DCcomponent;   // 12-bit raw ADC sample, 0 to 4095
unsigned long t;                  // time in 2.5 ms
unsigned long myId = OS_Id(); 
  ADC_Collect(0, 2, 0x5, 1, FS, &Producer); // start ADC sampling, channel 5, PD2, 400 Hz
  NumCreated += OS_AddThread(&Display,128,1); 
  while(NumSamples < RUNLENGTH) { 
		#ifdef DEBUG
		PE2 = 0x04;
		#endif
    for(t = 0; t < 64; t++){   // collect 64 ADC samples
      data = OS_Fifo_Get();    // get from producer
      x[t] = data;             // real part is 0 to 4095, imaginary part is 0
    }
		#ifdef DEBUG
    PE2 = 0x00;
		#endif
    cr4_fft_64_stm32(y,x,64);  // complex FFT of last 64 ADC values
    DCcomponent = y[0]&0xFFFF; // Real part at frequency 0, imaginary part should be zero
    OS_MailBox_Send(DCcomponent); // called every 2.5ms*64 = 160ms
  }
  OS_Kill();  // done
}
//******** Display *************** 
// foreground thread, accepts data from consumer
// displays calculated results on the LCD
// inputs:  none                            
// outputs: none
void Display(void){ 
unsigned long data,voltage;
  OS_DisplayMessage(0,1,"Run length = ",(RUNLENGTH)/FS);   // top half used for Display
  while(NumSamples < RUNLENGTH) { 
    data = OS_MailBox_Recv();
    voltage = 3000*data/4095;               // calibrate your device so voltage is in mV
    #ifdef DEBUG
		PE3 = 0x08;
		#endif
    OS_DisplayMessage(0,2,"v(mV) =",voltage); 
		OS_Suspend();
		#ifdef DEBUG
    PE3 = 0x00;
		#endif
  } 
  OS_Kill();  // done
} 

//--------------end of Task 3-----------------------------

//------------------Task 4--------------------------------
// foreground thread that runs without waiting or sleeping
// it executes a digital controller 
//******** PID *************** 
// foreground thread, runs a PID controller
// never blocks, never sleeps, never dies
// inputs:  none
// outputs: none
short IntTerm;     // accumulated error, RPM-sec
short PrevError;   // previous error, RPM
short Coeff[3];    // PID coefficients
short Actuator;
void PID(void){ 
short err;  // speed error, range -100 to 100 RPM
unsigned long myId = OS_Id(); 
  PIDWork = 0;
  IntTerm = 0;
  PrevError = 0;
  Coeff[0] = 384;   // 1.5 = 384/256 proportional coefficient
  Coeff[1] = 128;   // 0.5 = 128/256 integral coefficient
  Coeff[2] = 64;    // 0.25 = 64/256 derivative coefficient*
  while(NumSamples < RUNLENGTH) { 
    for(err = -1000; err <= 1000; err++){    // made-up data
      Actuator = PID_stm32(err,Coeff)/256;
    }
    PIDWork++;        // calculation finished
  }
  for(;;){ }          // done
}
//--------------end of Task 4-----------------------------

//------------------Task 5--------------------------------
// UART background ISR performs serial input/output
// Two software fifos are used to pass I/O data to foreground
// The interpreter runs as a foreground thread
// The UART driver should call OS_Wait(&RxDataAvailable) when foreground tries to receive
// The UART ISR should call OS_Signal(&RxDataAvailable) when it receives data from Rx
// Similarly, the transmit channel waits on a semaphore in the foreground
// and the UART ISR signals this semaphore (TxRoomLeft) when getting data from fifo
// Modify your intepreter from Lab 1, adding commands to help debug 
// Interpreter is a foreground thread, accepts input from serial port, outputs to serial port
// inputs:  none
// outputs: none
//void Interpreter(void);    // just a prototype, link to your interpreter
// add the following commands, leave other commands, if they make sense
// 1) print performance measures 
//    time-jitter, number of data points lost, number of calculations performed
//    i.e., NumSamples, NumCreated, MaxJitter, DataLost, FilterWork, PIDwork
      
// 2) print debugging parameters 
//    i.e., x[], y[] 

void Interpreter(void){
	for(;;){
   OS_ProcessInterpreter(1);  
	 //OS_DisplayInterpreter();
	// OS_Suspend();
	}
	
}//
//--------------end of Task 5-----------------------------


//*******************final user main DEMONTRATE THIS TO TA**********
int main0(void){ 
  OS_Init();           // initialize, disable interrupts
  PortE_Init();
  DataLost = 0;        // lost data between producer and consumer
  NumSamples = 0;
  MaxJitter = 0;       // in 1us units

//********initialize communication channels
  OS_MailBox_Init();
  OS_Fifo_Init(1);    // ***note*** 4 is not big enough*****

//*******attach background tasks***********

// OS_AddSW1Task(&SW1Push,1);
//1 OS_AddSW2Task(&SW2Push,2);  // add this line in Lab 3
OS_AddSWTask(&SW1Push,2,1);
OS_AddSWTask(&SW2Push,2,2);	
	
  ADC_Open(0,3,0x4,1);  // sequencer 3, channel 4, PD3, sampling in DAS()
	ADC_Open(1,3,0x4,1);  // sequencer 3, channel 4, PD3, sampling in DAS2()
  //OS_AddPeriodicThread(&DAS,PERIOD,1); // 2 kHz real time sampling of PD3
	OS_AddPeriodicThread(&DAS2,1,2); // 2 kHz real time sampling of PD3
	OS_AddPeriodicThread(&DAS,1,2); // 2 kHz real time sampling of PD3
  NumCreated = 0 ;
// create initial foreground threads
  NumCreated += OS_AddThread(&Interpreter,128,3); 
  NumCreated += OS_AddThread(&Consumer,128,2); 
  NumCreated += OS_AddThread(&PID,128,4);  // Lab 3, make this lowest priority
	//OS_AddThread(&Crap, 128, 0);
 
  OS_Launch(TIME_2MS); // doesn't return, interrupts enabled in here
  return 0;            // this never executes
}




