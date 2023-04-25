;==========================
;filename 2440IO_addr.inc
;==========================
;=================
; Memory control 
;=================
BWSCON       EQU  0x48000000     ;Bus width & wait status
BANKCON0    EQU  0x48000004     ;Boot ROM control
BANKCON1    EQU  0x48000008     ;BANK1 control
BANKCON2    EQU  0x4800000c     ;BANK2 cControl
BANKCON3    EQU  0x48000010     ;BANK3 control
BANKCON4    EQU  0x48000014     ;BANK4 control
BANKCON5 	EQU		0x48000018 	;BANK5 Control
BANKCON6 	EQU		0x4800001C 	;BANK6 Control
BANKCON7 	EQU		0x48000020 	;BANK7 Control
REFRESH 	EQU		0x48000024 	;DRAM/SDRAM Refresh Control
BANKSIZE 	EQU		0x48000028 	;Flexible Bank Size
MRSRB6 		EQU		0x4800002C 	;Mode register set for SDRAM BANK6
MRSRB7 		EQU		0x48000030 	;Mode register set for SDRAM BA


;==========================
;2440 I/O PORT address
;==========================
GPACON      EQU  0x56000000     ;Port A control
GPADAT      EQU  0x56000004     ;Port A data                  
GPBCON      EQU  0x56000010     ;Port B control
GPBDAT      EQU  0x56000014     ;Port B data
GPBUP       EQU  0x56000018     ;Pull-up control B
GPCCON 		EQU	0x56000020 	;Port C Control
GPCDAT		EQU	0x56000024	;Port C Data
GPCUP		EQU	0x56000028	;Pull-up Control C
GPDCON		EQU	0x56000030	;Port D Control
GPDDA1T		EQU	0x56000034	;Port D Data
GPDUP		EQU	0x56000038	;Pull-up Control D
GPECON		EQU	0x56000040	;Port E Control
GPEDAT		EQU	0x56000044	;Port E Data
GPEUP		EQU	0x56000048	;Pull-up Control E
GPFCON		EQU	0x56000050	;Port F Control
GPFDAT		EQU	0x56000054	;Port F Data
GPFUP		EQU	0x56000058	;Pull-up Control F
GPGCON		EQU	0x56000060	;Port G Control
GPGDAT		EQU	0x56000064	;Port G Data
GPGUP		EQU	0x56000068	;Pull-up Control G
GPHCON		EQU	0x56000070	;Port H Control
GPHDAT		EQU	0x56000074	;Port H Data
GPHUP		EQU	0x56000078	;Pull-up Control H
GPJCON		EQU	0x560000D0	;Port J Control
GPJDAT		EQU	0x560000D4	;Port J Data
GPJUP		EQU	0x560000D8	;Pull-up Control J
MISCCR		EQU	0x56000080	;Miscellaneous Control
DCLKCON		EQU	0x56000084	;DCLK0/1 Control
EXTINT0		EQU	0x56000088	;External Interrupt Control Register 0
EXTINT1		EQU	0x5600008C	;External Interrupt Control Register 1
EXTINT2		EQU	0x56000090	;External Interrupt Control Regist


;==========================
; CLOCK & POWER MANAGEMENT
;==========================
LOCKTIME    EQU  0x4c000000     ;PLL lock time counter
MPLLCON     EQU  0x4c000004     ;MPLL Control
UPLLCON     EQU  0x4c000008     ;UPLL Control
CLKCON      EQU  0x4c00000c     ;Clock generator control
CLKSLOW     EQU  0x4c000010     ;Slow clock control
CLKDIVN     EQU  0x4c000014     ;Clock divider control
;=================
; WATCH DOG TIMER
;=================
WTCON     EQU  0x53000000       ;Watch-dog timer mode
WTDAT     EQU  0x53000004       ;Watch-dog timer data
WTCNT     EQU  0x53000008       ;Eatch-dog timer count

;我自己加的
INTOFFSET	EQU	0X4A000014
	END