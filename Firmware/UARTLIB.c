#include "UARTLIB.h"







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


char getChar(void)
{
   char charactere ;
   while(RI0 == 0) {};   
   charactere    = SBUF0;
   RI0 = 0;
   return charactere;
}
