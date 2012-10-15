#include "I2CLib.h"
#include "KmodACL.h"

float AccX,AccY,AccZ;
int AccXi,AccYi,AccZi;

void GetAcc(void) {
   // Reset Timeout flag
   SMB_TIMEOUT = 0;

   // Send command
   SMB_DATA_OUT[0] = 0x32;          // Command byte
   TARGET = KMODACL_ADDR;            // Target the Slave for next SMBus
                                   // transfer
   SMB_Write();                    // Initiate SMBus write

   // Read response
      TARGET = KMODACL_ADDR;            // Target the Slave for next SMBus
                                      // transfer
      ByteRequested = 6;           // Size of the answer
      T0_Waitms (10);               // Wait for the request to be processed
      SMB_Read();                     // Read the answer
   
   

   AccXi = SMB_DATA_IN[0] + ((int)SMB_DATA_IN[1])*256;
   AccYi = SMB_DATA_IN[2] + ((int)SMB_DATA_IN[3])*256;
   AccZi = SMB_DATA_IN[4] + ((int)SMB_DATA_IN[5])*256;

   AccX = AccXi/2048.;
   AccY = AccYi/2048.;
   AccZ = AccZi/2048.;

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

void send_acc_command_to_specific_register(unsigned char theregister,unsigned char cmd) {
   // Reset Timeout flag
   SMB_TIMEOUT = 0;

   // Send command
   SMB_DATA_OUT[0] = theregister;          // Command byte
   SMB_DATA_OUT[1] = cmd;          // Command byte
   TARGET = KMODACL_ADDR;            // Target the Slave for next SMBus
                                   // transfer
   SMB_Write();                    // Initiate SMBus write
}
