ARM_init.s是主程序，其中调用程序即说明如下：
void Clock_init(void)		初始化时钟、看门狗电路 c函数，第1组负责提供
void MemSetup(void)		初始化SDRAM c函数，第2组负责提供
void dm_init(void)			初始化以太网 c函数，第6组负责提供	*
void lcdpininit(void)
void lcdctrlinit(void)		初始化LCD c函数，第10组负责提供	*
void Init_NORFlash(void)		初始化NOR Flash c函数，第3组负责提供
void KeyINT_Init(void)		初始化键盘 c函数，LCD查阅SD卡时需要键盘，所以也应由第10组负责	*
void SD_Init(void)		初始化SD CARD c函数，第5组负责	*
void Handle_EINT0(void)		处理EINT0的中断服务程序，第9组负责
用于处理传感器的中断
void Key_ISR(void)		处理EINT4_7的中断服务程序，第10组负责	*
用于处理键盘的中断

其中*表示已完成的