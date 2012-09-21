#ifndef __UTILS__H_
#define __UTILS__H_

#include <C8051F330.h>
#include "Protocole.h"

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define  SYSCLK         24500000       // System clock frequency in Hz

#define  SMB_FREQUENCY  10000          // Target SCL clock rate
                                       // This example supports between 10kHz
                                       // and 100kHz (maybe 400kHz)

// Device addresses (7 bits, lsb is a don't care)
#define  SLAVE_ADDR_WR     0x3A           // Device address for slave target
#define  SLAVE_ADDR_RD     0x3B           // Device address for slave target

#define BAUDRATE             19200           // Baud rate of UART in bps

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

// Counter for the number of communication error with the salve module (SMBus)
extern unsigned long NUM_ERRORS;
// Software flag to indicate timeout during the last SMBus communication
extern bit SMB_TIMEOUT;

// Buffers for data from the slave module
extern char Date[SIZE_GETDATE];
extern char Latitude[SIZE_GETLAT];
extern char Longitude[SIZE_GETLONG];
extern int AccX,AccY,AccZ;

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

// Initialization function
void SysInit(void);

// Helpers
void T0_Waitms (unsigned char ms);

// Communication with module (see associated .pdf documentation)
int getID(void);
void StopConso(void);
void StartConso(void);
void GetLatitude(void);
void GetLongitude(void);
void GetLigneImage(void);
void GetDate(void);
void TakePicture(void);


//Communication avec le portable
void SendChar(char);
void SendLigneImage(void);
void SendWord(char Mot[],unsigned int Size);
void SendCoords(void);
void StartACC(void);
unsigned char GetAccID(void);
void GetAccX(void);

#endif