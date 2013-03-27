/** @package

	eeprom.c

	Copyright(c) Toshiba 2000

	Author: TOM BURNS
	Created: TB  26/01/2013 12:16:14 PM
	Last Change: TB  26/01/2013 12:16:14 PM
*/
//	***************************************************************************
#include <iom2560v.h>

#include "eeprom.h"

//	***************************************************************************
char EEpromRead_1 (unsigned int addr)
{ // read specified byte from internal EEPROM
  register char data;

  while (EECR & (1<<EEWE))
    ;			// wait till EEPROM is ready
  asm(" cli");
  EEAR = addr;	// set the address
  EECR = 1<<EERE;	// issue read command
  data = EEDR;	// read data
  asm(" sei");
  return data;
}

/****************************************************************************/
char EEpromRead_1_default (unsigned int addr, char def)
{
  int value;

  value = EEpromRead_1(addr);
  if ( value == 0xFF )
    return def;
  return value;
}

/****************************************************************************/
int EEpromRead_2 (unsigned int  addr)
{ // read specified word from internal EEPROM - little-endian

  return (EEpromRead_1(addr) | ((int)EEpromRead_1(addr+1) << 8) );
}

/****************************************************************************/
int EEpromRead_2_default (unsigned int  addr, int def)
{ // read specified word from internal EEPROM - little-endian
  int value;

  value = (EEpromRead_1(addr) | ((int)EEpromRead_1(addr+1) << 8) );
  if ( value == 0xFFFF )
    return def;

  return value;
}


/****************************************************************************/
void EEpromRead_n (unsigned int  addr, char *buf, int count)
{ // read a block from EEPROM
  while ( count-- )
  {
    *buf++ = EEpromRead_1(addr++);
  }
}

/****************************************************************************/
void EEprom_Write_1(unsigned int addr, char data)
{ // write byte to eeprom
  if ( EEpromRead_1(addr) == data)
    return;		// makes sure we are not re-writting the same data

  while (EECR & (1<<EEWE))
    ;				// wait till EEPROM is ready

  asm(" cli");
  EEDR = (char )data;
  EEAR = addr;
  EECR = (1<<EEMWE);	// issue write enable command
  EECR = (1<<EEWE);	// issue write command
  asm(" sei");
}

/****************************************************************************/
void EEprom_Write_2 (unsigned int addr, int data)
{ // write specified word to internal EEPROM - little-endian
  EEprom_Write_1(addr, data);
  EEprom_Write_1(addr+1, data >> 8);
}

/****************************************************************************/
void EEprom_Write_n (unsigned int addr, char *buf, int length)
{ // write a block to EEPROM (MULTI-TASKING FRIENDLY)
  while ( length-- )
  {
    EEprom_Write_1(addr++, *buf++);
  }
}

/****************************************************************************/
/****************************************************************************/

