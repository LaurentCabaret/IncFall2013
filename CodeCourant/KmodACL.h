#ifndef __KMODACL__H_
#define __KMODACL__H_

#include "I2CLib.h"
#include "BaseLib.h"
#include "Protocole.h"


// Device addresses (7 bits, lsb is a don't care)
#define  KMODACL_ADDR  0x3A           // Device address for slave target

void StartACC(void);
unsigned char GetAccID(void);
void GetAcc(void);
void send_acc_command(unsigned char cmd, unsigned char size, unsigned char wait);
void send_acc_command_to_specific_register(unsigned char theregister,unsigned char cmd);

// Buffers used for information exchanges
extern float AccX, AccY, AccZ;

#endif //I2CLIB
