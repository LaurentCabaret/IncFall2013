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
#define SIZE_GETLAT      8
#define SIZE_GETLONG     8


//Adresses
#define SLAVE_MEM_ADR 0

#endif