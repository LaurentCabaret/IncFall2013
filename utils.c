#include "utils.h"
#include <string.h>

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
#define  WRITE          0x00           // SMBus WRITE command
#define  READ           0x01           // SMBus READ command

// Status vector - top 4 bits only
#define  SMB_MTSTA      0xE0           // (MT) start transmitted
#define  SMB_MTDB       0xC0           // (MT) data byte transmitted
#define  SMB_MRDB       0x80           // (MR) data byte received
// End status vector definition

#define  NUM_BYTES_WR   2              // Number of bytes to write
                                       // Master -> Slave
#define  NUM_BYTES_RD   32            // Max Number of bytes to read
                                       // Master <- Slave
#define SYSTEMCLOCK 25000000
//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

// Initialization functions
void SMBus_Init (void);
void Timer0_Init (void);
void Timer3_Init (void);
void Port_Init (void);
void UART0_Init(void);

// Interruption routines
void SMBus_ISR (void);
void Timer3_ISR (void);

// Helpers
void SMB_Write (void);
void SMB_Read (void);
void send_acc_command(unsigned char cmd, unsigned char size, unsigned char wait);
void send_acc_command_to_specific_register(unsigned char theregister,unsigned char cmd);

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

// Global holder for SMBus data
// All receive data is written here
xdata unsigned char SMB_DATA_IN[NUM_BYTES_RD];

// Global holder for SMBus data.
// All transmit data is read from here
xdata unsigned char SMB_DATA_OUT[NUM_BYTES_WR];

unsigned char TARGET;                  // Target SMBus slave address

bit SMB_BUSY;                          // Software flag to indicate when the
                                       // SMB_Read() or SMB_Write() functions
                                       // have claimed the SMBus

bit SMB_RW;                            // Software flag to indicate the
                                       // direction of the current transfer
bit SMB_TIMEOUT;                       // Software flag to indicate SMBus
                                       // communication timeout

unsigned long NUM_ERRORS = 0;          // Counter for the number of errors.

unsigned int ByteRequested;
// 16-bit SFR declarations
sfr16    TMR3RL   = 0x92;              // Timer3 reload registers
sfr16    TMR3     = 0x94;              // Timer3 counter registers

sbit SDA = P0^2;                       // SMBus on P0.0
sbit SCL = P0^3;                       // and P0.1

char Latitude[SIZE_GETLAT];
char Longitude[SIZE_GETLONG];
char Date[SIZE_GETDATE];
int AccX,AccY,AccZ;


//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMBus_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// SMBus configured as follows:
// - SMBus enabled
// - Slave mode inhibited
// - Timer1 used as clock source. The maximum SCL frequency will be
//   approximately 1/3 the Timer1 overflow rate
// - Setup and hold time extensions enabled
// - Bus Free and SCL Low timeout detection enabled
//
void SMBus_Init (void)
{
   SMB0CF = 0x55;                      // Use Timer1 overflows as SMBus clock
                                       // source;
                                       // Disable slave mode;
                                       // Enable setup & hold time
                                       // extensions;
                                       // Enable SMBus Free timeout detect;

   SMB0CF |= 0x80;                     // Enable SMBus;
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


//-----------------------------------------------------------------------------
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

   TMR3RL = 0;//-(SYSCLK/12/1);           // Timer3 configured to overflow after
   TMR3 = TMR3RL;                      // ~25ms (for SMBus low timeout detect):
                                       // 1/.025 = 40

   EIE1 |= 0x80;                       // Timer3 interrupt enable
   //TMR3CN |= 0x04;                     // Start Timer3
}

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
   P0MDOUT = 0x00;                     // All P0 pins open-drain output exept P0.6 for PWM
   P0SKIP  = 0x03;                     // Déplace le bus I2C en P0.2 et P0.3
   P1MDOUT |= 0x08;                    // Make the LED (P1.3) a push-pull
                                       // output

   XBR0 = 0x05;                        // Enable SMBus pins & UART
   XBR1 = 0x41;                        // Enable crossbar and weak pull-ups

   P0 = 0xFF;
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMBus Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// SMBus ISR state machine
// - Master only implementation - no slave or arbitration states defined
// - All incoming data is written to global variable array <SMB_DATA_IN>
// - All outgoing data is read from global variable array <SMB_DATA_OUT>
//
void SMBus_ISR (void) interrupt 7
{
   bit FAIL = 0;                       // Used by the ISR to flag failed
                                       // transfers

   static unsigned int sent_byte_counter;
   static unsigned int rec_byte_counter;

   if (ARBLOST == 0)                   // Check for errors
   {
      // Normal operation
      switch (SMB0CN & 0xF0)           // Status vector
      {
         // Master Transmitter/Receiver: START condition transmitted.
         case SMB_MTSTA:
            SMB0DAT = TARGET;          // Load address of the target slave
            SMB0DAT &= 0xFE;           // Clear the LSB of the address for the
                                       // R/W bit
            SMB0DAT |= SMB_RW;         // Load R/W bit
            STA = 0;                   // Manually clear START bit
            rec_byte_counter = 1;      // Reset the counter
            sent_byte_counter = 1;     // Reset the counter
            break;

         // Master Transmitter: Data byte transmitted
         case SMB_MTDB:
            if (ACK)                   // Slave ACK?
            {
               if (SMB_RW == WRITE)    // If this transfer is a WRITE,
               {
                  if (sent_byte_counter <= NUM_BYTES_WR)
                  {
                     // send data byte
                     SMB0DAT = SMB_DATA_OUT[sent_byte_counter-1];
                     sent_byte_counter++;
                  }
                  else
                  {
                     STO = 1;          // Set STO to terminate transfer
                     SMB_BUSY = 0;     // And free SMBus interface
                  }
               }
               else {}                 // If this transfer is a READ,
                                       // proceed with transfer without
                                       // writing to SMB0DAT (switch
                                       // to receive mode)


            }
            else                       // If slave NACK,
            {
               STO = 1;                // Send STOP condition, followed
               STA = 1;                // By a START
               NUM_ERRORS++;           // Indicate error
            }
            break;

         // Master Receiver: byte received
         case SMB_MRDB:
            if (rec_byte_counter < ByteRequested)
            {
               SMB_DATA_IN[rec_byte_counter-1] = SMB0DAT; // Store received
                                                         // byte
               ACK = 1;                 // Send ACK to indicate byte received
               rec_byte_counter++;      // Increment the byte counter
            }
            else
            {
               SMB_DATA_IN[rec_byte_counter-1] = SMB0DAT; // Store received
                                                         // byte
               SMB_BUSY = 0;            // Free SMBus interface
               ACK = 0;                 // Send NACK to indicate last byte
                                       // of this transfer

               STO = 1;                 // Send STOP to terminate transfer
			   TMR3CN &= 0xF9;          // Stop timeout timer
            }
            break;

         default:
            FAIL = 1;                  // Indicate failed transfer
                                       // and handle at end of ISR
            break;

      } // end switch
   }
   else
   {
      // ARBLOST = 1, error occurred... abort transmission
      FAIL = 1;
   } // end ARBLOST if

   if (FAIL)                           // If the transfer failed,
   {
      SMB0CF &= ~0x80;                 // Reset communication
      SMB0CF |= 0x80;
      STA = 0;
      STO = 0;
      ACK = 0;

      SMB_BUSY = 0;                    // Free SMBus

      FAIL = 0;
//      LED = 0;

      NUM_ERRORS++;                    // Indicate an error occurred
   }

   SI = 0;                             // Clear interrupt flag
}

//-----------------------------------------------------------------------------
// Timer3 Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// A Timer3 interrupt indicates an SMBus SCL low timeout.
// The SMBus is disabled and re-enabled here
//
void Timer3_ISR (void) interrupt 14
{
   SMB0CF &= ~0x80;                    // Disable SMBus
   SMB0CF |= 0x80;                     // Re-enable SMBus
   TMR3CN &= ~0x80;                    // Clear Timer3 interrupt-pending
                                       // flag
   STA = 0;
   SMB_BUSY = 0;                       // Free SMBus
   SMB_TIMEOUT = 1;
}

//-----------------------------------------------------------------------------
// Support Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMB_Write
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Writes a single byte to the slave with address specified by the <TARGET>
// variable.
// Calling sequence:
// 1) Write target slave address to the <TARGET> variable
// 2) Write outgoing data to the <SMB_DATA_OUT> variable array
// 3) Call SMB_Write()
//
void SMB_Write (void)
{
   while (SMB_BUSY);                   // Wait for SMBus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)
   SMB_RW = 0;                         // Mark this transfer as a WRITE
   STA = 1;                            // Start transfer

   //TMR3 = TMR3RL;                      // Set Timeout counter to ~25ms
   //TMR3CN |= 0x04;                     // Start Timeout counter
}

//-----------------------------------------------------------------------------
// SMB_Read
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Reads a single byte from the slave with address specified by the <TARGET>
// variable.
// Calling sequence:
// 1) Write target slave address to the <TARGET> variable
// 2) Call SMB_Write()
// 3) Read input data from <SMB_DATA_IN> variable array
//
void SMB_Read (void)
{
   while (SMB_BUSY);                   // Wait for bus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)
   SMB_RW = 1;                         // Mark this transfer as a READ

   STA = 1;                            // Start transfer

   //TMR3 = TMR3RL;                      // Set Timeout counter to ~25ms
   //TMR3CN |= 0x04;                     // Start Timeout counter

   while (SMB_BUSY);                   // Wait for transfer to complete
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
   if (SYSTEMCLOCK/BAUDRATE/2/256 < 1) {
      TH1 = -(SYSTEMCLOCK/BAUDRATE/2);
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   } else if (SYSTEMCLOCK/BAUDRATE/2/256 < 4) {
      TH1 = -(SYSTEMCLOCK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
      CKCON |=  0x01;
   } else if (SYSTEMCLOCK/BAUDRATE/2/256 < 12) {
      TH1 = -(SYSTEMCLOCK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   } else {
      TH1 = -(SYSTEMCLOCK/BAUDRATE/2/48);
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

//---------------------------------------------------------------
// Communication with the ACC module
//---------------------------------------------------------------
void send_gps_command(unsigned char cmd, unsigned char size, unsigned char wait) {
   // Reset Timeout flag
   SMB_TIMEOUT = 0;

   // Send command
   SMB_DATA_OUT[0] = cmd;          // Command byte
   TARGET = KMODACL_ADDR;            // Target the Slave for next SMBus
                                   // transfer
   SMB_Write();                    // Initiate SMBus write

   // Read response
   if (size > 0) {
      TARGET = KMODACL_ADDR;            // Target the Slave for next SMBus
                                      // transfer
      ByteRequested = size;           // Size of the answer
      T0_Waitms (wait);               // Wait for the request to be processed
      SMB_Read();                     // Read the answer
   }
}

int GetID(void) {
   send_gps_command(CMD_GETID, SIZE_GETID, 1);
   if (SMB_TIMEOUT)
      return -1;
   else
      return SMB_DATA_IN[0];
}

void GetLatitude(void) {
   send_gps_command(CMD_GETLAT, SIZE_GETLAT, 10);
   memcpy(Latitude, SMB_DATA_IN, SIZE_GETLAT);
}
void GetLongitude(void) {
   send_gps_command(CMD_GETLONG, SIZE_GETLONG, 10);
   memcpy(Longitude, SMB_DATA_IN, SIZE_GETLAT);
}
void GetDate(void) {
   send_gps_command(CMD_GETDATE, SIZE_GETDATE, 10);
   memcpy(Date, SMB_DATA_IN, SIZE_GETDATE);
}

void GetAccX(void) {
   // Reset Timeout flag
   SMB_TIMEOUT = 0;

   // Send command
   SMB_DATA_OUT[0] = 0x32;          // Command byte
   TARGET = SLAVE_ACC_ADR;            // Target the Slave for next SMBus
                                   // transfer
   SMB_Write();                    // Initiate SMBus write

   // Read response
      TARGET = SLAVE_ACC_ADR;            // Target the Slave for next SMBus
                                      // transfer
      ByteRequested = 6;           // Size of the answer
      T0_Waitms (10);               // Wait for the request to be processed
      SMB_Read();                     // Read the answer
   
   
   AccX = SMB_DATA_IN[0] + (int)SMB_DATA_IN[1]<<8;
   AccY = SMB_DATA_IN[2] + (int)SMB_DATA_IN[3]<<8;
   AccZ = SMB_DATA_IN[4] + (int)SMB_DATA_IN[5]<<8;
}

//---------------------------------------------------------------
// Initialisation gloable du système
//---------------------------------------------------------------
void SysInit(void)
{
   unsigned char i;                    // Dummy variable counters
   unsigned int k;

   PCA0MD = 0x00; 	//pour éteindre le Watchdog

   OSCICN |= 0x03;                     // Set internal oscillator to highest
                                       // setting of 24500000

   while(!SDA)
   {
      // Provide clock pulses to allow the slave to advance out
      // of its current state. This will allow it to release SDA.
      XBR1 = 0x40;                     // Enable Crossbar
      SCL = 0;                         // Drive the clock low
      for(i = 0; i < 255; i++);        // Hold the clock low
      SCL = 1;                         // Release the clock
      while(!SCL);                     // Wait for open-drain
                                       // clock output to rise
      for(i = 0; i < 10; i++);         // Hold the clock high
      XBR1 = 0x00;                     // Disable Crossbar
   }

   Port_Init();                        // Initialize Crossbar and GPIO


   Timer0_Init();                      // Configure Timer1 for use
                                       // with SMBus baud rate

   Timer3_Init ();                     // Configure Timer3 for use with
                                       // SCL low timeout detect

   SMBus_Init ();                      // Configure and enable SMBus

   // If slave is holding SDA low because of an improper SMBus reset or error


   UART0_Init();

   EIE1 |= 0x01;                       // Enable the SMBus interrupt

//   LED = 0;

   EA = 1;                             // Global interrupt enable

   // Initialize buffers
   for (k=0; k < SIZE_GETDATE; k++)
      Date[k] = " ";
   Date[SIZE_GETDATE-1] = '\0';

   for (k=0; k < SIZE_GETLAT; k++)
      Latitude[k] = " ";
   Latitude[SIZE_GETLAT-1] = '\0';

   for (k=0; k < SIZE_GETLONG; k++)
      Longitude[k] = " ";
   Longitude[SIZE_GETLONG-1] = '\0';

}


void SendChar(char charactere)
{
   SBUF0 = charactere;
   while(TI0 == 0) {};
   TI0 = 0;
}

void SendWord(char Mot[],unsigned int Size)
{
   unsigned int i = 0;
   while(i<Size) {
	   SendChar(Mot[i]);
	   i++;
	   }
}

void SendCoords(void)
{
	SendWord("<LAT>",5);
	SendWord(Latitude,SIZE_GETLAT);
	SendWord("<LON>",5);
	SendWord(Longitude,SIZE_GETLONG);
	SendWord("\n\n\n",3);
}

void StartACC(void)
{   
   send_acc_command_to_specific_register(0x38,0x00);
   send_acc_command_to_specific_register(0x31,0x0F);
   send_acc_command_to_specific_register(0x2C,0x04);
   send_acc_command_to_specific_register(0x2D,0x00);
   send_acc_command_to_specific_register(0x2D,0x10);
   send_acc_command_to_specific_register(0x2D,0x08);
}

unsigned char GetAccID(void)
{
   send_acc_command(0x00, 1, 10);
   return SMB_DATA_IN[0];

}

//---------------------------------------------------------------
// Communication with the ACC module
//---------------------------------------------------------------
void send_acc_command(unsigned char cmd, unsigned char size, unsigned char wait) {
   // Reset Timeout flag
   SMB_TIMEOUT = 0;

   // Send command
   SMB_DATA_OUT[0] = cmd;          // Command byte
   TARGET = SLAVE_ACC_ADR;            // Target the Slave for next SMBus
                                   // transfer
   SMB_Write();                    // Initiate SMBus write


   // Read response
   if (size > 0) {
      TARGET = SLAVE_ACC_ADR;            // Target the Slave for next SMBus
                                      // transfer
      ByteRequested = size;           // Size of the answer
      T0_Waitms (wait);               // Wait for the request to be processed
      SMB_Read();                     // Read the answer
   }
}

void send_acc_command_to_specific_register(unsigned char theregister,unsigned char cmd) {
   // Reset Timeout flag
   SMB_TIMEOUT = 0;

   // Send command
   SMB_DATA_OUT[0] = theregister;          // Command byte
   SMB_DATA_OUT[1] = cmd;          // Command byte
   TARGET = SLAVE_ACC_ADR;            // Target the Slave for next SMBus
                                   // transfer
   SMB_Write();                    // Initiate SMBus write
}

