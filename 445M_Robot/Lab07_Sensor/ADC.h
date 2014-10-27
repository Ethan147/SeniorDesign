// ADC.h
// Runs on LM4F120
// Provides functions to access ADC and sample. Uses code from Daniel Valvano's
// ADCT0ATrigger.c to do timeinterrupted samples.
// Victor Vo and Alex Taft
// January 28, 2014

extern unsigned short ADC_SampleBuffer[];

//------------------ADC_EnableTimer------------------
//Enables Timer0A to be associated with the ADC to a frequency specified
//by the user. For the purpose of this lab it will most likely be 10 kHz.
//Input: ft Frequency desired for the timer.
void ADC_EnableTimer(int ft);

//------------------ADC_Open-------------------------
//Configures a sampling sequence for sampling via software triggers.
//Call ADC_In to sample the channel.
//Input:  module      the ADC module to enable (0 or 1)
//        ssNum       the sample sequencer to configure (0 to 3)
//        sequence    sequence sample, given in hex, eg. 0x0030ABC1
//        seqLen      how many channels to sample (including duplicates)
//Output: -1 if open failed, 1 otherwise
int ADC_Open(short module, short ssNum, int sequence, short seqLen);

//----------------------ADC_In-----------------------
//Samples the ADC once using software triggers and returns it
//Input:  module the module to smaple from
//        ssNum  the sample sequencer to sample from
//Output: ADC sample from the channel requested
int ADC_In(short module, short ssNum);//, unsigned int *outputBuffer);

//-------------------ADC_Collect----------------------
//Initiates a hardware triggered ADC sampling and stores the
//measured values in the buffer passed. Monitor the progress
//of this method using ADC_Status.
//Input: module          the ADC module to use (0 or 1)
//       ssNum           the sample sequencer to use (0 to 3)
//       sequence        the order to sample channels, e.g. 0x0023
//       seqLen          how many channels are being sampled
//       fs              sampling frequency (ft >= fs > 0)
//       buffer          array of where to store sampled values
//       numberOfSamples how many samples to take before workng on data
int ADC_Collect(short module, short ssNum, int sequence, short seqLen,
	unsigned int fs, void(*producerTask)(unsigned long));
//unsigned short buffer[], unsigned int numberOfSamples);

//---------------------ADC_Status----------------------
// Returns the current state of the ADC.
// Output: ADC status flag
int ADC_Status(int module, int sequence);
