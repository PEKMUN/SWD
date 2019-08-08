#ifndef _SWDLOWLEVEL_H
#define _SWDLOWLEVEL_H

void swdWriteBit(int bit);
int swdReadBit(void);
void swdReadTurnAround();
void swdWriteTurnAround();

void configureGPIO();

#endif // _SWDLOWLEVEL_H
