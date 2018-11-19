#include "2450addr.h"
#include "my_lib.h"
#include "option.h"
#include "stdarg.h"


void Init_Key(void)
{
    // 1. GPF2~6를 input으로 설정
    rGPFCON &= ~(0x3ff << 4);
	
    // 2. GPF7, GPG0을 output으로 설정
	rGPFCON = (rGPFCON & ~(0x3<<14)) | (0x1<<14);
	rGPGCON = (rGPGCON & ~(0x3<<0)) | (0x1<< 0);
	
    // 3. GPF7, GPG0에 LOW(0) 출력
    rGPFDAT &= ~(0x1<<7);
	rGPGDAT &= ~(0x1<<0);
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
