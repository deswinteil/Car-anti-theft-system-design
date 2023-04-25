#include <stdint.h>;
#include "RTC_time.h"
typedef uint32_t u32;///32位
typedef uint16_t u16;///16位
typedef uint8_t u8;///8位

u8 SD_Init(void)
{
    u16 i;
    u8 r1;
    u16 retry;
    u8 buff[6];
    SPI_ControlLine();
    //SD卡初始化时时钟不能超过400KHz
    SPI_SetSpeed(SPI_SPEED_LOW);
    //CS为低电平，片选置低，选中SD卡		
    SD_CS_ENABLE();
    //纯延时，等待SD卡上电稳定
    for (i = 0; i < 0xf00; i++);
    //先产生至少74个脉冲，让SD卡初始化完成
    for (i = 0; i < 10; i++)
    {
        //参数可随便写，经过10次循环，产生80个脉冲
        SPI_ReadWriteByte(0xff);
    }
    //-----------------SD卡复位到idle状态----------------
    //循环发送CMD0，直到SD卡返回0x01,进入idle状态
    //超时则直接退出
    retry = 0;
    do
    {
        //发送CMD0，CRC为0x95
        r1 = SD_SendCommand(CMD0, 0, 0x95);
        retry++;
    } while ((r1 != 0x01) && (retry < 200));
    //跳出循环后，检查跳出原因，
    if (retry == 200)	//说明已超时	
    {
        return 1;
    }
    //如果未超时，说明SD卡复位到idle结束
    //发送CMD8命令，获取SD卡的版本信息
    r1 = SD_SendCommand(CMD8, 0x1aa, 0x87);
    //下面是SD2.0卡的初始化		
    if (r1 == 0x01)
    {
        // V2.0的卡，CMD8命令后会传回4字节的数据，要跳过再结束本命令
        buff[0] = SPI_ReadWriteByte(0xFF);
        buff[1] = SPI_ReadWriteByte(0xFF);
        buff[2] = SPI_ReadWriteByte(0xFF);
        buff[3] = SPI_ReadWriteByte(0xFF);
        SD_CS_DISABLE();
        //多发8个时钟	  
        SPI_ReadWriteByte(0xFF);
        retry = 0;
        //发卡初始化指令CMD55+ACMD41
        do
        {
            r1 = SD_SendCommand(CMD55, 0, 0);
            //应返回0x01
            if (r1 != 0x01)
                return r1;
            r1 = SD_SendCommand(ACMD41, 0x40000000, 1);
            retry++;
            if (retry > 200)
                return r1;
        } while (r1 != 0);
        //初始化指令发送完成，接下来获取OCR信息	   
        //----------鉴别SD2.0卡版本开始-----------
        //读OCR指令
        r1 = SD_SendCommand_NoDeassert(CMD58, 0, 0);
        //如果命令没有返回正确应答，直接退出，返回应答
        if (r1 != 0x00)
            return r1;
        //应答正确后，会回传4字节OCR信息
        buff[0] = SPI_ReadWriteByte(0xFF);
        buff[1] = SPI_ReadWriteByte(0xFF);
        buff[2] = SPI_ReadWriteByte(0xFF);
        buff[3] = SPI_ReadWriteByte(0xFF);
        //OCR接收完成，片选置高
        SD_CS_DISABLE();
        SPI_ReadWriteByte(0xFF);
        //检查接收到的OCR中的bit30位（CSS），确定其为SD2.0还是SDHC
        //CCS=1：SDHC   CCS=0：SD2.0
        if (buff[0] & 0x40)
        {
            SD_Type = SD_TYPE_V2HC;
        }
        else
        {
            SD_Type = SD_TYPE_V2;
        }
        //-----------鉴别SD2.0卡版本结束----------- 
        SPI_SetSpeed(1); 		//设置SPI为高速模式
    }
}

u8  SPI_ReadWriteByte(u8 TxData)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, TxData);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
}

u8 SD_ReceiveData(u8* data, u16 len, u8 release)
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

u8  SD_SendCommand(u8 cmd, u32 arg, u8 crc)
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

u32 SD_GetCapacity(void)
{
    u8 csd[16];
    u32 Capacity;
    u8 r1;
    u16 i;
    u16 temp;
    //取CSD信息，如果出错，返回0
    if (SD_GetCSD(csd) != 0)
        return 0;
    //如果是CSD寄存器是2.0版本，按下面方式计算
    if ((csd[0] & 0xC0) == 0x40)
    {
        Capacity = ((u32)csd[8]) << 8;
        Capacity += (u32)csd[9] + 1;
        Capacity = (Capacity) * 1024;	//得到扇区数
        Capacity *= 512;	//得到字节数   
    }
    else		//CSD寄存器是1.0版本
    {
        i = csd[6] & 0x03;
        i <<= 8;
        i += csd[7];
        i <<= 2;
        i += ((csd[8] & 0xc0) >> 6);
        r1 = csd[9] & 0x03;
        r1 <<= 1;
        r1 += ((csd[10] & 0x80) >> 7);
        r1 += 2;
        temp = 1;
        while (r1)
        {
            temp *= 2;
            r1--;
        }
        Capacity = ((u32)(i + 1)) * ((u32)temp);
        i = csd[5] & 0x0f;
        temp = 1;
        while (i)
        {
            temp *= 2;
            i--;
        }
        //最终结果
        Capacity *= (u32)temp;
        //字节为单位  
    }
    return (u32)Capacity;
}

u8 SD_ReadSingleBlock(u32 sector, u8* buffer)
{
    u8 r1;
    //高速模式
    SPI_SetSpeed(SPI_SPEED_HIGH);
    if (SD_Type != SD_TYPE_V2HC)	//如果不是SDHC卡
    {
        sector = sector << 9;	//512*sector即物理扇区的边界对其地址
    }
    r1 = SD_SendCommand(CMD17, sector, 1);	//发送CMD17 读命令
    if (r1 != 0x00)	return r1;
    r1 = SD_ReceiveData(buffer, 512, RELEASE);	//一个扇区为512字节
    if (r1 != 0)
        return r1;   //读数据出错
    else
        return 0; 		//读取正确，返回0
}

int main(void)
{
    u16 i;
    USART1_Config();
    send_data[1536] = RTC.get_time()
    switch (SD_Init())
    {
    case 0:
        USART1_Puts("\r\nSD Card Init Success!\r\n");
        break;
    case 1:
        USART1_Puts("Time Out!\n");
        break;
    case 99:
        USART1_Puts("No Card!\n");
        break;
    default: USART1_Puts("unknown err\n");
        break;
    }
    SD_WriteSingleBlock(30, send_data);
    SD_ReadSingleBlock(30, receive_data);
    if (Buffercmp(send_data, receive_data, 512))
    {
        USART1_Puts("\r\n single read and write success \r\n");
        //USART1_Puts(receive_data);
    }
    while (1);
}