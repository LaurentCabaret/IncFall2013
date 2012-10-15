#ifndef __KMODMEM__H_
#define __KMODMEM__H_

//#include "I2CLib.h"
#include "BaseLib.h"
#include "Protocole.h"


// Device addresses (7 bits, lsb is a don't care)
//#define  KMODMEM_ADDR  0xE0           // Device address for slave target

void InitMem(void);

void WriteData(void * Data,unsigned char N,unsigned int Adresse);

void ReadData(void * Data,unsigned char N,unsigned int Adresse);

// Buffers used for information exchanges

#endif //KMODMEM
