#define AddMacq(NAME, SIZE, TYPE)                               \
TYPE NAME ## Macq[SIZE];                                        \
int NAME ## MacqStartI;                                         \
int NAME ## MacqSize;                                           \
void NAME ## MacqInit(void){                                     \
	NAME ## MacqStartI = 0;                                     \
	NAME ## MacqSize = 0;                                       \
}                                                               \
void NAME ## MacqAdd(TYPE data){                                 \
	int index;                                                  \
	index = ( NAME ## MacqStartI + NAME ## MacqSize ) % SIZE;   \
	NAME ## Macq[index] = data;                                 \
	if ( NAME ## MacqSize < SIZE ){                             \
		NAME ## MacqSize++;                                     \
	} else {                                                    \
		NAME ## MacqStartI = ( NAME ## MacqStartI + 1 ) % SIZE; \
	}                                                         \
}                                                           \
void NAME ## MacqClear(void){                               \
	int i;                                                    \
	NAME ## MacqInit();                                       \
	for (i = 0; i < SIZE; i++){                               \
		NAME ## Macq[i] = 0;                                    \
	}                                                         \
}
