#include "2450addr.h"
#include "my_lib.h"
#include "option.h"
#include "stdarg.h"

void Key_EINT2_ISR(void) __attribute__ ((interrupt ("IRQ")));;
void Key_EINT3_ISR(void) __attribute__ ((interrupt ("IRQ")));;
void Key_EINT4_6_ISR(void) __attribute__ ((interrupt ("IRQ")));;

void Key_EINT2_ISR(void)
{
	rINTMSK1 |= (0x7<<2);
	rINTMSK1  |= (0x1<<10);

	if(rGPFDAT == 0xff){
		Uart_Printf("key 1 pushed!!!\n");
	}
	else{
		Uart_Printf("key 6 pushed!!!\n");
	}


	rSRCPND1 |= (0x7<<2);
	rINTPND1 |= (0x7<<2);
	rINTMSK1 &=~(0x7<<2);
	rINTMSK1  &= ~(0x1<<10);
}
void Key_EINT3_ISR(void)
{
	rINTMSK1 |= (0x7<<2);
	rINTMSK1  |= (0x1<<10);
	if(rGPFDAT == 0xff){
		Uart_Printf("key 2 pushed!!!\n");
	}
	else {
		Uart_Printf("key 7 pushed!!!\n");
	}
	

	rSRCPND1 |=(0x7<<2);
	rINTPND1 |=(0x7<<2);
	rINTMSK1&=~(0x7<<2); 
	rINTMSK1  &= ~(0x1<<10);
}
void Key_EINT4_6_ISR(void)
{
	rEINTMASK |= 0x7<<4; // masking 
	rINTMSK1 |= (0x7<<2);
	rINTMSK1  |= (0x1<<10);	

	if(rEINTPEND == 0x10){
		if( rGPFDAT == 0xff ){
			Uart_Printf("key 3 pushed!!!\n");
		}
		else {
			Uart_Printf("key 8 pushed!!!\n");
		}
	}
	else if(rEINTPEND == 0x20){
		if( rGPFDAT == 0xff ){
			Uart_Printf("key 4 pushed!!!\n");
		}
		else {
			Uart_Printf("key 9 pushed!!!\n");
		}
	}
	else if(rEINTPEND == 0x40){
		if( rGPFDAT == 0xff ){
			Uart_Printf("key 5 pushed!!!\n");
		}
		else {
			Uart_Printf("key 10 pushed!!!\n");
		}
	}

	rEINTPEND |= 0x3<<4; // pending	
	rSRCPND1 |=(0x3<<2);
	rINTPND1 |= (0x3<<2);

	

	rEINTMASK &= ~0x7<<4; // not masking 
	rINTMSK1&=~(0x7<<2);
	rINTMSK1  &= ~(0x1<<10);	
}


void Init_Key(void)
{
	pISR_EINT2 = Key_EINT2_ISR;
	pISR_EINT3 = Key_EINT3_ISR;
	pISR_EINT4_7 = Key_EINT4_6_ISR;

	/* GPFCON - EINT Mode GPF [2:6] */
	rGPFCON |= (0x2aa<<4);
	rEXTINT0 |= 0x3<<8;
	rEXTINT0 |= 0x3<<12;
	rEXTINT0 |= 0x3<<16;
	rEXTINT0 |= 0x3<<20;
	rEXTINT0 |= 0x3<<24;
	
	/* EINT [2:6] */
	rINTMOD1 &= ~ 0x7<<2;
	rINTMSK1 &= ~ 0x7<<2;
	rEINTMASK &= ~ 0x7<<4;
	
	#if 0
    // 1. GPF2~6를 input으로 설정
    rGPFCON &= ~(0x3ff << 4);
	
    // 2. GPF7, GPG0을 output으로 설정
	rGPFCON = (rGPFCON & ~(0x3<<14)) | (0x1<<14);
	rGPGCON = (rGPGCON & ~(0x3<<0)) | (0x1<< 0);
	
    // 3. GPF7, GPG0에 LOW(0) 출력
    rGPFDAT &= ~(0x1<<7);
	rGPGDAT &= ~(0x1<<0);
	#endif
}

int Get_Key_byPolling(void)
{
    //*******************************************************
	int i;
	
	rGPFDAT |= 0x1<<7;
	rGPGDAT &= ~(0x1<<0);
	
	while(1)
	{
		for(i=0; i<5; i++)
		{
			if( (rGPFDAT & (0x1 << (i+2))) == 0)
			{
				if(rGPFDAT & (0x1<<7))
				{
					return (i+6);
				}
				
				else
				{
					return (i+1);
				}
			    	
			}
		}
		rGPFDAT ^= 0x1<<7;
		rGPGDAT ^= 0x1<<0;
	}
	
	//*******************************************************
}


unsigned char Key_input(void)
{
    int i, pressed_key = 0;

    Uart_Printf("Press key : ");
    pressed_key = Get_Key_byPolling();
    Uart_Printf("[%d] pressed !! \n", pressed_key);
    Timer0_Delay(200); 
}
