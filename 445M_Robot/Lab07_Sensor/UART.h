// UART.h
// Runs on LM4F120
// Use UART0 to implement bidirectional data transfer to and from a
// computer running HyperTerminal.  This time, interrupts and FIFOs
// are used.
// Victor VO and Alex Taft
// Janmuary 29, 2014


// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

// standard ASCII symbols
#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20
#define DEL  0x7F

enum UART_CMD_NUM{
	NO_CMD,
	CLK_ON,
	CLK_OFF,
	CLK_RST,
	DEV0_ON,
	DEV0_OFF,
	HELP,
	ADC0_ON,
	ADC0_OFF,
	ADC1_ON,
	ADC1_OFF,
	UNKNOWN_CMD
};

typedef struct{
	int displayRunTime;
	int displayDEV0;
	int help;
	int displayADC0;
	int displayADC1;
} CommandState;
//CommandState SystemState;

/*
extern char UART_RxCMD_Buffer[];
extern char UART_RX_Buffer[];
extern unsigned char UART_CharacterCount;
extern int UART_MessageLine;
extern char UART_Message_Buffer[];
extern int UART_CMD;
*/

//------------UART_InChar------------
// Wait for new serial port input
// Initialize the UART for 115,200 baud rate (assuming 50 MHz clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void UART_Init(void);

//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
unsigned char UART_InChar(void);

//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART_OutChar(unsigned char data);

//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART_OutString(char *pt);

//------------UART_InUDec------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 32-bit unsigned number
//     valid range is 0 to 4294967295 (2^32-1)
// Input: none
// Output: 32-bit unsigned number
// If you enter a number above 4294967295, it will return an incorrect value
// Backspace will remove last digit typed
unsigned long UART_InUDec(void);

//-----------------------UART_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void UART_OutUDec(unsigned long n);

//---------------------UART_InUHex----------------------------------------
// Accepts ASCII input in unsigned hexadecimal (base 16) format
// Input: none
// Output: 32-bit unsigned number
// No '$' or '0x' need be entered, just the 1 to 8 hex digits
// It will convert lower case a-f to uppercase A-F
//     and converts to a 16 bit unsigned number
//     value range is 0 to FFFFFFFF
// If you enter a number above FFFFFFFF, it will return an incorrect value
// Backspace will remove last digit typed
unsigned long UART_InUHex(void);

//--------------------------UART_OutUHex----------------------------
// Output a 32-bit number in unsigned hexadecimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1 to 8 digits with no space before or after
void UART_OutUHex(unsigned long number);

//------------UART_InString------------
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
void UART_InString(char *bufPt, unsigned short max);


// ------------------- UART_RX_FifoSize --------------------------
// Returns the number of bytes currently in the RX software fifo
// Inputs:  none
// Output:  return number of bytes in the UART RX fifo
unsigned short UART_RX_FifoSize(void);

//-------------------------- UART_Interpreter -----------------------------
// If there is a character in the UART RX software fifo place it into 
// the buffer "UART_RxCMD_Buffer" to be processed. This buffer contains 
// all characters that have been received via the UART terminal until
// enter "CR" is pressed. Once Enter is pressed, the buffer is treated 
// as a string and tested for possible command matches. If no matches do
// nothing, otherwise set a flag for the coresponding command. The buffer 
// is cleared regardless.
// Input:  none
// Output: sets flag for command matches  
void UART_Interpreter(void);
	
