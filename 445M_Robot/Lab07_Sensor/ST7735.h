/***************************************************
  This is a library for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
  as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

// ST7735.h
// Runs on LM4F120
// Low level drivers for the ST7735 160x128 LCD based off of
// the file described above.
// Daniel Valvano
// June 19, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#ifndef _ST7735H_
#define _ST7735H_

// some flags for ST7735_InitR()
enum initRFlags{
  none,
  INITR_GREENTAB,
  INITR_REDTAB,
  INITR_BLACKTAB
};

#define ST7735_TFTWIDTH  128
#define ST7735_TFTHEIGHT 160

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Color definitions
#define  ST7735_BLACK  0x0000
#define  ST7735_RED    0x001F
#define  ST7735_BLUE   0xF800
#define  ST7735_GREEN  0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF

// Initialization for ST7735B screens
void ST7735_InitB(void);

// Initialization for ST7735R screens (green or red tabs)
// ST7735_InitR(INITR_REDTAB);
void ST7735_InitR(enum initRFlags option);

//void setAddrWindow(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1); // function is now private

//void pushColor(unsigned short color); // function is now private

void ST7735_DrawPixel(short x, short y, unsigned short color);

void ST7735_DrawFastVLine(short x, short y, short h, unsigned short color);

void ST7735_DrawFastHLine(short x, short y, short w, unsigned short color);

void ST7735_FillScreen(unsigned short color);

void ST7735_FillRect(short x, short y, short w, short h, unsigned short color);

unsigned short ST7735_Color565(unsigned char r, unsigned char g, unsigned char b);

unsigned short ST7735_SwapColor(unsigned short x);

void ST7735_DrawBitmap(short x, short y, const unsigned short *image, short w, short h);

void ST7735_DrawCharS(short x, short y, unsigned char c, short textColor, short bgColor, unsigned char size);

void ST7735_DrawChar(short x, short y, unsigned char c, short textColor, short bgColor, unsigned char size);

void ST7735_SetRotation(unsigned char m);

void ST7735_InvertDisplay(int i);

// *************** ST7735_PlotClear ********************
// Clear the graphics buffer, set X coordinate to 0
// This routine clears the display 
// Inputs: ymin and ymax are range of the plot
// Outputs: none
void ST7735_PlotClear(long ymin, long ymax);

// *************** ST7735_PlotPoint ********************
// Used in the voltage versus time plot, plot one point at y
// It does output to display 
// Inputs: y is the y coordinate of the point plotted
// Outputs: none
void ST7735_PlotPoint(long y);

// *************** ST7735_PlotBar ********************
// Used in the voltage versus time bar, plot one bar at y
// It does not output to display until RIT128x96x4ShowPlot called
// Inputs: y is the y coordinate of the bar plotted
// Outputs: none
void ST7735_PlotBar(long y);

// *************** ST7735_PlotdBfs ********************
// Used in the amplitude versus frequency plot, plot bar point at y
// 0 to 0.625V scaled on a log plot from min to max
// It does output to display 
// Inputs: y is the y ADC value of the bar plotted
// Outputs: none
void ST7735_PlotdBfs(long y);

// *************** ST7735_PlotNext ********************
// Used in all the plots to step the X coordinate one pixel
// X steps from 0 to 127, then back to 0 again
// It does not output to display 
// Inputs: none
// Outputs: none
void ST7735_PlotNext(void);



//*************** All functions below were written by Victor Vo and Alex Taft
#define MAX_LINES   6
#define MAX_DEVICES 2


//-------------------- ST7735_ResetMessage --------------------------
// Displays text "RESET" on the middle of the screen for 1.5 seconds.
// Works for both portrait and landscape orinetations.
// Inputs: None
// Outputs: None
void ST7735_ResetMessage(void);

// ------------------------ ST7735_ClearLine --------------------------
// Clear line of text on the display (text size 1) all the way across the
// diplay by using ST7735_FillRect. Approximately 10 pixels high. 
// Input:   Y cordinate; Clears area in region Y to Y+10 
// Ouptut:  none
void ST7735_ClearLine(int y, short bgColor);


//--------------------------- ST7735_Message ---------------------------
// Displays a line of message on the lcd.
// Input: device  (0 or 1) for top or bottom half respectively
//        line    line number of the device, from 0 to 3 (at least)
//        string  the message to display (null terminated ASCII String)
//        value   a number to display
void ST7735_Message(int device, int line, char *string, long value);


//--------------------------- ST7735_DisplayRunTime -----------------------------
// Displays the Run Time in seconds and approximate number of minutes
// Input:   number of seconds program has been running
// Outputs: none
void ST7735_DisplayRunTime(unsigned long runTime);


// ---------------------- ST7735_NoDisplayRunTime ----------------------------------
// Clear the text at the bottom of screen where run time is displyed.
// Does not affect actual run time counter, just the display.
// Input:   none
// Output:  none
void ST7735_NoDisplayRunTime(void);

//---------------------ST7735_UnknownCMD_Flash-------------------------------------
// Creates a flashing effect on the CMD prompt line to signal
// to the user that the previous input was invalid.
void ST7735_UnknownCMD_Flash(void);

// ------------------------------- ST7735_Interpreter ------------------------------
// Displays what was entered by the terminal on the screen. Clears entered text every
// time enter "CR" is pressed. Designed to be used with UART_Interpreter.
// Input:   pointer to the command string buffer
// Output:  displays commands entered
void ST7735_Interpreter(char* stringCMD);


#endif
