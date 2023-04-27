#include "ethernet interface.h"
#include "SD Card.h"



void vibrate(if_vibrate)
{
    //调用第五组的函数(向SD CARD写入报警日志)
    AlarmTime alarm_time(RTC.get_time());
    SD_WriteSingleBlock(alarm_time, alarm_time.release);
    //调用第六组的函数
    unsigned char arpsendbuf[42] = {

       0xff,0xff,0xff,0xff,0xff,0xff,                     //以太网目标地址，全1表示为广播地址   
       0x00,0x01,0x02,0x03,0x04,0x05,        //以太网源地址   
       0x08,0x06,                                        //帧类型：ARP帧   

       0x00,0x01,                                        //硬件类型：以太网   
       0x08,0x00,                                        //协议类型：IP协议   
       0x06,                                                //硬件地址长度：6字节   
       0x04,                                                //协议地址长度：4字节   
       0x00,0x01,                                        //操作码：ARP请求   

       0x00,0x01,0x02,0x03,0x04,0x05,        //发送端以太网硬件地址   
       192, 168, 1, 50,                                 //发送端IP协议地址   
       0x00,0x00,0x00,0x00,0x00,0x00,        //接收端以太网硬件地址   
       192, 168, 1, 120                                 //接收端IP协议地址   
    };
    dm_init();              //DM9000初始化
    dm_tran_packet(arpsendbuf,42);
}
