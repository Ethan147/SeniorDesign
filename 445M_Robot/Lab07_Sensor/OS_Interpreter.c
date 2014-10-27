#include "OS.h"
#include "UART.h"
#include "string.h"



//semaphores
Sema4Type Sema4_UART;

Sema4Type UART_DataAvailable;
Sema4Type UART_DataRoomLeft;
Sema4Type *OS_SystemState;

SystemState DisplayState;

char *commands[] = {"clk on\r",
                    "clk off\r",
                    "clk rst\r",
                    "dev0 on\r",
                    "dev0 off\r",
                    "help\r",
                    "adc0 on\r",
                    "adc0 off\r",
                    "adc1 on\r",
                    "adc1 off\r",
                    0};


//====================================================================
//                         OS INTERPRETER FUNCTIONS 
//====================================================================

// ---------------------- OS_Interpreter_Init ------------------------
void OS_Interpreter_Init(void){
	UART_Init();
	OS_InitSemaphore(&Sema4_UART, 1);
	
	DisplayState.Time.Data = 1; //start with run time clk on
}
										
// ------------------------ UART_InChar ------------------------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
unsigned char OS_UART_InChar(void){
  unsigned char result;

  OS_bWait(&Sema4_UART);
  result = UART_InChar();
  OS_bSignal(&Sema4_UART);

  return result;
}

// -------------------------- UART_OutChar ---------------------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void OS_UART_OutChar(unsigned char data){
  OS_bWait(&Sema4_UART);
  UART_OutChar(data);
  OS_bSignal(&Sema4_UART);
}

// ------------------------- UART_OutString --------------------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void OS_UART_OutString(char *pt){
  OS_bWait(&Sema4_UART);
  UART_OutString(pt);
  OS_bSignal(&Sema4_UART);
}

// ------------------------- UART_InUDec -----------------------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 32-bit unsigned number
//     valid range is 0 to 4294967295 (2^32-1)
// Input: none
// Output: 32-bit unsigned number
// If you enter a number above 4294967295, it will return an incorrect value
// Backspace will remove last digit typed
unsigned long OS_UART_InUDec(void){
  unsigned long result;

  OS_bWait(&Sema4_UART);
  result = UART_InUDec();
  OS_bSignal(&Sema4_UART);

  return result;
}

// -------------------------- UART_OutUDec ---------------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void OS_UART_OutUDec(unsigned long n){
  OS_bWait(&Sema4_UART);
  UART_OutUDec(n);
  OS_bSignal(&Sema4_UART);
}

// ------------------------- UART_InUHex -----------------------------
// Accepts ASCII input in unsigned hexadecimal (base 16) format
// Input: none
// Output: 32-bit unsigned number
// No '$' or '0x' need be entered, just the 1 to 8 hex digits
// It will convert lower case a-f to uppercase A-F
//     and converts to a 16 bit unsigned number
//     value range is 0 to FFFFFFFF
// If you enter a number above FFFFFFFF, it will return an incorrect value
// Backspace will remove last digit typed
unsigned long OS_UART_InUHex(void){
  unsigned long result;

  OS_bWait(&Sema4_UART);
  result = UART_InUHex();
  OS_bSignal(&Sema4_UART);

  return result;
}

// ------------------------- UART_OutUHex ----------------------------
// Output a 32-bit number in unsigned hexadecimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1 to 8 digits with no space before or after
void OS_UART_OutUHex(unsigned long number){
  OS_bWait(&Sema4_UART);
  UART_OutUHex(number);
  OS_bSignal(&Sema4_UART);
}

// ----------------------- UART_InString -----------------------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until <enter> is typed
//    or until max length of the string is reached.
// It echoes each character as it is inputted.
// If a backspace is inputted, the string is modified
//    and the backspace is echoed
// terminates the string with a null character
// uses busy-waiting synchronization on RDRF
// Input: pointer to empty buffer, size of buffer
// Output: Null terminated string
// -- Modified by Agustinus Darmawan + Mingjie Qiu --
void OS_UART_InString(char *bufPt, unsigned short max){
  OS_bWait(&Sema4_UART);
  UART_InString(bufPt, max);
  OS_bSignal(&Sema4_UART);
}

// ---------------=----- UART_RX_FifoSize ----------------------------
// Returns the number of bytes currently in the RX software fifo
// Inputs:  none
// Output:  return number of bytes in the UART RX fifo
unsigned short OS_UART_RX_FifoSize(void){
  unsigned short result;

  OS_bWait(&Sema4_UART);
  result = UART_RX_FifoSize();
  OS_bSignal(&Sema4_UART);

  return result;
}


  
// ----------------------- OS_UART_SendHelp -----------------------------
// Displays lists of commands the system will recognize if entered
// Input:   none
// Ouput: Displays available commands on the terminal screen
void OS_UART_SendHelp(void){
  char **command = commands;

  OS_bWait(&Sema4_UART);

  
	UART_OutString("\n\n\r--------- COMMANDS -----------\n\r");
  while (*command != 0){
    UART_OutString(*command++);
		UART_OutString("\n");
  }
	UART_OutString("------------------------------\n\n\r");
	
  OS_bSignal(&Sema4_UART);
}

// ------------------- OS_UpdateSystemState --------------------------
void OS_UpdateSystemState(char* command){

/*  
  if(strcmp(inString, "clk on\r") == 0)         //clk on
    SystemState.displayRunTime = CLK_ON;
  else if(strcmp(inString, "clk off\r") == 0)   //clk off
    SystemState.displayRunTime = CLK_OFF;
  else if(strcmp(inString, "clk rst\r") == 0)   //clk rst
    UART_CMD =  CLK_RST;
  else if(strcmp(inString, "dev0 on\r") == 0)   //dev0 on
    SystemState.displayDEV0 = DEV0_ON;
  else if(strcmp(inString, "dev0 off\r") == 0)  //dev0 off
    SystemState.displayDEV0 = DEV0_OFF;
  else if(strcmp(inString, "help\r") == 0)      //dev0 off
    SystemState.help = HELP;
  else if(strcmp(inString, "adc0 on\r") == 0)   //adc0 on
    SystemState.displayADC0 = ADC0_ON;
  else if(strcmp(inString, "adc0 off\r") == 0)  //adc0 off
    SystemState.displayADC0 = ADC0_OFF;
  else if(strcmp(inString, "adc1 on\r") == 0)   //adc1 on
    SystemState.displayADC1 = ADC1_ON;
  else if(strcmp(inString, "adc1 off\r") == 0)  //adc1 off
    SystemState.displayADC1 = ADC1_OFF;
  else 
    UART_CMD =  UNKNOWN_CMD;
}
*/


  if(strcmp(command, "clk on\r") == 0)     //clk on
    DisplayState.Time.Data = 1;
  else if(strcmp(command, "clk off\r") == 0)   //clk off
    DisplayState.Time.Data = 0;
	else if(strcmp(command, "help\r") == 0)
		DisplayState.Help.Data = 1;
	else
		OS_DisplayUnknownCMD_Flash();
	
}


// ----------------------- UART_Interpreter --------------------------
// If there is a character in the UART RX software fifo place it into 
// the buffer "UART_RxCMD_Buffer" to be processed. This buffer contains 
// all characters that have been received via the UART terminal until
// enter "CR" is pressed. Once Enter is pressed, the buffer is treated 
// as a string and tested for possible command matches. If no matches do
// nothing, otherwise set a flag for the coresponding command. The buffer 
// is cleared regardless.
// Input:  none
// Output: sets flag for command matches
char UART_RxCMD_Buffer[50];
char UART_RX_Buffer[50];	//characters received 
char UART_Message_Buffer[50];	
unsigned char UART_CharacterCount = 0;
//int UART_MessageLine = 0; 
int UART_CMD = 0;

void UART_Interpreter(void){
	static int CMD_CharCount = 0;  //number of characters entered
	
	UART_CMD = NO_CMD;
	
	if(UART_RX_FifoSize() > 0){ 	                      //a character has been sent
		UART_RxCMD_Buffer[CMD_CharCount] = UART_InChar();	//store character in the buffer
		UART_RxCMD_Buffer[CMD_CharCount + 1] = 0;					//terminate the string
		UART_OutChar(UART_RxCMD_Buffer[CMD_CharCount]);	 	//echo the character received to the temrinal 
			
		if((UART_RxCMD_Buffer[CMD_CharCount] == BS) || (UART_RxCMD_Buffer[CMD_CharCount] == DEL)){
			UART_RxCMD_Buffer[CMD_CharCount] = 0;
			UART_RxCMD_Buffer[CMD_CharCount - 1] = 0;
			CMD_CharCount--;
		}
		else if(UART_RxCMD_Buffer[CMD_CharCount] == CR){	//enter was hit
			UART_OutChar(CR); UART_OutChar(LF);						  //align on the terminal side	
			OS_UpdateSystemState(UART_RxCMD_Buffer);

			while(CMD_CharCount >= 0){ 											//clear the buffer
				UART_RxCMD_Buffer[CMD_CharCount] = 0;	
				CMD_CharCount--;
			}
			CMD_CharCount = 0;
		}
		else{
			CMD_CharCount++;
		}
	}
}

// -------------------- OS_ProcessInterpreter ------------------------
// If there is a character in the UART RX software fifo place it into 
// the buffer "UART_RxCMD_Buffer" to be processed. This buffer contains 
// all characters that have been received via the UART terminal until
// enter "CR" is pressed. Once Enter is pressed, the buffer is treated 
// as a string and tested for possible command matches. If no matches do
// nothing, otherwise set a flag for the coresponding command. The buffer 
// is cleared regardless.
// Input:  none
// Output: sets flag for command matches  
void OS_ProcessInterpreter(void){
  while(1){
		OS_bWait(&Sema4_UART);
		UART_Interpreter();
		OS_bSignal(&Sema4_UART);
	
		OS_DisplayInterpreter(UART_RxCMD_Buffer);  //display what was received
	}
}

