//-----------------------------------------------------------------------------
// Firmware ExLight
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "UARTLIB.h"

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

char FLAG = 0;
short int pwmValue= 120;
unsigned char currentVal = 255;
sbit LED = P3^3;
sbit sw0 = P1^0;
sbit sw1 = P1^1;
sbit sw2 = P1^2;
sbit sw3 = P1^3;
sbit BTN = P0^7;

//-----------------------------------------------------------------------------
// Interrupt Routine
//-----------------------------------------------------------------------------
//
// Main routine performs all configuration tasks, then :

void eventUart() interrupt 4 
{
   char charactere ;
   charactere    = SBUF0;
   if (RI0==1) {
      FLAG = 1;
	  pwmValue = charactere;
	  RI0 = 0;
	}	
}


//-----------------------------------------------------------------------------
// Outils
//-----------------------------------------------------------------------------
//
// Quelques outils de base

char getID() {
	if ((sw3==0)&&(sw2==0)&&(sw1==0)&&(sw0==0))  return 0;
	if ((sw3==0)&&(sw2==0)&&(sw1==0)&&(sw0==1))  return 1;
	if ((sw3==0)&&(sw2==0)&&(sw1==1)&&(sw0==0))  return 2;
	if ((sw3==0)&&(sw2==0)&&(sw1==1)&&(sw0==1))  return 3;
	if ((sw3==0)&&(sw2==1)&&(sw1==0)&&(sw0==0))  return 4;
	if ((sw3==0)&&(sw2==1)&&(sw1==0)&&(sw0==1))  return 5;
	if ((sw3==0)&&(sw2==1)&&(sw1==1)&&(sw0==0))  return 6;
	if ((sw3==0)&&(sw2==1)&&(sw1==1)&&(sw0==1))  return 7;
	if ((sw3==1)&&(sw2==0)&&(sw1==0)&&(sw0==0))  return 8;
	if ((sw3==1)&&(sw2==0)&&(sw1==0)&&(sw0==1))  return 9;
	if ((sw3==1)&&(sw2==0)&&(sw1==1)&&(sw0==0))  return 10;
	if ((sw3==1)&&(sw2==0)&&(sw1==1)&&(sw0==1))  return 11;
	if ((sw3==1)&&(sw2==1)&&(sw1==0)&&(sw0==0))  return 12;
	if ((sw3==1)&&(sw2==1)&&(sw1==0)&&(sw0==1))  return 13;
	if ((sw3==1)&&(sw2==1)&&(sw1==1)&&(sw0==0))  return 14;
	if ((sw3==1)&&(sw2==1)&&(sw1==1)&&(sw0==1))  return 15;



}

void setCharID(char Mot[5],char Id)
{
 switch(Id) {
 case 0 : Mot[3] = '0'; Mot[4] = '0'; break;
 case 1 : Mot[3] = '0'; Mot[4] = '1'; break;
 case 2 : Mot[3] = '0'; Mot[4] = '2'; break;
 case 3 : Mot[3] = '0'; Mot[4] = '3'; break;
 case 4 : Mot[3] = '0'; Mot[4] = '4'; break;
 case 5 : Mot[3] = '0'; Mot[4] = '5'; break;
 case 6 : Mot[3] = '0'; Mot[4] = '6'; break;
 case 7 : Mot[3] = '0'; Mot[4] = '7'; break;
 case 8 : Mot[3] = '0'; Mot[4] = '8'; break;
 case 9 : Mot[3] = '0'; Mot[4] = '9'; break;
 case 10 : Mot[3] = '1'; Mot[4] = '0'; break;
 case 11 : Mot[3] = '1'; Mot[4] = '1'; break;
 case 12 : Mot[3] = '1'; Mot[4] = '2'; break;
 default : Mot[3] = '0'; Mot[4] = '0'; break;
}

}

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
//
// Main routine performs all configuration tasks, then :


void GoToSleep() {

	  PCON |= 0x01; // set IDLE bit?
      PCON = PCON; // ... followed by a 3-cycle dummy instruction
	
}

void main (void)
{
  unsigned long int i;
  unsigned long int FLAG = 0;
  char Id;
  float IntTempVal;
  char FreeKey[6] = "CF_XX\r";
  char Presence[6] = "CP_XX\r";

  SysInit();     
  pwmValue = 0;

  while (1) {
	Id = getID();
    setCharID(FreeKey,Id);
    setCharID(Presence,Id);

	
    SendWord(FreeKey,6);
		LED = 0;
    
	if (BTN == 1) {
		SendWord(Presence,6);
		LED = 1;
		}

	
	while(i<30000) {i++;}
	i=0;

  }
  
  
  
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------