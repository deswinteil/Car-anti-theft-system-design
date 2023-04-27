#include "watchdog.h"
 
u8 wwdgUp = 0;
 
void __irq Wdt_Int(void)
{
	rSRCPND |=BIT_WDT_AC97;
    rINTPND |=BIT_WDT_AC97;
    rSUBSRCPND |= BIT_SUB_WDT; //清除中断标志
   
//    WatchdogInit(0);
	wwdgUp = 1;
}
 
//看门狗不连接系统复位
//prescaler 分频系数 8位
//mux 始终选择 0 16 1 32 2 64 3 128
//countValue 计数值
void WWDGInitForTimer(u8 prescaler,u8 mux,u16 countValue)
{
	
	rWTCON &= ~(1<<5);//看门狗禁止
	
	rWTCON &= 0xff00;//清除预分频系数
	rWTCON |= prescaler<<8;//设置预分频系数
	
	rWTCON &= ~(0x03<<3);
	rWTCON |= mux<<3;//设置时钟选择器
	
	rWTCON |= 1<<2;//使能中断]
	rWTCON &= ~(1<<0);//禁止复位
	rWTDAT = countValue;
	rWTCNT = countValue;//初次使用,必须设置CNT的值
	
	rSRCPND |=BIT_WDT_AC97;		//清除标志
    rINTPND |=BIT_WDT_AC97;
	rSUBSRCPND |= BIT_SUB_WDT; //清除子中断标志
	
	rINTMOD &= ~BIT_WDT_AC97;//设置中断模式为IRQ模式
	rINTMSK &= ~(BIT_WDT_AC97); //开中断
  	rINTSUBMSK &= ~(BIT_SUB_WDT);//开子中断
	
	pISR_WDT_AC97=(unsigned)Wdt_Int;//设置中断程序入口
	
	rWTCON |= (1<<5);//看门狗启动
	
}
 
 
//看门狗连接系统复位
//prescaler 分频系数 8位
//mux 始终选择 0 16 1 32 2 64 3 128
//countValue 计数值
void WWDGInitForReset(u8 prescaler,u8 mux,u16 countValue)
{
	
	rWTCON &= ~(1<<5);//看门狗禁止
	
	rWTCON &= 0xff00;//清除预分频系数
	rWTCON |= prescaler<<8;//设置预分频系数
	
	rWTCON &= ~(0x03<<3);
	rWTCON |= mux<<3;//设置时钟选择器
	
	rWTCON &= ~(1<<2);//禁止中断]
	rWTCON |= (1<<0);//使能复位
	rWTDAT = countValue;
	rWTCNT = countValue;//初次使用,必须设置CNT的值
	
	rWTCON |= (1<<5);//看门狗启动
	
}
 
void Watchfeed(u16 count)	//看门狗喂狗
{
	rWTCNT=count;	//喂狗	
}