#include "I2CLib.h"
#include "UARTLIB.h"


//---------------------------------------------------------------
// Initialisation globale du système
//---------------------------------------------------------------
void SysInit(void)
{

  PCA0MD = 0x00; 	//pour éteindre le Watchdog	
  PCA0MD = 0x02; 
  
  OSCICN = 0x83;
  // setting of 24500000
 // REF0CN = 0x08;
  Port_Init();                        // Initialize Crossbar and GPIO  
//  Timer0_Init();                      // Configure Timer1 for use 
				      // with SMBus baud rate  
//  Timer3_Init ();                     // Configure Timer3 for use with
				      // SCL low timeout detect  
//  SMBus_Init ();                      // Configure and enable SMBus
  // If slave is holding SDA low because of an improper SMBus reset or error
  UART0_Init();
//  EIE1 |= 0x01;                       // Enable the SMBus interrupt
  PCA0CPM0 = 0x42;
  PCA0CPH0 = 120;
  PCA0CN = 0x40;
  EA = 1;                             // Global interrupt enable
   
}

//-----------------------------------------------------------------------------
// Timer0_Init()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Timer1 configured as the SMBus clock source as follows:
// - Timer1 in 8-bit auto-reload mode
// - SYSCLK or SYSCLK / 4 as Timer1 clock source
// - Timer1 overflow rate => 3 * SMB_FREQUENCY
// - The resulting SCL clock rate will be ~1/3 the Timer1 overflow rate
// - Timer1 enabled
//
void Timer0_Init (void)
{
  
  // Make sure the Timer can produce the appropriate frequency in 8-bit mode
  // Supported SMBus Frequencies range from 10kHz to 100kHz.  The CKCON register
  // settings may need to change for frequencies outside this range.
  #if ((SYSCLK/SMB_FREQUENCY/3) < 255)
  #define SCALE 1
  CKCON |= 0x04;                   // Timer0 clock source = SYSCLK
  #elif ((SYSCLK/SMB_FREQUENCY/4/3) < 255)
  #define SCALE 4
  CKCON |= 0x01;
  CKCON &= ~0x0A;                  // Timer1/0 clock source = SYSCLK / 4
  #endif
  
  TMOD = 0x02;                        // Timer0 in 8-bit auto-reload mode
  
  // Timer1 configured to overflow at 1/3 the rate defined by SMB_FREQUENCY
  TH0 = -(SYSCLK/SMB_FREQUENCY/SCALE/3);
  
  TL0 = TH0;                          // Init Timer1
  
  TR0 = 1;                            // Timer1 enabled
}

/*//-----------------------------------------------------------------------------
// Timer3_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Timer3 configured for use by the SMBus low timeout detect feature as
// follows:
// - Timer3 in 16-bit auto-reload mode
// - SYSCLK/12 as Timer3 clock source
// - Timer3 reload registers loaded for a 25ms overflow period
// - Timer3 pre-loaded to overflow after 25ms
// - Timer3 enabled
//
void Timer3_Init (void)
{
  TMR3CN = 0x00;                      // Timer3 configured for 16-bit auto-
				      // reload, low-byte interrupt disabled  
  CKCON &= ~0x40;                     // Timer3 uses SYSCLK/12
  TMR3RL = 0;	;           	      // Timer3 configured to overflow after ???
  TMR3 = TMR3RL;                         
  EIE1 |= 0x80;                       // Timer3 interrupt enable  
}*/

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the Crossbar and GPIO ports.
//
// P0.0   digital   open-drain    SMBus SDA
// P0.1   digital   open-drain    SMBus SCL
//
// P1.3   digital   push-pull     LED
//
// all other port pins unused
//
// Note: If the SMBus is moved, the SCL and SDA sbit declarations must also
// be adjusted.
//
void PORT_Init (void)
{
  P0MDOUT = 0x01;                     // All P0 pins open-drain output exept P0.6 for PWM
  P3MDOUT = 0x08;                     // All P0 pins open-drain output exept P0.6 for PWM
  //P0SKIP  = 0x00;                     // Déplace le bus I2C en P0.2 et P0.3
  P1MDIN = 0x0F;
  XBR0 = 0x01;                        // Enable SMBus pins & UART
  XBR1 = 0x40;                        // Enable crossbar and weak pull-ups and PWM

  //P0 = 0xFF;
}


//-----------------------------------------------------------------------------
// T0_Waitms
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) unsigned char ms - number of milliseconds to wait
//                        range is full range of character: 0 to 255
//
// Configure Timer0 to wait for <ms> milliseconds using SYSCLK as its time
// base.
//
void T0_Waitms (unsigned char ms)
{
  TCON &= ~0x30;                      // Stop Timer0; Clear TF0
  TMOD &= ~0x0f;                      // 16-bit free run mode
  TMOD |=  0x01;
  
  CKCON |= 0x04;                      // Timer0 counts SYSCLKs
  
  while (ms) {
    TR0 = 0;                         // Stop Timer0
    TH0 = -(SYSCLK/1000 >> 8);       // Overflow in 1ms
    TL0 = -(SYSCLK/1000);
    TF0 = 0;                         // Clear overflow indicator
    TR0 = 1;                         // Start Timer0
    while (!TF0);                    // Wait for overflow
    ms--;                            // Update ms counter
  }
  
  TR0 = 0;                            // Stop Timer0
}


//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART0 using Timer1, for <BAUDRATE> and 8-N-1.
//-----------------------------------------------------------------------------
void UART0_Init (void)
{
   SCON0 = 0x10;                       // SCON0: 8-bit variable bit rate
                                       //        level of STOP bit is ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits
   if (SYSCLK/BAUDRATE/2/256 < 1) {
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   } else if (SYSCLK/BAUDRATE/2/256 < 4) {
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
      CKCON |=  0x01;
   } else if (SYSCLK/BAUDRATE/2/256 < 12) {
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   } else {
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
   }

   TL1 = TH1;                          // init Timer1
   TMOD &= ~0xf0;                      // TMOD: timer 1 in 8-bit autoreload
   TMOD |=  0x20;
   TR1 = 1;                            // START Timer1

   IP |= 0x10;                         // Make UART high priority
   ES0 = 1;                            // Enable UART0 interrupts

}

