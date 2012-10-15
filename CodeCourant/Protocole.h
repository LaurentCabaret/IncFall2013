#ifndef __PROTOCOLE__H_
#define __PROTOCOLE__H_

// Command IDs for SMBus commands

#define CMD_GETDATE  	0x01
#define CMD_GETID 		0x02
#define CMD_STARTCONSO 	0x03
#define CMD_STOPCONSO 	0x04      
#define CMD_GETLAT 		0x05
#define CMD_GETLONG		0x06

#define SIZE_GETDATE     18
#define SIZE_GETID       1
#define SIZE_STARTCONSO  0
#define SIZE_STOPCONSO   0      
#define SIZE_GETLAT      4
#define SIZE_GETLONG     4

typedef struct {
   float Latitude ; 
   float Longitude;
} coordonnees;

typedef union {
	coordonnees AccesFlottants;
	unsigned char AccesOctets[8];
} Datas;


//Adresses


#endif