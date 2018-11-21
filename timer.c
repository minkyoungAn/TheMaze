#include "2450addr.h"
#include "option.h"
#include "my_lib.h"

extern unsigned char game_play;
unsigned int play_time;

void Timer0_ISR(void)
{
	int min;
	int sec;
	int mil;
	
	rINTMSK1 |= BIT_TIMER0;

	if (game_play == 2)
	{
		play_time++;

		sec = (play_time%60);
		min = (play_time/60);
		
		Lcd_Printf(270,30,0xFFFF,0x0000,1,1,"%02d:%02d",min,sec);
		Lcd_Display_Frame_Buffer(1);
	}
	else
	{
		if ((game_play == 3) || (game_play == 4))
		{
			play_time = 0;
		}
	}
	
	rSRCPND1 |= BIT_TIMER0;
	rINTPND1 |= BIT_TIMER0;
	
	rINTMSK1 &= ~BIT_TIMER0;
}

void Timer0_ISR_Init(void)
{
	rINTMSK1 &= ~BIT_TIMER0;

	rSRCPND1 |= BIT_TIMER0;
	rINTPND1 |= BIT_TIMER0;
	
	pISR_TIMER0 = Timer0_ISR;

	Timer0_Delay(1000);
}

void Timer0_Init(void)
{
	/* 
	* 	Timer0 Init 
	* Prescaler value : 255, dead zone length = 0
	* Divider value : 1/16, no DMA mode
	* New frequency : (PCLK/(Prescaler value+1))*Divider value = (66Mhz/(256))*(1/16)
	*				= 16.113Khz(16113Hz)
	*/
	rTCFG0 = (0<<8)|(0xff); 
	rTCFG1 = (0<<20)|(3); 
	
	/* TCON설정 :Dead zone disable,  auto reload off, output inverter off
	*  manual update no operation, timer0 stop, TCNTB0=0, TCMPB0=0
	*/
	//rTCON   = (0<<4)|(0<<3)|(0<<2)|(0<<1)|(1);
	rTCON   = (1<<3);
	rTCNTB0 = 0;
	rTCMPB0 = 0;

 	//Timer0_ISR_Init();
}

void Timer0_Delay(int msec)
{
	/*
	* 1) TCNTB0설정 : 넘겨받는 data의 단위는 msec이다.
	*                  따라서 msec가 그대로 TCNTB0값으로 설정될 수는 없다.
	* 2) manual update후에  timer0를 start시킨다. 
	* 	 note : The bit has to be cleared at next writing.
	* 3) TCNTO0값이 0이 될때까지 기다린다. 	
	*/
	rTCNTB0 = 16.113*msec;	

	rTCON |= (1<<1)|(0);
	rTCON &= ~(1<<1);
	
	rTCON |= 1;	
	
	//while(rTCNTO0 != 0);
	//rTCON &= ~1;	
}


//
// Use Timer0 for PC_ElapsedStart
// Timer INPUT clock Resolution 2.0625MHz
void ElapseTimer_Start(void)
{
 rTCFG0 = (rTCFG0 & ~0xff) | 1;
 rTCFG1 = (rTCFG1 & ~0xf) | 3; 
 
 /* TCON설정 :Dead zone disable,  auto reload on, output inverter off
 *  manual update no operation, timer0 stop, TCNTB0=0, TCMPB0 =0
 */
 rTCON  = (rTCON & ~((1<<4)|(1<<2)|(1<<1)|(1))) | (1<<3);

 rTCNTB0 = 0;
 rTCMPB0 = 0;

 rTCNTB0 = 0xffff; // initial value(65535) for timer

 rTCON |= (1<<1)|(0);
 rTCON &= ~(1<<1);
 rTCON |= 1;
}

//
// Use Timer0 for PC_ElapsedStart
// Timer Resolution 1.5625MHz
unsigned int ElapseTimer_Stop(void)
{
 rTCON &= ~1;

 return (0xffff - rTCNTO0);
}

