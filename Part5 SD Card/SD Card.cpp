#include <stdint.h>;
#include "2440Reg_addr.h"
#include "def.h"
#include "option.h" 

typedef uint32_t u32;///32位
typedef uint16_t u16;///16位
typedef uint8_t u8;///8位

struct AlarmTime
{
    int year;
    int month;
    int date;
    int hour;
    int minute;
    int second;
    AlarmTime::AlarmTime(char* RTCtime);
};

u8 SD_Init(void) // 初始化SD卡
{
    u32 i;
    rGPECON |= 0xaaaaa800;    // 设置GPE5-10为SD卡功能
    rSDIPRE = PCLK / (SD_BAUD_RATE)-1;   //设置SD接口波特率为500kh
    rSDICON = (1 << 4) | 1;
    rSDIFSTA = rSDIFSTA | (1 << 16);
    for (i = 0; i < 0x1000; i++);   //等待74个SDCLK
    CMD0();  //先执行CMD0命令，让卡进入idel状态
    Check_SD_OCR();   //检测SD卡是否可用
    Read_Card_CID(SDCard_Info->CardCID);  //读取SD卡的CID
    Set_Card_RCA(0, &SDCard_Info->CardRCA);  //设置RCA卡逻辑地址
    Read_Card_CSD(SDCard_info->CardRCA, SDCard_info->CardCSD);     // CSD卡特性寄存器
    Select_or_Deselect_Card(1, SDCard_Info->CardRCA); //选中卡入传输状态
    Set_DataBus_Width(SDCard_Info->CardType, 1, SDCard_Info->CardRCA); //设置总线带宽
    return 1;
}

u8  SD_SendCommand(u8 cmd, u32 arg, u8 crc)  //向SD卡发送命令
{
    unsigned char r1;
    unsigned int Retry = 0;
    SD_CS_DISABLE();
    //发送8个时钟，提高兼容性
    SPI_ReadWriteByte(0xff);
    //选中SD卡
    SD_CS_ENABLE();
    /*按照SD卡的命令序列开始发送命令 */
    //cmd参数的第二位为传输位，数值为1，所以或0x40  
    SPI_ReadWriteByte(cmd | 0x40);
    //参数段第24-31位数据[31..24]
    SPI_ReadWriteByte((u8)(arg >> 24));
    //参数段第16-23位数据[23..16]	
    SPI_ReadWriteByte((u8)(arg >> 16));
    //参数段第8-15位数据[15..8]	
    SPI_ReadWriteByte((u8)(arg >> 8));
    //参数段第0-7位数据[7..0]
    SPI_ReadWriteByte((u8)arg);
    SPI_ReadWriteByte(crc);
    //等待响应或超时退出
    while ((r1 = SPI_ReadWriteByte(0xFF)) == 0xFF)
    {
        Retry++;
        if (Retry > 800)	break; 	//超时次数
    }
    //关闭片选
    SD_CS_DISABLE();
    //在总线上额外发送8个时钟，让SD卡完成剩下的工作
    SPI_ReadWriteByte(0xFF);
    //返回状态值	
    return r1;
}

u8  SPI_ReadWriteByte(u8 TxData) // SPI模式读取单字节的数据
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, TxData);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
}

u8 SD_ReceiveData(u8* data, u16 len, u8 release) //SD卡读取指定长度的数据
{
    u16 retry;
    u8 r1;
    //启动一次传输
    SD_CS_ENABLE();
    retry = 0;
    do
    {
        r1 = SPI_ReadWriteByte(0xFF);
        retry++;
        if (retry > 4000)  //4000次等待后没有应答，退出报错（可多试几次）
        {
            SD_CS_DISABLE();
            return 1;
        }
    }
    //等待SD卡发回数据起始令牌0xFE
    while (r1 != 0xFE);
    //跳出循环后，开始接收数据
    while (len--)
    {
        *data = SPI_ReadWriteByte(0xFF);
        data++;
    }
    //发送2个伪CRC
    SPI_ReadWriteByte(0xFF);
    SPI_ReadWriteByte(0xFF);
    //按需释放总线
    if (release == RELEASE)
    {
        SD_CS_DISABLE();
        SPI_ReadWriteByte(0xFF);
    }
    return 0;
}

u8 SD_GetCID(u8* cid_data)
{
    u8 r1;
    //发CMD10命令，读取CID信息
    r1 = SD_SendCommand(CMD10, 0, 0xFF);
    if (r1 != 0x00)
        return r1;  	//响应错误，退出 
    //接收16个字节的数据
    SD_ReceiveData(cid_data, 16, RELEASE);
    return 0;
}

u8 SD_WriteSingleBlock(u32 sector, const u8* data)
{
    u8 r1;
    u16 i;
    16 retry;
    //高速模式
    SPI_SetSpeed(SPI_SPEED_HIGH);
    //如果不是SDHC卡，将sector地址转为byte地址
    if (SD_Type != SD_TYPE_V2HC)
    {
        sector = sector << 9;
    }
    //写扇区指令
    r1 = SD_SendCommand(CMD24, sector, 0x00);
    if (r1 != 0x00)
    {
        //应答错误，直接返回
        return r1;
    }
    //开始准备数据传输
    SD_CS_ENABLE();
    //先发3个空数据，等待SD卡准备好
    SPI_ReadWriteByte(0xff);
    SPI_ReadWriteByte(0xff);
    SPI_ReadWriteByte(0xff);
    //放起始令牌0xFE
    SPI_ReadWriteByte(0xFE);
    //发一个sector数据
    for (i = 0; i < 512; i++)
    {
        SPI_ReadWriteByte(*data++);
    }
    //发送2个伪CRC校验
    SPI_ReadWriteByte(0xff);
    SPI_ReadWriteByte(0xff);
    //等待SD卡应答
    r1 = SPI_ReadWriteByte(0xff);
    //如果为0x05说明数据写入成功
    if ((r1 & 0x1F) != 0x05)
    {
        SD_CS_DISABLE();
        return r1;
    }
    //等待操作完成
    retry = 0;
    //卡自编程时，数据线被拉低
    while (!SPI_ReadWriteByte(0xff))
    {
        retry++;
        if (retry > 65534)        //如果长时间没有写入完成，退出报错
        {
            SD_CS_DISABLE();
            return 1;           //写入超时，返回1
        }
    }
    //写入完成，片选置1
    SD_CS_DISABLE();
    SPI_ReadWriteByte(0xff);
    return 0;
}

int main()
{
    SD_Init();
    AlarmTime alarm_time(RTC.get_time());   //从RTC模块获取到报警时间
    SD_WriteSingleBlock(alarm_time.time, alarm_time.release);  //把报警时间写入SD卡
    SD_ReceiveData(alarm_time.address, alarm_time.len, alarm_time.release);  //将报警时间通过SD卡读取
    return 0;
}
