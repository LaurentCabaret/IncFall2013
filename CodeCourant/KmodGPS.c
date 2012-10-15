#include "I2CLib.h"
#include "KmodGPS.h"

char Date[SIZE_GETDATE];


//---------------------------------------------------------------
// Communication with the GPS module
//---------------------------------------------------------------
void send_gps_command(unsigned char cmd, unsigned char size, unsigned char wait) {
   // Reset Timeout flag
   SMB_TIMEOUT = 0;

   // Send command
   SMB_DATA_OUT[0] = cmd;          // Command byte
   TARGET = KMODGPS_ADDR;            // Target the Slave for next SMBus
                                   // transfer
   SMB_Write();                    // Initiate SMBus write

   // Read response
   if (size > 0) {
      TARGET = KMODGPS_ADDR;            // Target the Slave for next SMBus
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

float GetLatitude(void) {
   Datas Latitude;
   send_gps_command(CMD_GETLAT, SIZE_GETLAT, 10);
   memcpy(Latitude.AccesOctets, SMB_DATA_IN, SIZE_GETLAT);
   return Latitude.AccesFlottants.Latitude;
}

float GetLongitude(void) {
   Datas Longitude;
   send_gps_command(CMD_GETLONG, SIZE_GETLONG, 10);
   memcpy(Longitude.AccesOctets+4, SMB_DATA_IN, SIZE_GETLAT);
   return Longitude.AccesFlottants.Longitude;
}
void GetDate(void) {
   send_gps_command(CMD_GETDATE, SIZE_GETDATE, 10);
   memcpy(Date, SMB_DATA_IN, SIZE_GETDATE);
}
