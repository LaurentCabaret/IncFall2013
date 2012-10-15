#ifndef __BASELIB__H_
#define __BASELIB__H_


//-----------------------------------------------------------------------------
// Libs
//-----------------------------------------------------------------------------
#include <C8051F330.h>
#include <string.h>
#include "I2CLib.h"
#include "UARTLIB.h"
#include "KmodGPS.h"
#include "KmodACL.h"
#include "Protocole.h"

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define  SYSCLK         24500000       // System clock frequency in Hz
#define  BAUDRATE          19200       // Baud rate of UART in bps


// 16-bit SFR declarations
sfr16    TMR3RL   = 0x92;              // Timer3 reload registers
sfr16    TMR3     = 0x94;              // Timer3 counter registers

sbit SDA = P0^2;                       // SMBus on P0.2
sbit SCL = P0^3;                       //      and P0.3



//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------

void SysInit(void);
void Timer0_Init (void);
void Timer3_Init (void);
void Port_Init (void);
void T0_Waitms (unsigned int ms);
void UART0_Init(void);

#endif //BASELIB