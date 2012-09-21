#include "I2CLib.h"





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






//---------------------------------------------------------------
// Communication with the ACC module
//---------------------------------------------------------------
void send_gps_command(unsigned char cmd, unsigned char size, unsigned char wait) {
   // Reset Timeout flag
   SMB_TIMEOUT = 0;

   // Send command
   SMB_DATA_OUT[0] = cmd;          // Command byte
   TARGET = SLAVE_GPS_ADR;            // Target the Slave for next SMBus
                                   // transfer
   SMB_Write();                    // Initiate SMBus write

   // Read response
   if (size > 0) {
      TARGET = SLAVE_GPS_ADR;            // Target the Slave for next SMBus
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

