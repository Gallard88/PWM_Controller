/** @package 

	conf_eeprom.h
	
	Copyright(c) Toshiba 2000
	
	Author: TOM BURNS
	Created: TB  2/02/2013 10:25:04 AM
	Last Change: TB  2/02/2013 10:25:04 AM
*/
//*****************************************************************************
#ifndef __EEPROM_H__
#define __EEPROM_H__
//*****************************************************************************
#define EE_TEMP_LIMIT		1	// 2 bytes
#define EE_TEMP_UPDATE		3

#define EE_CURRENT_LIMIT	5	// 2 bytes
#define EE_CURRENT_UPDATE	7

#define EE_COM_TIMEOUT	9		// 1 bytes

#define EE_NEXT				10
#define EE_LIMIT			(4*1024)

//*****************************************************************************
//*****************************************************************************
#endif

