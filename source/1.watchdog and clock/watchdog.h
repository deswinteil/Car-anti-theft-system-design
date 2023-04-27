#ifndef __WATCHDOG_H
#define __WATCHDOG_H
#include "s3c2440.h"
#include "def.h"
 
 
void WWDGInitForTimer(u8 prescaler,u8 mux,u16 countValue);
 
void WWDGInitForReset(u8 prescaler,u8 mux,u16 countValue);
 
void Watchfeed(u16 count);	//看门狗喂狗
 
 
extern u8 wwdgUp;
 
 
#endif