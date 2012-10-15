#include "I2CLib.h"
#include <string.h>
#include "KmodMem.h"

#define MEM_SIZE 128
xdata unsigned char Memory[MEM_SIZE];
//---------------------------------------------------------------
// Communication with the Mem module
//---------------------------------------------------------------

void InitMem(void) {
	int i;
	for (i=0; i < MEM_SIZE; i++)
		Memory[i] = 'x';
}

void WriteData(void * Data,unsigned char N,unsigned int Adresse)
{
	if (Adresse>=MEM_SIZE) {
	} else {
		if (Adresse+N>MEM_SIZE) N=MEM_SIZE-Adresse;
		memcpy(&Memory[Adresse],Data,N);
	}
}

void ReadData(void * Data,unsigned char N,unsigned int Adresse)
{
	memcpy(Data,&Memory[Adresse],N);
}