实现以太网接口DM9000A的初始化和数据收发，主要函数如下：

```c++
void dm_reg_write(unsigned char reg, unsigned char data)//向DM9000寄存器写数据
unsigned int dm_reg_read(unsigned char reg)//从DM9000寄存器读数据
//上述两个函数主要是供后面三个函数调用
void dm_init(void) //初始化
void dm_tran_packet(unsigned char *datas, int length)//发数据包
int dm_rec_packet(unsigned char *datas)   //收数据包
```

供第九组调用，调用格式如下：

```assembly
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
```

