// Music.c
// File to initialize and define a Music driver

// Instrument List:
//		Sin Wave
//		Bassoon64
//		Oboe64
//		Trumpet64
//		Flute
//		Horn
//		SineUpdateDelay[61]

/* Includes */

#include "Music.h"
#include "timer.h"
#include "inc/tm4c123gh6pm.h"

long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value

/* Globals */

Song songPtr;
 
/* Implementation */

void Music_Init(void)
{
	songPtr.instPtr = 0;
	songPtr.wavePtr = 0;
	songPtr.notePtr = 0;
	songPtr.songLen = SNA_LEN;
	songPtr.curSong = 0;
	Music_SetEnvelope();
}
void Music_Rewind(void)
{
	//Restarts current song
	int32_t sr;
	sr = StartCritical();
	songPtr.wavePtr = 0;
	songPtr.notePtr = 0;
	EndCritical(sr);
}

void Music_Play()
{
	//Function to enable music
	int32_t sr;
	sr = StartCritical();
	Timer0A_Enable();
	Timer2A_Enable();
	EndCritical(sr);	
}

void Music_Pause()
{
	//Function to pause music
	int32_t sr;
	sr = StartCritical();
	Timer0A_Disable();
	Timer2A_Disable();
	EndCritical(sr);
}

void Music_Toggle()
{
	//Function to toggle state of music
	//Plays on if paused
	//Pauses if playing
	int32_t sr;
	sr = StartCritical();
	TIMER0_CTL_R ^= 0x00000001;
	TIMER2_CTL_R ^= 0x00000001;
	EndCritical(sr);
}
void Music_NextSong()
{
	//int32_t sr;
	//sr = StartCritical();
	songPtr.curSong++;
	if(songPtr.curSong >= NUM_SONGS)
		songPtr.curSong = 0;
	songPtr.notePtr = 0;
	songPtr.wavePtr = 0;
	switch(songPtr.curSong)
	{
		case 0:
			//SNA
			songPtr.songLen = SNA_LEN;
			break;
		case 1:
			songPtr.songLen = SCALE_LEN;
			//SOTW
			break;
		case 2:
			songPtr.songLen = SOTW_LEN;
			//Scale
			break;
		case 3:
			songPtr.songLen = FUR_LEN;
			//Scale
			break;	
		case 4:
			songPtr.songLen = EOT_LEN;
			break;
	}
	//EndCritical(sr);
	//Function to advance to the next song
}

//Moves to the next instrument
void Music_NextInst()
{
	int32_t sr;
	sr = StartCritical();
	songPtr.instPtr++;
	if(songPtr.instPtr >= NUM_INSTRUMENTS)
		songPtr.instPtr = 0;
	EndCritical(sr);
}

void Music_SetEnvelope(void)
{
	switch(notes[songPtr.curSong][songPtr.notePtr][ENVELOPE]){
		case NEVER:
			songPtr.envelope = 0;
			break;
		case Q_SEC:
			songPtr.envelope = 1;
			songPtr.divisor = 16000;
			songPtr.multiplier = songPtr.divisor;
			break;
		case H_SEC:
			songPtr.envelope = 1;
			songPtr.divisor = 32000;
			songPtr.multiplier = songPtr.divisor;
			break;
		case F_SEC:
			songPtr.envelope = 1;
			songPtr.divisor = 64000;
			songPtr.multiplier = songPtr.divisor;
			break;
		case D_SEC:
			songPtr.envelope = 1;
			songPtr.divisor = 128000;
			songPtr.multiplier = songPtr.divisor;
			break;
	}
}
