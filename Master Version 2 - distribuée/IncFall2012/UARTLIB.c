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

void SendCoords(void)
{
	SendWord("<LAT>",5);
	SendWord(Latitude,SIZE_GETLAT);
	SendWord("<LON>",5);
	SendWord(Longitude,SIZE_GETLONG);
	SendWord("\n\n\n",3);
}