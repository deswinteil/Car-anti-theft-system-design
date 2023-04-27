#include <iostream>
#include <stdint.h>
#include "2440addr.h"
#define NOR_FLASH_BASE 0
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

// Initialization of nor flash
void Init_NORFlash(void)
{
    rBANKCON0 = 0x0060;
    *((volatile u16 *)0x0) = 0x90;
    std::cout << "Manufacturer ID: " << *(volatile u16 *)(NOR_FLASH_BASE + 0x0) << std::endl; // 0x89
    std::cout << "Device ID: " << *(volatile u16 *)(NOR_FLASH_BASE + 0x1) << std::endl;       // 0x18
}

// Reset function, write 0xff to any address
void nor_reset()
{
    *((volatile u16 *)0x0) = 0xff;
}

// Write command
void nor_write_cmd(u32 addr, u16 val)
{
    *((volatile u16 *)addr) = val;
}

// Reading data function, in which the offset is from the Nor perspective
u16 nor_read(unsigned int offset)
{
    return *(volatile u16 *)(NOR_FLASH_BASE + offset);
}

// Sector erase function
u32 nor_erase_block(u32 targetAddress)
{
    u16 ReadStatus;
    u16 bSR5;
    u16 bSR7;
    nor_write_cmd(targetAddress, 0x0020); // first cycle of erase command
    nor_write_cmd(targetAddress, 0x00d0); // second cycle of erase command
    nor_write_cmd(targetAddress, 0x0070); // command of reading status register
    ReadStatus = nor_read(targetAddress); // read status register
    bSR7 = ReadStatus & (1 << 7);
    while (!bSR7) // determine the 7th bit of the status register
    {
        nor_write_cmd(targetAddress, 0x0070);
        ReadStatus = nor_read(targetAddress);
        bSR7 = ReadStatus & (1 << 7);
    }
    nor_write_cmd(targetAddress, 0x0070);
    ReadStatus = _RD(targetAddress);
    bSR5 = ReadStatus & (1 << 5); // determine the 5th bit of the status register
    if (bSR5 == 0)
    {
        std::cout << "Successfully erased " << std::endl;
    }
    else
    {
        nor_write_cmd(targetAddress, 0x0050); // clear status register
        std::cout << "Error!" << std::endl;
        return -1;
    }
    nor_reset();
    return 0;
}

// Write Function
u32 nor_write(u32 realAddr, u16 data)
{
    unsigned long ReadStatus;
    unsigned long bSR4;
    unsigned long bSR7;
    nor_write_cmd(realAddr, 0x0040);
    nor_write_cmd(realAddr, data);
    nor_write_cmd(realAddr, 0x0070);
    ReadStatus = _RD(realAddr);
    bSR7 = ReadStatus & (1 << 7);
    while (!bSR7) // determine the 7th bit of the status register
    {
        nor_write_cmd(realAddr, 0x0070);
        ReadStatus = _RD(realAddr);
        bSR7 = ReadStatus & (1 << 7);
    }
    nor_write_cmd(realAddr, 0x0070);
    ReadStatus = _RD(realAddr);
    bSR4 = ReadStatus & (1 << 4); // determine the 4th bit of the status register
    if (bSR4 == 0)
    {
        std::cout << "Successfully programmed" << std::endl;
    }
    else
    {
        nor_write_cmd(realAddr, 0x0050); // clear status register
        std::cout << "Error!" << std::endl;
        return -1;
    }
    nor_reset();
    return 0;
}
