#include "RTC_readtime.h"
#include "2440Reg_addr.h"
#include "def.h"
unsigned char tmp_rtc[6]={0}
unsigned char rtc_data_tmp[14]={0}
void init_rtc(void) {
    /* 1. 设置RTC时钟源为低速外部时钟 */
    rtccon |= (1 << 0);
    rtccon &= ~(1 << 1);
    /* 2. 设置RTC分频器为32768 */
    rtccon |= (0x1 << 5);
    /* 3. 设置RTC控制寄存器，使能RTC模块 */
    rtccon |= (1 << 3);
}
void set_rtc()  
{  
//2023,04.26,13.06.51  
    rRTCCON |= 0x01;  
    rBCDSEC = 5*16 +1;  
    rBCDMIN = 6;  
    rBCDHOUR = 1*16 +3;  
    rBCDDATE = 2*16+6;  
    rBCDMON =4;  
    rBCDYEAR = 2*16+3;  
    rRTCCON &= ~(0x01);  
}  
void get_rtc(unsigned char rtc_data[6])  
{  
    
    rRTCCON |= 0x01;  
    rtc_data[0] = rBCDSEC;  
    rtc_data[1] = rBCDMIN;  
    rtc_data[2] = rBCDHOUR;  
    rtc_data[3] = rBCDDATE;  
    rtc_data[4] = rBCDMON;  
    rtc_data[5] = rBCDYEAR;  
      
    rRTCCON &= ~(0x01);   
  
    if(rtc_data[0] == 0)  
    {  
    rRTCCON |= 0x01;  
    rtc_data[0] =rBCDSEC;  
    rtc_data[1] = rBCDMIN;  
    rtc_data[2] = rBCDHOUR;  
    rtc_data[3] = rBCDDATE;  
    rtc_data[4] = rBCDMON;  
    rtc_data[5] = rBCDYEAR;   
    rRTCCON &= ~(0x01);   
    }  
  
}  
unsigned char* get_data()
{   
    init_rtc();
    set_rtc(); 
    get_rtc(tmp_rtc);  
    rtc_data_tmp[13] = tmp_rtc[0]%16+'0';  
    rtc_data_tmp[12] = tmp_rtc[0]/16+'0'; 
    rtc_data_tmp[11] = tmp_rtc[1]%16+'0';  
    rtc_data_tmp[10] = tmp_rtc[1]/16+'0';  
    rtc_data_tmp[9] = tmp_rtc[2]%16+'0';  
    rtc_data_tmp[8] = tmp_rtc[2]/16+'0';  
    rtc_data_tmp[7] = tmp_rtc[3]%16+'0';  
    rtc_data_tmp[6] = tmp_rtc[3]/16+'0'; 
    rtc_data_tmp[5] = tmp_rtc[4]%16+'0';  
    rtc_data_tmp[4] = tmp_rtc[4]/16+'0';  
    rtc_data_tmp[3] = tmp_rtc[5]%16+'0';  
    rtc_data_tmp[2] = tmp_rtc[5]/16+'0';   
    rtc_data_tmp[1] = tmp_rtc[6]%16+'0';  
    rtc_data_tmp[0] = tmp_rtc[6]/16+'0';   
    return rtc_data_tmp;
}