//-----------------------------------------------------------------------------
// Firmware Centrale KeyBox INCFall2013
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

char Ligne[20];
int RxCpt = 0;
void AnalyseText();
char getID();
void setCharID(char *,char);
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
	  if (charactere == 13) {
		AnalyseText();
		RxCpt = 0;
	  }
	  else {
		  Ligne[RxCpt] = charactere;
  		  RxCpt = RxCpt + 1;
		  }
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

void AnalyseText() {
 char Result[6] = "KPYXX\r";
  	char Id = getID();
    setCharID(Result,Id);
	if ((Ligne[0] == Result[0])&&(Ligne[1] == Result[1])&&(Ligne[2] == Result[2])&&(Ligne[3] == Result[3])&&(Ligne[4] == Result[4])) 
	{
	  LED = 1;
//      SendWord(Result,6);
	}
	else 	  LED = 0;
}

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


void wait(unsigned long int time) {
	unsigned long int i = 0;
	while(i<time) {i++;}
}


void main (void)
{
  unsigned long int i,j;
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

//		SendWord(Presence,6);
	j++;
	if (j==30) {
	    SendWord(FreeKey,6);
		j=0;
		}

    
	if (BTN == 0) {
    SendWord(Presence,6);
	LED = 0;
		}

	wait(30000);

	

  }
  
  
  
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------