#ifndef __I2CLIB__H_
#define __I2CLIB__H_

#include "BaseLib.h"

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
#define  SMB_FREQUENCY  10000          // Target SCL clock rate
                                       // This example supports between 10kHz and 100kHz 

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

// Global holder for SMBus data
// All receive data is written here
extern xdata unsigned char SMB_DATA_IN[NUM_BYTES_RD];

// Global holder for SMBus data.
// All transmit data is read from here
extern xdata unsigned char SMB_DATA_OUT[NUM_BYTES_WR];

extern unsigned char TARGET;                  // Target SMBus slave address

extern bit SMB_BUSY;                          // Software flag to indicate when the
                                       // SMB_Read() or SMB_Write() functions
                                       // have claimed the SMBus

extern bit SMB_RW;                            // Software flag to indicate the
                                       // direction of the current transfer
//bit SMB_TIMEOUT;                       // Software flag to indicate SMBus
                                       // communication timeout

//unsigned long NUM_ERRORS = 0;          // Counter for the number of errors.

extern unsigned int ByteRequested;

// Counter for the number of communication error with the salve module (SMBus)
extern unsigned long NUM_ERRORS;
// Software flag to indicate timeout during the last SMBus communication
extern bit SMB_TIMEOUT;



//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

// Initialization functions
void SMBus_Init (void);

// Interruption routines
void SMBus_ISR (void);

// Helpers
void Timer3_ISR (void);
void SMB_Write (void);
void SMB_Read (void);

#endif //I2CLIB
