


long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value

void PING_Init(void (*task)(void));
void PING_Start (void);

extern unsigned long PING_Time1;
extern unsigned long PING_Time2;
extern unsigned long PING_Time3;
extern unsigned long PING_Time4;
