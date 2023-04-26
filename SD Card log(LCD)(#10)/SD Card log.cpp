#include <iostream>
using namespace std;

int GBPCON;
int CPDCON;

void lcdpininit() 
{ //初始化lcd引脚
	GBPCON &= ~0x3;//对GBPCON低二位清零
	GPBCON |= 0x01;//把最后一位设成1
	//初始化背光引脚GPB0
	GPCCON = 0xaaaaaaaa;
	GPDCON = 0xaaaaaaaa;
	//初始化LCD专用引脚
	GPGCON |= (3 << 8);
	//复用
}

void lcdctrlinit()
{
	int CLKVAL = 100 / VCLK / 2 - 1;
	//设置时钟分频值
	int bppmod = 0xd;
	int LCDCON1;
	int LCDCON2;
	int LCDCON3;
	int LCDCON4;
	int LCDCON5;
	LCDCON1 = (CLKVAL << 8) | (3 << 5) | (BPPMODE << 1);
	LCDCON2 = (VBPD < 24) | (LINEVAL << 14) | (VFPD << 6) | (VSPW << 0);
	LCDCON3 = (HBPD << 19) | (HOZVAL << 8) | (HFPD << 0);
	LCDCON4 = (HSPW << 0);
	LCDCON5 = (0 << 10) | (0 << 9) | (0 << 8) | (0 << 7) | (0 << 6) | (0 << 5);
	//初始化LCD控制器寄存器组
	int addr = fb_base & ~(1 << 31);
	//初始化地址，并且地址最高位总为0
	addr = fb_base + xres * yres * 4;
	addr >>= 1;		 
	addr &= 0x1fffff;  	
	LCDSADDR2 = addr;
}

void lcdconuse(void) {
	GPBDAT |= (1 << 0);
	// pwren给LCD提供AVDD
	LCDCON5 |= (1 << 3);
	//LCDCON1'BIT 0 : 设置LCD控制器是否输出信号
	LCDCON1 |= (1 << 0);
}

void lcdconstop(void) {
	GPBDAT &= ~(1 << 0);
	// pwren	给LCD提供AVDD
	LCDCON5 &= ~(1 << 3);
	// LCDCON1'BIT 0 : 设置LCD控制器是否输出信号
	LCDCON1 &= ~(1 << 0);
}

void getc() {

}

extern const unsigned char fontdata_8x16[];
//ASCII字符点阵
void fbshowchar(int x, int y, char c, unsigned char red, char green, char blue) {
	int i, j;
	//获得显示参数
	get_lcd_params(&fb_base, &xres, &yres);
	// 获得c的ascii码的点阵数据
	unsigned char* dots = &fontdata_8x16[c * 16];
	unsigned char data;   //存一行点阵数据
	int bit;
	for (j = y; j < y + 16; j++) { //读取每行像素
		data = *dots++;
		bit = 7;
		for (i = x; i < x + 8; i++) {
			//根据点阵的某位将像素的颜色数据写入FB
			if (data & (1 << bit))				fb_put_pixel(i, j, red, green, blue);
			bit--;
		}
	}
}

void fbshowpixal(int i, int j, unsigned char red, char green, char blue) {
	unsigned int* pc;
	unsigned int pixel_base;
	pixel_base = fb_base + (xres * j + i) * 4;
	pc = (unsigned int*)pixel_base;
	*pc = (0 << 24) | ((red << 16) | (green << 8) | blue;//根据不同数值确定输出颜色
}

//键盘中断初始化
void KeyINT_Init(void) {
	rGPFCON = (rGPFCON & 0xF3FF) | (2 << 10);
	rGPECON = (rGPECON & 0xF3FFFFFF) | (0x01 << 26); //设置GPE13为输出位，模拟时钟输出
	rEXTINT0 &= 0xff8fffff; //外部中断5低电平有效
	if (rEINTPEND & 0x20)   //清除外部中断5的中断挂起位
		rEINTPEND |= 0x20;
	if ((rINTPND & BIT_EINT4_7)) {
		rSRCPND |= BIT_EINT4_7;
		rINTPND |= BIT_EINT4_7;
	}
	rINTMSK &= ~(BIT_EINT4_7);      //使用外部中断4～7
	rEINTMASK &= 0xffffdf;           //外部中断5使能
	pISR_EINT4_7 = (int)Key_ISR;
}

//书写中断服务子程序
void __irq Key_ISR(void) {
	int j;
	rINTMSK |= BIT_ALLMSK;   // 关中断
	if (rEINTPEND & 0x20) {
		key_number = read7279(cmd_read);//读键盘指令程序
		rINTMSK &= ~(BIT_EINT4_7);
		switch (key_number) {
		case 0x04:     key_number = 0x08;   break;
		case 0x05:     key_number = 0x09;   break;
		case 0x06:     key_number = 0x0A;   break;
		case 0x07:     key_number = 0x0B;   break;
		case 0x08:     key_number = 0x04;   break;
		case 0x09:     key_number = 0x05;   break;
		case 0x0A:     key_number = 0x06;   break;
		case 0x0b:     key_number = 0x07;   break;
		default:        break;
		}
		Uart_Printf("key is %x \n", key_number);
	}
	rEINTPEND |= 0x20;
	rSRCPND |= BIT_EINT4_7;
	rINTPND |= BIT_EINT4_7;
}

