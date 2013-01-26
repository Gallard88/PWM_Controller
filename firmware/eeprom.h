



/******************************************
 *
 *
 *
 *
	Last change: TB 26/01/2013 12:16:17 PM
 */
char EEpromRead_1 (unsigned int addr);
char EEpromRead_1_default (unsigned int addr, char def);
int EEpromRead_2 (unsigned int  addr);
int EEpromRead_2_default (unsigned int  addr, int def);
void EEpromRead_n (unsigned int  addr, char *buf, int count);

void EEprom_Write_1(unsigned int addr, char data);
void EEprom_Write_2 (unsigned int addr, int data);
void EEprom_Write_n (unsigned int addr, char *buf, int length);

