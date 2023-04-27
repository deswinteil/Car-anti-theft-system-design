#include <iostream>
#include <stdint.h>
#include "2440addr.h"
#define NOR_FLASH_BASE 0
#define rCMD0 *((volatile u16 *)(0x555 << 1 + NOR_FLASH_BASE))
#define rCMD1 *((volatile u16 *)(0x2aa << 1 + NOR_FLASH_BASE))
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

// Initialization of nor flash
void Init_NORFlash(void)
{
    rBANKCON0 = 0x0060;
    rCMD0 = 0xaa;
    rCMD1 = 0x55;
    rCMD0 = 0x90;
    std::cout << "Manufacturer ID: " << *(volatile u16 *)(NOR_FLASH_BASE + 0x0) << std::endl; // 0x01
    std::cout << "Device ID: " << *(volatile u16 *)(NOR_FLASH_BASE + 0x1 << 1) << std::endl;  // 0x225b
    rCMD0 = 0x0;
    rCMD1 = 0x0;
}

// Reset function, write 0xf0 to any address
void nor_reset()
{
    *((volatile u16 *)0x0) = 0xf0;
}

// Write check
u8 nor_check()
{
    volatile u16 newtoggle, oldtoggle;
    oldtoggle = *((volatile u16 *)0x0);
    while (1)
    {
        newtoggle = *((volatile u16 *)0x0);
        if ((oldtoggle & 0x40) == (newtoggle & 0x40)) // DQ6
            break;
        if (newtoggle & 0x20) // DQ5
        {
            oldtoggle = *((volatile u16 *)0x0);
            newtoggle = *((volatile u16 *)0x0);
            if ((oldtoggle & 0x40) == (newtoggle & 0x40))
                break;
            else
                return -1; // wrong
        }
        oldtoggle = newtoggle;
    }
    return 0; // correct
}

// Reading data function, in which the offset is from the Nor perspective
unsigned int nor_read(unsigned int offset)
{
    return *(volatile u16 *)(NOR_FLASH_BASE + offset);
}

// Sector erase function
u8 nor_erase_sector(u16 addr)
{
    rCMD0 = 0xaa;
    rCMD1 = 0x55;
    rCMD0 = 0x80;
    rCMD0 = 0xaa;
    rCMD1 = 0x55;
    *((volatile u16 *)(addr)) = 0x30;
    return nor_check();
}

// Write Function
u8 nor_write(u32 addr, u16 val)
{
    rCMD0 = 0xaa;
    rCMD1 = 0x55;
    rCMD0 = 0xa0;
    *((volatile u16 *)(addr)) = val;
    return nor_check();
}
