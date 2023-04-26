Mode_USR	EQU     0x10	;开放IRQ、FIQ中断
Mode_SVC 	EQU     0xD3	;关闭IRQ、FIQ中断
Mode_SYS	EQU     0xDF	;关闭IRQ、FIQ中断
Mode_IRQ	EQU     0xD2	;关闭IRQ、FIQ中断

	GBLS C_Entry		;定义一个全局的字符串符号变量
	GBLS	C_EINT0
	GBLS	C_EINT4_7
	GBLS	INIT_Clock
	GBLS	INIT_MemSetup
	GBLS	INIT_Enternet
	GBLS	INIT_LCD_1
	GBLS	INIT_LCD_2
	GBLS	INIT_NORFlash
	GBLS	INIT_keyboard_INT
	GBLS	INIT_SDCARD	
C_EINT0		SETS	"Handle_EINT0"
C_EINT4_7	SETS	"Key_ISR"
INIT_Clock	SETS	"Clock_init"
INIT_MemSetup	SETS	"MemSetup"
INIT_Enternet	SETS	"dm_init"
INIT_LCD_1	SETS	"lcdpininit"
INIT_LCD_2	SETS	"lcdctrlinit"
INIT_NORFlash	SETS	"Init_NORFlash"
INIT_keyboard_INT	SETS	"KetINIT_Init"
INIT_SDCARD	SETS	"SD_Init"
C_Entry SETS "C_Interrupt"
	IMPORT $C_Entry;$表示用其值代换符号变量，因而得到常量C_Interrupt
	IMPORT $C_EINT0
	IMPORT $C_EINT4_7
	IMPORT $INIT_Clock
	IMPORT $INIT_MemSetup
	IMPORT $INIT_Enternet
	IMPORT $INIT_LCD	_1
	IMPORT $INT_LCD_2
	IMPORT $INIT_NORFlash
	IMPORT $INIT_keyboard_INT
	IMPORT $INIT_SDCARD
	GET		2440Reg_addr.s
	AREA MyCode,CODE,READONLY
	ENTRY			;设置中断向量表
	B	ResetHandler		;Reset中断服务程序
	B	.			;handlerUndef
	B	.			;SWI interrupt handler
	B	.			;handlerPAbort
	B	.			;handlerDAbort
	B	.			;handlerReserved
	B	HandlerIRQ		;HandlerIRQ, INTMOD复位为0
	B	.			;HandlerFIQ

ResetHandler
	B	$INIT_Clock	;初始化看门狗、时钟
	B	$INIT_MemSetup	;初始化SDRAM
	BL	$INIT_Enternet	;初始化以太网
	BL	$INIT_LCD_1			;初始化LCD
	BL	$INIT_LCD_2
	BL	$INIT_NORFlash	;初始化NOR Flash
	BL	$INIT_keyboard_INT	;初始化键盘
	BL	$INIT_SDCARD		;初始化SD Card
	LDR	SP,=SvcStackSpace	;设置管理模式堆栈
	MSR	CPSR_c,#Mode_IRQ
	LDR	SP,=IrqStackSpace	;设置IRQ中断模式堆栈
	MSR 	CPSR_c,#Mode_USR
	LDR	SP,=UsrStackSpace	;设置用户与系统模式堆栈
		;进行中断的初始化
	LDR	R0,=GPFCON
	LDR	R1,=0xFFFFAAFE			;设置用GPF0和GPF4-7做外部中断输入引脚，对应EINT0和EINT4-7中断
	STR	R1,[R0]
	
	LDR	R0,EXTINT0
	LDR R1,=0x0		;表示EINT0/4-7是低电平触发
	STR	R1,[R0]
	
	LDR	R0,=INTMSK
	LDR	R1,=0xFFFFFFEE	;表示EINT0和EINT4-7可以中断
	STR	R1,[R0]
	
	LDR R0,=pEINT0	;EINT0在中断入口散转表中地址
	LDR	R1,=EINT0	;中断服务程序入口
	STR	R1,[R0]		
	
	
	LDR R0,=pEINT4_7	;EINT4_7在中断入口散转表中地址
	LDR	R1,=EINT4_7	;中断服务程序入口
	STR	R1,[R0]
	
	BL	Loop

Loop
	NOP
	B Loop	;死循环，被中断打断
		


HandlerIRQ
		SUB	LR, LR, #4	;计算返回地址
		STMFD	SP!, { LR }	;保存断点到IRQ模式的堆栈
		LDR	LR,=Int_Return	;修改LR，执行完中断后返回到Int_Return处 
		LDR	R0,=INTOFFSET	;取得中断源的编号
		LDR	R1,[R0]
		LDR	R2,= Int_EntryTable 
		LDR	PC,[R2,R1,LSL#2]	;根据散转表跳转
Int_Return	
		LDMFD  	SP!, { PC }^	
	

EINT0	;为处理传感器的中断服务程序
	STMFD	SP!,{LR}
	B	$C_EINT0
	LDMFD	SP!,{PC}		;由EINT0处理程序返回IRQ中断处理程序

EINT4_7	;为处理键盘的中断处理程序
	STMFD	SP!,{LR}
	B	$C_EINT4_7
	LDMFD	SP!,{PC}

	
		AREA    	MyRWData, DATA, READWRITE	;设置RW Base=0x33ffe700
Int_EntryTable
		;中断散转表，共32个入口
pEINT0	DCD	0
pEINT1	DCD	0
pEINT2	DCD	0
pEINT3	DCD	0
pEINT4_7	DCD	0
pEINT8_23	DCD	0
pINT_CAM	DCD	0
pnBATT_FLT	DCD	0
pINT_TICK	DCD	0
pINT_WDT	DCD	0
pINT_TIMER0	DCD	0
pINT_TIMER1	DCD	0
pINT_TIMER2	DCD	0
pINT_TIMER3	DCD	0
pINT_TIMER4	DCD	0
pINT_UART2	DCD	0
pINT_LCD	DCD	0
pINT_DMA0	DCD	0
pINT_DMA1	DCD	0
pINT_DMA2	DCD	0
pINT_DMA3	DCD	0
pINT_SDI	DCD	0
pINT_SPI0	DCD	0
pINT_UART1	DCD	0
pINT_NFCON	DCD	0
pINT_USBD	DCD	0
pINT_USBH	DCD	0
pINT_IIC	DCD	0
pINT_UART0	DCD	0
pINT_SPI1	DCD	0
pINT_RTC	DCD	0
pINT_ADC	DCD	0
		AREA    	MyZIData, DATA, READWRITE, NOINIT,ALIGN =8
;此处的对齐就不仅仅是为了方便栈区空间计算，同时也为C语言定义的全局变量留出存储空间
		;设置各模式栈区空间
			SPACE	0x100*4	;管理模式堆栈空间
		SvcStackSpace	SPACE	0x100*4	;中断模式堆栈空间
		IrqStackSpace	SPACE	0x100*4	;用户（系统）模式堆栈空间
		UndtStackSpace	SPACE	0x100*4	;用户（系统）模式堆栈栈顶指针
		UsrStackSpace
		END 
		
