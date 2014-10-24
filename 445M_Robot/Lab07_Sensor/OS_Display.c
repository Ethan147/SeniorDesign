#include "OS.h"
#include "ST7735.h"

//====================================================================
//                           OS LCD FUNCTIONS 
//====================================================================

//Display Variables
Sema4Type Sema4_LCD;

void OS_Display_Init(){
  ST7735_InitR(INITR_REDTAB);
  ST7735_ResetMessage();
  OS_InitSemaphore(&Sema4_LCD, 1);
}


//-------------------- ST7735_ResetMessage --------------------------
// Displays text "RESET" on the middle of the screen for 1.5 seconds.
// Works for both portrait and landscape orinetations.
// Inputs: None
// Outputs: None
void OS_DisplayResetMessage(void){
  OS_bWait(&Sema4_LCD);
  ST7735_ResetMessage();
  OS_bSignal(&Sema4_LCD);
}

// ------------------------ ST7735_ClearLine --------------------------
// Clear line of text on the display (text size 1) all the way across the
// diplay by using ST7735_FillRect. Approximately 10 pixels high. 
// Input:   Y cordinate; Clears area in region Y to Y+10 
// Ouptut:  none
void OS_DisplayClearLine(int y, short bgColor){
  OS_bWait(&Sema4_LCD);
  ST7735_ClearLine(y, bgColor);
  OS_bSignal(&Sema4_LCD);
}

//--------------------------- ST7735_Message ---------------------------
// Displays a line of message on the lcd
// Input: device  (0 or 1) for top or bottom half respectively
//        line    line number of the device, from 0 to 3 (at least)
//        string  the message to display (null terminated ASCII String)
//        value   a number to display
void OS_DisplayMessage(int device, int line, char *string, long value){
  OS_bWait(&Sema4_LCD);
  ST7735_Message(device, line, string, value);
  OS_bSignal(&Sema4_LCD);
}


// --------------------- ST7735_DisplayRunTime -----------------------
// Displays the Run Time in seconds and approximate number of minutes
// Input:   number of seconds program has been running
// Outputs: none
void OS_DisplayRunTime(int isDisplaying, unsigned long runTime){
  OS_bWait(&Sema4_LCD);
  if (isDisplaying == 0){
    ST7735_NoDisplayRunTime();    
  } else {
    ST7735_DisplayRunTime(runTime);
  }
  OS_bSignal(&Sema4_LCD);
	
	OS_Suspend(); 
}

// ------------------ OS_DisplayUnknownCMD_Flash --------------------
// Creates a flashing effect on the CMD prompt line to signal
// to the user that the previous input was invalid.
void OS_DisplayUnknownCMD_Flash(void){
  OS_bWait(&Sema4_LCD);
  ST7735_UnknownCMD_Flash();
  OS_bSignal(&Sema4_LCD);
}


// --------------------- OS_DisplayInterpreter -----------------------
void OS_DisplayInterpreter(char* cmd){
  OS_bWait(&Sema4_LCD);
  ST7735_Interpreter(cmd);  //display what was received
  OS_bSignal(&Sema4_LCD);

  OS_Suspend();
}

// ------------------------- OS_Display ------------------------------
void OS_Display(void){

  for(;;){
    
    OS_DisplayRunTime(DisplayState.Time.Data, OS_RunTime);
    
		if(DisplayState.Help.Data == 1){
			OS_UART_SendHelp();
			DisplayState.Help.Data = 0;
		}
		
		OS_Sleep(50);
  }
}

// ------------------------- OS_PlotPoint ------------------------------
void OS_PlotPoint(long data){
	  OS_bWait(&Sema4_LCD);
		ST7735_PlotPoint(data);
	  OS_bSignal(&Sema4_LCD);
}

// ------------------------- OS_PlotBar ------------------------------
void OS_PlotBar(long data){
	  OS_bWait(&Sema4_LCD);
		ST7735_PlotBar(data);
	  OS_bSignal(&Sema4_LCD);
}

// ------------------------- OS_PlotdBfs ------------------------------
void OS_PlotdBfs(long y){
	  OS_bWait(&Sema4_LCD);
		ST7735_PlotdBfs(y);
	  OS_bSignal(&Sema4_LCD);
}
	
// ------------------------- OS_PlotNext ------------------------------
void OS_PlotNext(void){
	  OS_bWait(&Sema4_LCD);
		ST7735_PlotNext();
	  OS_bSignal(&Sema4_LCD);
}

// ------------------------- OS_PlotNext ------------------------------
void OS_PlotClear(long ymin, long ymax){  // range from 0 to 4095{
	  OS_bWait(&Sema4_LCD);
		ST7735_PlotClear(ymin, ymax);
	  OS_bSignal(&Sema4_LCD);
}

