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



void TestMem(void);
//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
//
// Main routine performs all configuration tasks, then loops forever sending
// and receiving SMBus data to the slave.
//

xdata char buffer[100];
extern xdata unsigned char Memory[128];

void main (void)
{
  int id;
  float Latitude,Longitude;
  
  SysInit();     
  
  // Reset Error Counter
  LED = 1;
  StartACC();
  id = GetAccID();
  
  while (1) {
    SendWord("Debut de la boucle : \n\r",23);
    LED = 1;
    SendWord("Recuperation de l'ID\n\r",22);
    id = getID();
    if (id == 42) {
      SendWord("Id Ok l'I2C fonctionne!!\n\r",26);
      LED = 0;
    }
    else {
      SendWord("Id Error : pas d'I2C\n\r",21);
	  SendChar(id);
    }

    
    GetDate();
    Latitude = GetLatitude();
    Longitude = GetLongitude();
	sprintf(buffer, "Pos : (%f,%f)\n\r",Latitude ,Longitude);
   	SendWord(buffer, strlen(buffer));

//	TestMem();

    GetAcc();
	sprintf(buffer, "Acc : (%f,%f,%f)\n\r", AccX, AccY, AccZ);
	SendWord(buffer, strlen(buffer));
    
    T0_Waitms (1000);
  }  
}


void TestMem(void)
{
    int longueur;
	float Latitude,Longitude;
	sprintf(buffer, "*** Début du test de la mémoire *** \n\r");
   	SendWord(buffer, strlen(buffer));

	//récupréation des nouvelles coordonnées
    Latitude = GetLatitude();
    Longitude = GetLongitude();

	//	Affichage de ces coordonnées
	sprintf(buffer, "Pos : (%f,%f)\n\r",Latitude ,Longitude);
	longueur = strlen(buffer)+1;
   	SendWord(buffer, strlen(buffer));

	//	Sauvegarde dans la mémoire
	WriteData(buffer,longueur,10);

	// Ecrasement du buffer et affichage
	sprintf(buffer, "Pos : (%f,%f)\n\r",0.0 ,0.0);
   	SendWord(buffer, strlen(buffer));

	//	Récupération depuisla mémoire et Affichage
	ReadData(buffer,longueur,10);
   	SendWord(buffer, strlen(buffer));

	sprintf(buffer, "*** Fin du test de la mémoire *** \n\r");
   	SendWord(buffer, strlen(buffer));
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------