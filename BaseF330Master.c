//-----------------------------------------------------------------------------
// F33x_SMBus_Master_Multibyte.c
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "BaseLib.h"
#include <stdio.h>    // For sprintf usage
#include <string.h>   // For strlen usage

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

sbit LED = P1^3;                       // LED on port P1.3

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
//
// Main routine performs all configuration tasks, then loops forever sending
// and receiving SMBus data to the slave.
//

xdata char buffer[100];

void main (void)
{
  int id;
  
  SysInit();     
  
  // Reset Error Counter
  LED = 1;
  StartACC();
  id = GetAccID();
  
  while (1) {
    SendWord("Debut de la boucle\n\r",20);
    LED = 1;
    SendWord("Recuperation de l'ID\n\r",22);
    id = getID();
    if (id == 42) {
      SendWord("Id Ok\n\r",7);
      LED = 0;
    }
    else {
      SendWord("Id Error\n\r",10);
	  SendChar(id);

    }
    
    GetDate();
    GetLatitude();
    GetLongitude();
    SendCoords();
    GetAccX();
	sprintf(buffer, "Acc : (%d,%d,%d)\n\r", AccX, AccY, AccZ);
    //SendChar((AccX/256));
    //SendChar((AccY/256));
    //SendChar((AccZ/256));
	SendWord(buffer, strlen(buffer));
    
    T0_Waitms (1000);
  }
  
  
  
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------