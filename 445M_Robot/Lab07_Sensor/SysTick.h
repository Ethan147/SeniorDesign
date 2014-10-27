// SysTick.h
// Runs on LM4F120
// Provide functions that initialize the SysTick module, wait at least a
// designated number of clock cycles, and wait approximately a multiple
// of 10 milliseconds using busy wait.  After a power-on-reset, the
// LM4F120 gets its clock from the 16 MHz precision internal oscillator,
// which can vary by +/- 1% at room temperature and +/- 3% across all
// temperature ranges.  If you are using this module, you may need more
// precise timing, so it is assumed that you are using the PLL to set
// the system clock to 50 MHz.  This matters for the function
// SysTick_Wait10ms(), which will wait longer than 10 ms if the clock is
// slower.
// Victor Vo and Alex Taft
// January 29, 2014


// --------------------- SysTick_Init --------------------------
// Initialize SysTick with busy wait running at bus clock
// Input:   none
// Output:  none
void SysTick_Init(void);


// ------------------ SysTick_InitInt -------------------------
// Initialize SysTick periodic interrupts
// Input: interrupt period
//        Units of period are 20ns (assuming 50 MHz clock)
//        Maximum is 2^24-1
//        Minimum is determined by length of ISR
// Output: none
void SysTick_InitInt(unsigned long period);


// ------------------------- SysTick_Wait -------------------------
// Time delay using busy wait.
// Input:   The delay parameter (units of 20 nsec for 50 MHz clock)
// Output:  none
void SysTick_Wait(unsigned long delay);


// ----------------------- SysTick_Wait10ms ------------------------
// Time delay using busy wait. Assumes 50 MHz system clock.
// Input:    The delay parameter (units of 10ms for 50 MHz clock)
// Output:   none
void SysTick_Wait10ms(unsigned long delay);


// ----------------------- SysTick_Wait1ms ------------------------
// Time delay using busy wait. Assumes 50 MHz system clock.
// Input:   The delay parameter (units of 1ms for 50 MHz clock)
// Output:  none
void SysTick_Wait1ms(unsigned long delay);


void Delay1ms(unsigned long n);

