#ifndef __UARTLIB__H_
#define __UARTLIB__H_

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#include "BaseLib.h"


//Communication avec le portable
void SendChar(char);
void SendWord(char Mot[],unsigned int Size);
char getChar(void);


#endif //UARTLIB