#ifndef __UARTLIB__H_
#define __UARTLIB__H_

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#include "BaseLib.h"


//Communication avec le portable
void SendChar(char);
void SendLigneImage(void);
void SendWord(char Mot[],unsigned int Size);
void SendCoords(void);


#endif //UARTLIB