## LCD控制器部分

为了在LCD上输出报警信息，在这里使用LCDcontroller对其进行管理。S3C2440A中的LCD控制器由从位于视频缓冲区传输LCD图像数据的逻辑组成，在系统内存中的外部LCD驱动。 LCD控制器支持单色，每像素2位(4级灰度)或每像素4位(16级灰度)模式下，采用基于时间的抖动算法和帧率控制(FRC)方法，实现了对单色液晶显示器的控制。

在此处采用TFT工艺下的LCD配套配置。TIMEGEN为LCD驱动产生$VSYNC$、$HSYNC$、$VCLK$、$VDEN$、$LEND$等控制信号。这些控制信号与LCDCON1/2/3/4/5寄存器的配置高度相关。

基于REGBANK中LCD控制寄存器上的这些可编程配置 

TIMEGEN可以生成可编程控制信号，适合支持许多不同类型的LCD驱动程序。 

$VSYNC$信号被断言导致LCD的线指针在显示的顶部重新开始。VSYNC和HSYNC脉冲的产生取决于$HOZVAL$字段和$LINEVAL$字段的配置字段在LCDCON2/3寄存器。$HOZVAL$和$LINEVAL$可以由LCD面板的大小决定 

根据下式: 

\- $HOZVAL =(水平显示大小)-1$

\- $LINEVAL =(垂直显示大小)-1$

VCLK信号的速率取决于LCDCON1寄存器中的CLKVAL字段，公式如程序中所示。

使用24BPP输出模式时，$BSWP=0，HWSWP=0，BPP24BL=0$



![LCD控制器](C:\Users\qinyibo\AppData\Roaming\Typora\typora-user-images\image-20230426003318945.png)

REGBANK：寄存器组，由17个寄存器和一个256×16的调色板构成，用来设置LCD控制器。

LCDCDMA：专用DMA，用来自动将存储器中的视频数据传送到LCD驱动器中。无需CPU干涉。

VIDPRCS：视频数据产生，用来从LCDCDMA接收视频数据，并且转换成适当的格式（如：4/8比特单扫描或4比特双扫描），然后从端口VD[23:0] 发送到LCD驱动器。

TIMEGEN: 时序产生，用来产生视频控制信号。

LPC3600：定时控制生成逻辑单元。

S3C2410A中的LCD控制器一共有33个输出端口，其中24个数据信号端口，9个控制信号端口。

$VFRAME/VSYNC/STV$ : LCD控制器发往驱动器的帧同步信号 ，它表示新的一帧的开始。LCD控制器在一个完整的帧显示后发出该信号，开始新的一帧的显示。

$VLINE/HSYNC/CPV￥ : LCD控制器发往驱动器的行同步脉冲信号 ，LCD驱动器在收到该信号后，将水平移位寄存器的内容显示到LCD屏上。LCD控制器在一整行数据全部传输到LCD驱动器后，发出一个VLINE信号。

$VCLK/LCD_HCLK$ : LCD控制器发往驱动器的像素时钟信号 ，LCD控制器在VCLK的上升沿发送数据，LCD驱动器在VCLK的下降沿采样数据。

VD[23:0] : LCD 像素数据输出端口 ，也就是平时所说的RGB信号线，采用的是5:6:5的模式。

$VM/VDEN/TP$ : LCD 驱动器所使用的的AC 偏置信号 。LCD驱动器使用该信号改变行电压和列电压的极性，进而打开或者关闭像素，使像素点显示或熄灭。VM信号可以与帧同步信号或者行同步信号进行同步。

$LEND/STH$ : 行终止信号 (TFT)/SEC TFT信号 STH

$LCD_PWREN$ : LCD 电源使能信号

$LCDVF0$ : SEC TFT OE信号

$LCDVF1$ : SEC TFT REV信号

$LCDVF2$ : SEC TFT REVB信号

---

### 调色板配置和格式控制 

S3C2440A为TFT LCD控制提供256色调色板。用户可以从这两种格式的64K颜色中选择256种颜色。256色调色板由256×16位SPSRAM组成。选项板支持5:6:5 (R:G:B)格式和5:5:5:1 (R, G, B: I)格式。当用户使用5:5:5:1格式时，强度数据(I)作为每个RGB数据的公共LSB位。所以,5:5:5:1格式与R(5+I):G(5+I):B(5+I)格式相同。

### 调色板读/写 

当用户在调色板上执行读/写操作时，必须使用LCDCON5寄存器的HSTATUS和VSTATUS。HSTATUS和VSTATUS处于ACTIVE状态时，禁止读写操作。

## 键盘部分

![HD7279A与S3C2410A的连接原理图](C:\Users\qinyibo\AppData\Roaming\Typora\typora-user-images\image-20230426003709715.png)

HA7279A是一片具有串行接口并可同时驱动8位共阴式数码管或64只独立LED的智能显示驱动芯片。该芯片同时可连接多达64键的键盘矩阵，一片即可完成LED显示及键盘接口的全部功能。

HA7279A一共有28个引脚：

RESET：复位端。通常，该端接＋5V电源；

DIG0～DIG7：８个LED管的位驱动输出端；

SA～SG：LED数码管的Ａ段～Ｇ段的输出端；

DP：小数点的驱动输出端；

RC：外接振荡元件连接端。

HD7279A与微处理器仅需４条接口线:

CS：片选信号（低电平有效）；

DATA：串行数据端。

CLK：数据串行传送的同步时钟输入端，时钟的上升沿表示数据有效。

KEY:按键信号输出端。该端在无键按下时为高电平；而在有键按下时变为低电平，并一直保持到按键释放为止。

 