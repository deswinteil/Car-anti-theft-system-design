#include "s3c2440.h"
void clock_init(void);
 
#define S3C2440_MPLL_400MHZ     ((92<<12)|(1<<4)|(1<<0))
/*
 * 对于MPLLCON寄存器，[19:12]为MDIV，[9:4]为PDIV，[1:0]为SDIV
 * 有如下计算公式：
 *  S3C2440: MPLL(FCLK) = (2 * m * Fin)/(p * 2^s)
 *  其中: m = MDIV + 8, p = PDIV + 2, s = SDIV
 * 对于本开发板，Fin = 12MHz
 * 设置CLKDIVN，令分频比为：FCLK:HCLK:PCLK=1:4:8，
 * FCLK=400MHz,HCLK=100MHz,PCLK=50MHz
 */
void clock_init(void)
{
    LOCKTIME = 0xFFFFFFFF;   // 使用默认值即可
    CLKDIVN  = 0x05;            // FCLK:HCLK:PCLK=1:4:8
 
    /* 如果HDIVN非0，CPU的总线模式应该从“fast bus mode”变为“asynchronous bus mode” */
__asm__(
    "mrc    p15, 0, r1, c1, c0, 0\n"        /* 读出控制寄存器 */ 
    "orr    r1, r1, #0xc0000000\n"          /* 设置为“asynchronous bus mode” */ //R1_nF:OR:R1_iA
    "mcr    p15, 0, r1, c1, c0, 0\n"        /* 写入控制寄存器 */
    );
 
    MPLLCON = S3C2440_MPLL_400MHZ;  /* 现在，FCLK=200MHz,HCLK=100MHz,PCLK=50MHz */      
}