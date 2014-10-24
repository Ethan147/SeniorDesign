#include "OS.h"

//====================================================================
//                         OS MAILBOX FUNCTIONS 
//====================================================================

//Mailbox Variables
unsigned long OSMailbox;
Sema4Type OSMailbox_BoxFree;
Sema4Type OSMailbox_DataValid;

// --------------------- OS_MailBox_Init -----------------------------
// Initialize communication channel
// Inputs:  none
// Outputs: none
void OS_MailBox_Init(void){
  OSMailbox = 0;
  OS_InitSemaphore(&OSMailbox_DataValid, 0);
  OS_InitSemaphore(&OSMailbox_BoxFree, 1);

}

// --------------------- OS_MailBox_Send -----------------------------
// enter mail into the MailBox
// Inputs:  data to be sent
// Outputs: none
// This function will be called from a foreground thread
// It will spin/block if the MailBox contains data not yet received 
void OS_MailBox_Send(unsigned long data){
  OS_bWait(&OSMailbox_BoxFree);
  OSMailbox = data;
  OS_bSignal(&OSMailbox_DataValid);
}

// -------------------- OS_MailBox_Recv ------------------------------
// remove mail from the MailBox
// Inputs:  none
// Outputs: data received
// This function will be called from a foreground thread
// It will spin/block if the MailBox is empty 
unsigned long OS_MailBox_Recv(void){
  unsigned long result;

  OS_bWait(&OSMailbox_DataValid);
  result = OSMailbox;
  OS_bSignal(&OSMailbox_BoxFree);

  return result;
}
