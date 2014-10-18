// Music.h

// Instrument List:
//		Sin Wave
//		Bassoon64
//		Oboe64
//		Trumpet64
//		Flute
//		Horn
// 		SinWaveUpdate[61]

/* Includes */

#include <stdint.h>
#include <stdio.h>

/* Typedefs */

//Song structure
//Contains the pointers and variables to control the song
typedef struct 
{
	uint16_t wavePtr;		//Where in the wave array we currently are
	uint16_t instPtr;		//Which instrument we're using
	uint16_t notePtr;		//Which note in the music we're using
	uint32_t songLen;		//Number of notes in the current song
	uint32_t curSong;		//Track number currently playing
	uint32_t multiplier;//When envelope is active, multiply number by this
	uint32_t divisor;		//When envelope is active, divide number by this
	uint32_t envelope;		//if 1, envelope active. 0 otherwise
} Song;

/* Constants */

#define C_2 19111   // 65.406 Hz
#define DF_1 18039   // 69.296 Hz
#define D_1 17026   // 73.416 Hz
#define EF_1 16071   // 77.782 Hz
#define E_1 15169   // 82.407 Hz
#define F_1 14317   // 87.307 Hz
#define GF_1 13514   // 92.499 Hz
#define G_1 12755   // 97.999 Hz
#define AF_1 12039   // 103.826 Hz
#define A_1 11364   // 110.000 Hz
#define BF_1 10726   // 116.541 Hz
#define B_1 10124   // 123.471 Hz
#define C_1 9556   // 130.813 Hz
#define DF0 9019   // 138.591 Hz
#define D0 8513   // 146.832 Hz
#define EF0 8035   // 155.563 Hz
#define E0 7584   // 164.814 Hz
#define F0 7159   // 174.614 Hz
#define GF0 6757   // 184.997 Hz
#define G0 6378   // 195.998 Hz
#define AF0 6020   // 207.652 Hz
#define A0 5682   // 220.000 Hz
#define BF0 5363   // 233.082 Hz
#define B0 5062   // 246.942 Hz
#define C0 4778   // 261.626 Hz
#define DF 4510   // 277.183 Hz
#define D 4257   // 293.665 Hz
#define EF 4018   // 311.127 Hz
#define E 3792   // 329.628 Hz
#define F 3579   // 349.228 Hz
#define GF 3378   // 369.994 Hz
#define G 3189   // 391.995 Hz
#define AF 3010   // 415.305 Hz
#define A 2841   // 440.000 Hz
#define BF 2681   // 466.164 Hz
#define B 2531   // 493.883 Hz
#define C 2389   // 523.251 Hz
#define DF1 2255   // 554.365 Hz
#define D1 2128   // 587.330 Hz
#define EF1 2009   // 622.254 Hz
#define E1 1896   // 659.255 Hz
#define F1 1790   // 698.456 Hz
#define GF1 1689   // 739.989 Hz
#define G1 1594   // 783.991 Hz
#define AF1 1505   // 830.609 Hz
#define A1 1420   // 880.000 Hz
#define BF1 1341   // 932.328 Hz
#define B1 1265   // 987.767 Hz
#define C1 1194   // 1046.502 Hz
#define DF2 1127   // 1108.731 Hz
#define D2 1064   // 1174.659 Hz
#define EF2 1004   // 1244.508 Hz
#define E2 948   // 1318.510 Hz
#define F2 895   // 1396.913 Hz
#define GF2 845   // 1479.978 Hz
#define G2 797   // 1567.982 Hz
#define AF2 752   // 1661.219 Hz
#define A2 710   // 1760.000 Hz
#define BF2 670   // 1864.655 Hz
#define B2 633   // 1975.533 Hz
#define C2 597   // 2093.005 Hz		

//Number of defined instruments
#define NUM_INSTRUMENTS 4

#define NOTE 0	 //Note is the first value
#define LENGTH 1 //Length of note is second
#define ENVELOPE 2	//Envelope length is third

//A rest is given by replacing a note with one of the below
//The note should be the same as the note length
#define SIXTYFOURTH_REST		1250000
#define THIRTYSECOND_REST		2500000
#define SIXTEENTH_REST		5000000
#define EIGHTH_REST				10000000
#define QUARTER_REST			20000000
#define HALF_REST					40000000
#define FULL_REST					80000000
#define DOUBLE_REST				160000000

//Note lengths
#define EIGHTH	10000000
#define QUARTER	20000000
#define HALF		40000000
#define FULL		80000000
#define DOUBLE	160000000

//Number of seconds for the wave to decay to zero
#define NEVER		0
#define Q_SEC		1
#define H_SEC		2
#define F_SEC		3
#define D_SEC		4

//Length in notes of Fur Elise
#define SNA_LEN 16
//Length in notes of one scale
#define SCALE_LEN 16
//Length in notes of Smoke on the Water
#define SOTW_LEN 24
//Length in notes of Fur Elise
#define FUR_LEN 41
//Length in notes of the longest song in memory
//Allows for switching between multiple songs
#define MAX_SONG_LEN	41	
//Length of Eyes of Texas
#define EOT_LEN 49
//Length in notes of the longest song in memory
//Allows for switching between multiple songs
//Number of songs we have
#define NUM_SONGS	5

	/***********Adding New Songs****************
	* 1)Increace NUM_SONGS by 1
	* 2)Add notes to the end of the notes array
	* 3a)If song is longer than current longest song,
	*		increace the MAX_SONG_LEN define to compensate.
	*		Pad other songs with {1,1} notes to get them to same length.
	*	3b)If song is shorter than longest song, pad with
	*		{1,1} notes until equal length
	*	4)Add a define for this song's length
	* 5)In the Music_NextSong() function, add a case for your song;
	*		All you need to do is set songPtr.songLen to the define of
	*		your song's length
	*
	********************************************/

	//Contains all songs
	//Songs shorter than the longest one should be padded with empty notes
	//Empty note is defined as {1,1} to make the interrupt to the next note run instantly
	static const uint32_t notes [NUM_SONGS][MAX_SONG_LEN][3] = 
	{		
		// Seven Nation Army
		{			
			{E, HALF, NEVER}, {E, EIGHTH, NEVER}, {THIRTYSECOND_REST, THIRTYSECOND_REST, NEVER}, {E, QUARTER, NEVER}, 
			{G, QUARTER, NEVER}, {G, EIGHTH, NEVER}, {E, QUARTER, NEVER}, {E, EIGHTH, NEVER}, 
			{D, QUARTER, NEVER}, {D, EIGHTH, NEVER}, {C0, HALF, NEVER}, {C0, HALF, NEVER}, 
			{THIRTYSECOND_REST, THIRTYSECOND_REST, NEVER}, {B0, HALF, NEVER}, {B0, EIGHTH, NEVER}, {FULL_REST, FULL_REST, NEVER},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}
		},
		//G Scale, zero patted to be the same length as SOTW.
		{
			{G, DOUBLE, H_SEC},	{A, DOUBLE, H_SEC}, {B, DOUBLE, H_SEC}, {C, DOUBLE, H_SEC}, 
			{D1, DOUBLE, H_SEC}, {E1, DOUBLE, H_SEC}, {F1, DOUBLE, H_SEC}, {G1, DOUBLE, H_SEC},
			{G1, DOUBLE, H_SEC}, {F1, DOUBLE, H_SEC}, {E1, DOUBLE, H_SEC}, {D1, DOUBLE, H_SEC},
			{C, DOUBLE, H_SEC}, {B, DOUBLE, H_SEC}, {A, DOUBLE, H_SEC}, {FULL_REST, FULL_REST, H_SEC},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}
		},
		//Smoke on the Water
		{			
			{E, QUARTER, NEVER},	{EIGHTH_REST, EIGHTH_REST, NEVER}, {G, QUARTER, NEVER}, {EIGHTH_REST, EIGHTH_REST, NEVER}, 
			{A, HALF, NEVER}, {E, QUARTER, NEVER},	{EIGHTH_REST, EIGHTH_REST, NEVER}, {G, QUARTER, NEVER}, 
			{EIGHTH_REST, EIGHTH_REST, NEVER}, {BF, QUARTER, NEVER}, {A, QUARTER, NEVER}, {A, QUARTER, NEVER},			
			{E, QUARTER, NEVER},	{EIGHTH_REST, EIGHTH_REST, NEVER}, {G, QUARTER, NEVER}, {EIGHTH_REST, EIGHTH_REST, NEVER}, 
			{A, HALF, NEVER}, {G, QUARTER, NEVER}, {EIGHTH_REST, EIGHTH_REST, NEVER},	{E, QUARTER, NEVER},	
			{E, QUARTER, NEVER},	{E, QUARTER, NEVER},	{EIGHTH_REST, EIGHTH_REST, NEVER}, {FULL_REST, FULL_REST, NEVER},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}
		},
		// Fur Elise
		{			
			{E2, QUARTER, NEVER}, {EF2, QUARTER, NEVER}, {E2, QUARTER, NEVER}, {EF2, QUARTER, NEVER}, 
			{E2, QUARTER, NEVER}, {B1, QUARTER, NEVER}, {D2, QUARTER, NEVER}, {C1, QUARTER, NEVER}, 
			{A1, QUARTER, NEVER}, {QUARTER_REST, QUARTER_REST, NEVER},
			
			{C0,QUARTER,NEVER}, {E1,QUARTER,NEVER}, {A1,QUARTER,NEVER},
<<<<<<< HEAD
			{B1, QUARTER, NEVER}, {QUARTER_REST, QUARTER_REST, NEVER},
			
			{E1,QUARTER,NEVER}, {AF1,QUARTER,NEVER}, {B1,QUARTER,NEVER},
			{C1, QUARTER, NEVER}, {QUARTER_REST, QUARTER_REST, NEVER},
			
			{E1,QUARTER,NEVER},
			
			{E2, QUARTER, NEVER}, {EF2, QUARTER, NEVER}, {E2, QUARTER, NEVER}, {EF2, QUARTER, NEVER}, 
			{E2, QUARTER, NEVER}, {B1, QUARTER, NEVER}, {D2, QUARTER, NEVER}, {C1, QUARTER, NEVER}, 
			{A1, QUARTER, NEVER}, {QUARTER_REST, QUARTER_REST, NEVER},
			
			{C0,QUARTER,NEVER}, {E1,QUARTER,NEVER}, {A1,QUARTER,NEVER},
			{B1, QUARTER, NEVER}, {QUARTER_REST, QUARTER_REST, NEVER},
			
			{E1,QUARTER,NEVER}, {C1,QUARTER,NEVER}, {B1,QUARTER,NEVER},
			{A1, QUARTER, NEVER}, {QUARTER_REST, QUARTER_REST, NEVER},			
			
=======
			{B1, QUARTER, NEVER}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1},
			{1,1,1}, {1,1,1}, {1,1,1}, {1,1,1}
		},
		//Eyes of Texas
		{
			{F1, QUARTER, NEVER},{F1, EIGHTH, NEVER},{BF1, HALF, NEVER},{BF1, EIGHTH, NEVER},{F1, EIGHTH, NEVER},
			{BF1, QUARTER, NEVER},{F1, EIGHTH, NEVER},{BF1, QUARTER, NEVER},{C1, EIGHTH, NEVER},{D2, HALF, NEVER},
			{D2, QUARTER, NEVER},{BF1, HALF, NEVER},{BF1, QUARTER, NEVER},{EF2, EIGHTH, NEVER},{EF2, HALF, H_SEC},
			{EF2, EIGHTH, NEVER},{BF1, QUARTER, NEVER},{BF1, EIGHTH, NEVER},{C1, EIGHTH, NEVER},{C1, QUARTER, NEVER},
			{D2, FULL, NEVER},{EIGHTH_REST,EIGHTH_REST,NEVER},{F1, QUARTER, NEVER},{F1, EIGHTH, NEVER},{BF1, HALF, NEVER},
			{BF1, EIGHTH, NEVER},{F1, EIGHTH, NEVER},{BF1, QUARTER, NEVER},{F1, EIGHTH, NEVER},{BF1, QUARTER, NEVER},
			{C1, EIGHTH, NEVER},{D2, HALF, NEVER},{D2, QUARTER, NEVER},{BF1, EIGHTH, NEVER},{BF1, QUARTER, NEVER},
			{D2, EIGHTH, NEVER},{D2, QUARTER, H_SEC},{D2, EIGHTH, NEVER},{D2, QUARTER, NEVER},{C1, EIGHTH, NEVER},
			{C1, QUARTER, H_SEC},{C1, EIGHTH, NEVER},{C1, QUARTER, NEVER},{D2, EIGHTH, NEVER},{D2, QUARTER, NEVER},
			{C1, EIGHTH, NEVER},{C1, QUARTER, NEVER},{C1, FULL, F_SEC},{EIGHTH_REST,EIGHTH_REST,NEVER}
>>>>>>> 2827cf7bc1f2233d06611bc3ffa6223f17b45197
		}
	};
	
	
static const unsigned Instruments[NUM_INSTRUMENTS][64] = 
{
	{	//Bassoon
		1068, 1169, 1175, 1161, 1130, 1113, 1102, 1076, 1032, 985, 963, 987, 1082, 1343, 1737, 1863, 
		1575, 1031, 538, 309, 330, 472, 626, 807, 1038, 1270, 1420, 1461, 1375, 1201, 1005, 819, 658, 
		532, 496, 594, 804, 1055, 1248, 1323, 1233, 1049, 895, 826, 826, 850, 862, 861, 899, 961, 1006, 
		1023, 1046, 1092, 1177, 1224, 1186, 1133, 1098, 1102, 1109, 1076, 1027, 1003
	},
	{	//Oboe
		1024, 1024, 1014, 1008, 1022, 1065, 1093, 1006, 858, 711, 612, 596, 672, 806, 952, 1074, 1154, 1191, 
		1202, 1216, 1236, 1255, 1272, 1302, 1318, 1299, 1238, 1140, 1022, 910, 827, 779, 758, 757, 782, 856, 
		972, 1088, 1177, 1226, 1232, 1203, 1157, 1110, 1067, 1028, 993, 958, 929, 905, 892, 900, 940, 1022, 
		1125, 1157, 1087, 965, 836, 783, 816, 895, 971, 1017
	},
	{	//Trumpet
		987, 1049, 1090, 1110, 1134, 1160, 1139, 1092, 1070, 1042, 1035, 1029, 1008, 1066, 1150, 1170, 1087, 915, 679, 372, 151, 
		558, 1014, 1245, 1260, 1145, 1063, 984, 934, 960, 1027, 1077, 1081, 1074, 1064, 1042, 1010, 974, 968, 974, 994, 1039, 
		1094, 1129, 1125, 1092, 1056, 1056, 1082, 1059, 1046, 1058, 1061, 1045, 1034, 1050, 1094, 1112, 1092, 1063, 1053, 1065, 1052, 992
	},
	{	//Pure Sine
		1050,1124,1196,1268,1337,1404,1467,1526,1580,1630,1674,1711,1743,1768,1786,1796,1800,1796,1786,1768,1743,1711,1674,1630,
		1580,1526,1467,1404,1337,1268,1196,1124,1050,976,904,832,763,696,633,574,520,470,426,389,357,332,314,304,300,304,314,332,
		357,389,426,470,520,574,633,696,763,832,904,976
	}
};
	
//static const unsigned short Flute[32] = 
//	{  	
//		1007,1252,1374,1548,1698,1797,1825,1797,1675,1562,1383,	
//		1219,1092,1007,913,890,833,847,810,777,744,674,	
//		598,551,509,476,495,533,589,659,758,876	
//	};  	

//static const unsigned short Horn[32] = 
//	{  	
//		1063,1082,1119,1275,1678,1748,1275,755,661,661,703,	
//		731,769,845,1039,1134,1209,1332,1465,1545,1427,1588,	
//		1370,1086,708,519,448,490,566,684,802,992	
//	};  	

/* Prototypes */

void Music_Init(void);
void Music_Play(void);	
void Music_Pause(void);
void Music_Rewind(void);
void Music_NextSong(void);
void Music_Toggle(void);
void Music_NextInst(void);
void Music_NextSong(void);
void Music_SetEnvelope(void);
