#ifndef __KMODGPS__H_
#define __KMODGPS__H_

#include "I2CLib.h"
#include "BaseLib.h"
#include "Protocole.h"


// Device addresses (7 bits, lsb is a don't care)
#define  KMODGPS_ADDR  0xF0           // Device address for slave target

// Communication avec le module GPS (see associated .pdf documentation)
int  getID(void);
void StopConso(void);
void StartConso(void);
void GetLatitude(void);
void GetLongitude(void);
void GetDate(void);

// Buffers used for information exchanges
extern char Date[SIZE_GETDATE];
extern char Latitude[SIZE_GETLAT];
extern char Longitude[SIZE_GETLONG];

#endif //KMODGPS
