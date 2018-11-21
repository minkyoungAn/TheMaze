#include "2450addr.h"
#include "my_lib.h"
#include "option.h"
#include "stdarg.h"

void Key_EINT2_ISR(void) __attribute__ ((interrupt ("IRQ")));;
void Key_EINT3_ISR(void) __attribute__ ((interrupt ("IRQ")));;
void Key_EINT4_6_ISR(void) __attribute__ ((interrupt ("IRQ")));;

extern unsigned char game_play;

unsigned long int old_key_interval_EINT2;
unsigned long int new_key_interval_EINT2;

unsigned long int old_key_interval_EINT3;
unsigned long int new_key_interval_EINT3;

unsigned long int old_key_interval_EINT4_6;
unsigned long int new_key_interval_EINT4_6;

unsigned int interval;
#define KEY_INTERVAL	5000

void Key_EINT2_ISR(void)
{
	rINTMSK1 |= (0x7<<2);
	rINTMSK1  |= (0x1<<10);

	//Uart_Printf("1\n");
	
	new_key_interval_EINT2 = (*(volatile unsigned long int *)0x57000090);

	if (new_key_interval_EINT2 > old_key_interval_EINT2)
	{
		interval = new_key_interval_EINT2 - old_key_interval_EINT2;
	}
	else
	{
		interval = old_key_interval_EINT2 - new_key_interval_EINT2;
	}

	if (interval > KEY_INTERVAL)
	{
		old_key_interval_EINT2 = new_key_interval_EINT2;
		if (game_play == 2)
		{
			maze_character_move_left();
		}
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

	//Uart_Printf("2\n");
	new_key_interval_EINT3 = (*(volatile unsigned long int *)0x57000090);
	
	if (new_key_interval_EINT3 > old_key_interval_EINT3)
	{
		interval = new_key_interval_EINT3 - old_key_interval_EINT3;
	}
	else
	{
		interval = old_key_interval_EINT3 - new_key_interval_EINT3;
	}

	if (interval > KEY_INTERVAL)
	{
		old_key_interval_EINT3 = new_key_interval_EINT3;	
		if (game_play == 2)
		{
			maze_character_move_down();
		}
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

	new_key_interval_EINT4_6 = (*(volatile unsigned long int *)0x57000090);
	
	if (new_key_interval_EINT4_6 > old_key_interval_EINT4_6)
	{
		interval = new_key_interval_EINT4_6 - old_key_interval_EINT4_6;
	}
	else
	{
		interval = old_key_interval_EINT4_6 - new_key_interval_EINT4_6;
	}

	if (interval > KEY_INTERVAL)
	{
		old_key_interval_EINT4_6 = new_key_interval_EINT4_6;		
		#if 1
		if(rEINTPEND == 0x10){
			//Uart_Printf("3\n");
			if (game_play == 2)
			{
				maze_character_move_up();
			}		
		}
		else if(rEINTPEND == 0x20){
			//Uart_Printf("4\n");
			if (game_play == 2)
			{
				maze_character_move_right();
			}		
		}
		else if(rEINTPEND == 0x40){
			//Uart_Printf("5\n");
			if (game_play == 3)
			{
				game_play = 4;
			}		
		}
		#endif
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
	// 1. GPF2~6 interrupt enable
	rGPFCON |= (0x2aa<<4);
	rEXTINT0 |= 0x3<<8;
	rEXTINT0 |= 0x3<<12;
	rEXTINT0 |= 0x3<<16;
	rEXTINT0 |= 0x3<<20;
	rEXTINT0 |= 0x3<<24;
	
	// 3. GPF7, GPG0 LOW(0)
    rGPFDAT &= ~(0x1<<7);
	rGPGDAT &= ~(0x1<<0);

	/* EINT [2:6] */
	rINTMOD1 &= ~ 0x7<<2;
	rINTMSK1 &= ~ 0x7<<2;
	rEINTMASK &= ~ 0x7<<4;
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
