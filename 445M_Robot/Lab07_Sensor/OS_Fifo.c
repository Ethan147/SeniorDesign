#include "OS.h"
#include "fifo.h"

//====================================================================
//                        OS FIFO FUNCTIONS 
//====================================================================

//FIFO Variables
#define MAX_FIFO_SIZE 128
typedef unsigned long OS_FIFO_TYPE;
AddPointerFifo(OS, MAX_FIFO_SIZE, OS_FIFO_TYPE, 1, 0);
Sema4Type OSFifo_RoomAvailable;
Sema4Type OSFifo_DataAvailable;

// ******** OS_Fifo_Init ************
// Initialize the Fifo to be empty
// Inputs: size
// Outputs: none 
// In Lab 2, you can ignore the size field
// In Lab 3, you should implement the user-defined fifo size
// In Lab 3, you can put whatever restrictions you want on size
//    e.g., 4 to 64 elements
//    e.g., must be a power of 2,4,8,16,32,64,128
void OS_Fifo_Init(unsigned long size){
  OSFifo_Init();
  OS_InitSemaphore(&OSFifo_RoomAvailable, size);
  OS_InitSemaphore(&OSFifo_DataAvailable, 0);
}

// ******** OS_Fifo_Put ************
// Enter one data sample into the Fifo
// Called from the background, so no waiting 
// Inputs:  data
// Outputs: true if data is properly saved,
//          false if data not saved, because it was full
// Since this is called by interrupt handlers 
//  this function can not disable or enable interrupts
int OS_Fifo_Put(unsigned long data){
  if (OSFifo_RoomAvailable.Value > 0){
    OSFifo_RoomAvailable.Value--;
    OSFifo_Put(data);
    OS_Signal(&OSFifo_DataAvailable);

    return 1;
  }

  return 0;
}

// ******** OS_Fifo_Get ************
// Remove one data sample from the Fifo
// Called in foreground, will spin/block if empty
// Inputs:  none
// Outputs: data 
unsigned long OS_Fifo_Get(void){
  unsigned long result;

  OS_Wait(&OSFifo_DataAvailable);
  OSFifo_Get(&result);
  OS_Signal(&OSFifo_RoomAvailable);

  return result;
}

// ******** OS_Fifo_Size ************
// Check the status of the Fifo
// Inputs: none
// Outputs: returns the number of elements in the Fifo
//          greater than zero if a call to OS_Fifo_Get will return right away
//          zero or less than zero if the Fifo is empty 
//          zero or less than zero if a call to OS_Fifo_Get will spin or block
long OS_Fifo_Size(void){
  return OSFifo_Size();
}
