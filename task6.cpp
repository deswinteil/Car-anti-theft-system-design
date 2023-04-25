/*供第九组调用，调用格式如下：
	
	     
    dm_init();              //DM9000初始化            
	....
    dm_tran_packet(arpsendbuf, 42 );   //发送ARP请求包   

	
	其中 arpsendbuf是第九组需要定义的一个遵循ARP协议格式的数组：
	unsigned char arpsendbuf[42]={   
    
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
更具体可参考我发在群里的网址，太长了就不写在注释里了 */


/*##############################################
在DM9000中，只有两个可以直接被处理器访问的寄存器，这里命名为CMD端口和DATA端口。
其他控制和状态寄存器的访问需要： 
（1）、将寄存器的地址写到CMD端口；
（2）、从DATA端口读写寄存器中的数据；
################################################*/

#define DM_ADD (*((volatile unsigned int *) 0x8000300))//地址口 
#define DM_CMD (*((volatile unsigned int *) 0x8000304))//数据口 
void delay(int count){
	for(int i=0;i<count;i++);
}
//向DM9000寄存器写数据
void dm_reg_write(unsigned char reg, unsigned char data){
	delay(20);
	DM_ADD = reg;//将寄存器地址写到INDEX端口
	delay(20);
	DM_CMD = data;//将数据写到DATA端口，即写进寄存器

}

//从DM9000寄存器读数据
unsigned int dm_reg_read(unsigned char reg){
    delay(20);
    DM_ADD = reg;
    delay(20)
    return DM_CMD;//将数据从寄存器中读出
}

/*##############################################
初始化DM9000，即给一系列寄存器赋值 
其中寄存器的地址宏定义在DM9000.h中已定义好。
################################################*/
void dm_init(void)   
{   
    dm_reg_write(DM9000_NCR,1); //软件复位DM9000   
    delay(30);              				  
    dm_reg_write(DM9000_NCR,0); //清除复位位   
    
    dm_reg_write(DM9000_NCR,1); //为了确保复位正确，再次复位   
    delay(30);   
    dm_reg_write(DM9000_NCR,0);   
         
    dm_reg_write(DM9000_GPCR,1); //设置GPIO0为输出   
    dm_reg_write(DM9000_GPR,	//激活内部PHY   
         
    dm_reg_write(DM9000_NSR,	0x2c); 	//清TX状态   
    dm_reg_write(DM9000_ISR,0xf);    //清中断状态   
         
    dm_reg_write(DM9000_RCR,0x39); //设置RX控制   
    dm_reg_write(DM9000_TCR,0);   //设置TX控制   
    dm_reg_write(DM9000_BPTR,0x3f);            
	dm_reg_write(DM9000_FCTR,0x3a);   
	dm_reg_write(DM9000_FCR,0xff);   
	dm_reg_write(DM9000_SMCR,0x00);   
      
    dm_reg_write(DM9000_PAR1,0x00); //设置MAC地址：00-01-02-03-04-05   
    dm_reg_write(DM9000_PAR2,0x01);           
    dm_reg_write(DM9000_PAR3,0x02);   
	dm_reg_write(DM9000_PAR4,0x03);   
    dm_reg_write(DM9000_PAR5,0x04);   
	dm_reg_write(DM9000_PAR6,0x05);   
      
    dm_reg_write(DM9000_NSR,0x2c); //再次清TX状态   
    dm_reg_write(DM9000_ISR,0xf);  //再次清中断状态   
    
    dm_reg_write(DM9000_IMR,0x81); //打开接受数据中断   
}  

/*##############################################
发送数据
DM9000内部有SRAM用于接受和发送数据缓存
当需要连续发送或接收数据时，需要分别把DM9000寄存器MWCMD或MRCMD赋予数据端口，这样就指定了SRAM中的某个地址，
并且在传输完一个数据后，指针会指SRAM中的下一个地址，从而完成了连续访问数据的目的。
但当我指向SRAM的数据指针不需要变化时，则要把MWCMDX或 MRCMDX赋予数据端口。
################################################*/
void dm_tran_packet(unsigned char *datas, int length)   
{   
    int i;   
         
    dm_reg_write(DM9000_IMR, 0x80);          //在发送数据过程中禁止网卡中断   
    
    dm_reg_write(DM9000_TXPLH, (length>>8) & 0x0ff);  //设置发送数据长度   
	dm_reg_write(DM9000_TXPLL, length & 0x0ff);     
    DM_ADDR_PORT = DM9000_MWCMD;            //发送数据缓存赋予数据端口   
	
	//发送数据   
    for(i=0;i<length;i+=2)   {   
        delay(50);   
        DM_DATA_PORT = datas[i]|(datas[i+1]<<8);  //8位数据转换为16位数据输出   
    }       
    dm_reg_write(DM9000_TCR, 0x01);          //把数据发送到以太网上   
    
	while((dm_reg_read(DM9000_NSR) & 0x0c) == 0)   
       ;                           //等待数据发送完成   
    delay(50); 
	  
	dm_reg_write(DM9000_NSR, 0x2c);          //清除TX状态   
	dm_reg_write(DM9000_IMR, 0x81);          //打开DM9000接收数据中断   
}   

/*##############################################
接收数据
发送数据比较简单，接收数据就略显复杂，因为它是有一定格式要求的
首字节如果为0x01，则表示这是一个可以接收的数据 包；如果为0x0，则表示没有可接收的数据包。
第二个字节为数据包的一些信息，它的高 字节的格式与DM9000的寄存器RSR完全一致。
第三个和第四个字节为数据包的长度。
后面的数据就是真正要接收的数据了
################################################*/
int dm_rec_packet(unsigned char *datas)  //输入存放输入数据数组的首地址，输出接收数据的长度。
{   
    unsigned char int_status;   
    unsigned char rx_ready;   
    unsigned short rx_status;   
    unsigned short rx_length;   
    unsigned short temp;   
    int i;   
    
    int_status = dm_reg_read(DM9000_ISR);           //读取ISR   
    if(int_status & 0x1)                     //判断是否有数据要接受   
    {   
        rx_ready = dm_reg_read(DM9000_MRCMDX);       //先读取一个无效的数据   
        rx_ready = (unsigned char)DM_DATA_PORT;      //真正读取到的数据包首字节   
                
        if(rx_ready == 1)                 
        {   
            DM_ADDR_PORT = DM9000_MRCMD;      //连续读取数据包内容   
            rx_status = DM_DATA_PORT;        //状态字节   
                       
            rx_length = DM_DATA_PORT;       //数据长度   
                       
            if(!(rx_status & 0xbf00) && (rx_length < 10000))     //判读数据是否符合要求   
            {   
                for(i=0; i<rx_length; i+=2)          //16位数据转换为8位数据存储   
                {   
                    delay(50);   
                    temp = DM_DATA_PORT;   
                    datas[i] = temp & 0x0ff;   
                    datas[i + 1] = (temp >> 8) & 0x0ff;   
                }   
            }   
        }   
        else if(rx_ready !=0)      //停止设备   
        {   
            //dm_reg_write(DM9000_IMR,0x80);  //停止中断   
            //dm_reg_write(DM9000_ISR,0x0F);   //清中断状态   
        	//dm_reg_write(DM9000_RCR,0x0);    //停止接收   
            //还需要复位系统，这里暂时没有处理   
        }   
    }   
    dm_reg_write(DM9000_ISR, 0x1);             //清中断   
    return rx_length;   
}  
