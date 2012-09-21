#ifndef __I2CLIB__H_
#define __I2CLIB__H_

#include "BaseLib.h"

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define  SYSCLK         24500000       // System clock frequency in Hz

#define  SMB_FREQUENCY  10000          // Target SCL clock rate
                                       // This example supports between 10kHz
                                       // and 100kHz (maybe 400kHz)

// Device addresses (7 bits, lsb is a don't care)
#define  SLAVE_ADDR_WR  0x3A           // Device address for slave target

#define  WRITE          0x00           // SMBus WRITE command
#define  READ           0x01           // SMBus READ command

// Status vector - top 4 bits only
#define  SMB_MTSTA      0xE0           // (MT) start transmitted
#define  SMB_MTDB       0xC0           // (MT) data byte transmitted
#define  SMB_MRDB       0x80           // (MR) data byte received
// End status vector definition

#define  NUM_BYTES_WR   2              // Number of bytes to write
                                       // Master -> Slave
#define  NUM_BYTES_RD   12            // Max Number of bytes to read
                                       // Master <- Slave



//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

// Counter for the number of communication error with the salve module (SMBus)
extern unsigned long NUM_ERRORS;
// Software flag to indicate timeout during the last SMBus communication
extern bit SMB_TIMEOUT;


sbit SDA = P0^2;                       // SMBus on P0.0
sbit SCL = P0^3;                       // and P0.1


//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

// Initialization functions
void SMBus_Init (void);


// Interruption routines
void SMBus_ISR (void);

// Helpers

void Timer3_ISR (void);

// Communication with module (see associated .pdf documentation)
int  getID(void);
void StopConso(void);
void StartConso(void);
void GetLatitude(void);
void GetLongitude(void);
void GetLigneImage(void);
void GetDate(void);
void TakePicture(void);

void StartACC(void);
unsigned char GetAccID(void);
void GetAccX(void);


// Helpers
void SMB_Write (void);
void SMB_Read (void);
void send_acc_command(unsigned char cmd, unsigned char size, unsigned char wait);
void send_acc_command_to_specific_register(unsigned char theregister,unsigned char cmd);
#endif //I2CLIB
