



/******************************************
 *
 *
 *
 *
 */
char EEpromRead_1 (unsigned int addr);
int EEpromRead_2 (unsigned int  addr);
void EEpromRead_n (unsigned int  addr, char *buf, int count);

void EEprom_Write_1(unsigned int addr, char data);
void EEprom_Write_2 (unsigned int addr, int data);
void EEprom_Write_n (unsigned int addr, char *buf, int length);

