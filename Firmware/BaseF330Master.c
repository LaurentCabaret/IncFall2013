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


void softSetVal(char val) {
	unsigned long int i;
	unsigned char targetVal;
	targetVal = (255 - (val * 255)/100 );
	if (currentVal > targetVal) {
		while ( currentVal > targetVal ) {
		currentVal--;
		PCA0CPH0 = currentVal;
		i=0;
		while (i<2000) {
					   i++;
						}
		}
		}
	else {
		while ( currentVal < targetVal ) {
			currentVal++;
		PCA0CPH0 = currentVal;
		i=0;
		while (i<2000) {
					   i++;
						}
		}
	}
}	


void turnOff() 
{
	softSetVal(0);
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

  float IntTempVal;
  char Tab[5] = "__0__";

  SysInit();     
  pwmValue = 0;
  turnOff();

  while (1) {

	if (sw0==0) LED = 0;
	else LED = 1;


//	Tab[3] = (char)pwmValue;
//	  SendWord(Tab,5);


  }
  
  
  
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------