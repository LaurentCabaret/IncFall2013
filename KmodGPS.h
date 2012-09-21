#ifndef __KMODGPS__H_
#define __KMODGPS__H_

#include "I2CLib.h"


// Device addresses (7 bits, lsb is a don't care)
#define  KMODGPS_ADDR  0x3A           // Device address for slave target

// Communication avec le module GPS (see associated .pdf documentation)
int  getID(void);
void StopConso(void);
void StartConso(void);
void GetLatitude(void);
void GetLongitude(void);
void GetDate(void);


#endif //KMODGPS
