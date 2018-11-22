/* ============================================================================ */
/*	      tetris_main.c : 미로 게임 프로그램		*/
/* ============================================================================ */

#include <stdlib.h>
#include <time.h>
#include "2450addr.h"
#include "my_lib.h"
#include "macro.h"
#include "tetris.h"

#include "images.h"

void MMU_Init(void);

void HW_Initial(void);
void Show_Welcome(char * msg);

void Draw_block(unsigned short xPos, unsigned short yPos, unsigned short color, unsigned char type);	/* draw a block */
void Draw_board(void);				// draw outline of game board
void Display_bitmap(void);
void Display_text(void);

/* ---------------------------------------------------------------------------- */


#define WHITE		0xFFFF			/* 16 bit 8 color data */
#define BLACK		0x0000
#define RED		    0xF800
#define GREEN		0x07E0
#define BLUE		0x001F
#define YELLOW		0xFFE0			// Yellow = Red + Green
#define CYAN		0x07FF			// Cyan = Green + Blue
#define MAGENTA		0xF81F			// Magenta = Red + Blue
#define BROWN		0xA145
#define KHAKI		0xF731
#define OLIVE		0x8400
#define ORANGE		0xFD20
#define PINK		0xFB56
#define PURPLE		0x8010
#define SILVER		0xC618
#define VIOLET		0xEC1D
#define TRANSPARENT	0x0821

#define LCD_XSIZE 		(480)	
#define LCD_YSIZE 		(272)

#define	Xbase	    9 				// x base position of board
#define	Ybase		18			// y base position of board

/*
#define Left		0x04			// KEY1 for left
#define Down		0x08			// KEY2 for down
#define UP		0x10			// KEY3 for up or game start
#define Right		0x20			// KEY4 for right
*/

#define LEFT		(1)	
#define DOWN	    (2) 		
#define UP		    (3)	
#define RIGHT	    (4)			


signed char screen[20][10];			// game screen data
signed char board[2][12] = { "333333333333",	// constant data for board outline
                             "322222222223" };

signed char tetris[7][4][4] = {			/* define tetromino */
		{ "1100",			// 0 = O type (White)
		  "1100",
		  "0000",
		  "0000" },
		{ "1000",			// 1 = I type (Blue)
		  "1000",
		  "1000",
		  "1000" },
		{ "1300",			// 2 = T type (Yellow)
		  "1100",
		  "1300",
		  "0000" },
		{ "3100",			// 3 = Z type (Magenta)
		  "1100",
		  "1300",
		  "0000" },
		{ "1300",			// 4 = S type (Red)
		  "1100",
		  "3100",
		  "0000" },
		{ "1100",			// 5 = L type (Green)
		  "3100",
		  "3100",
		  "0000" },
		{ "1100",			// 6 = J type (Cyan)
		  "1300",	
		  "1300",
		  "0000" }};

// Global Variables Declaration
unsigned int touch_key = -1;
unsigned int left_press = 0;
unsigned int right_press = 0;
unsigned int down_press = 0;
unsigned int up_press = 0;


unsigned int start_press = 0;
unsigned int reset_press = 0;

// 터치패드가 눌림을 알수 있는 값
volatile int Touch_pressed = 0;
// ADC 값
volatile int ADC_x=0, ADC_y=0;

// Calibration 정보 저장 값
volatile int Cal_x1=848;
volatile int Cal_y1=656;
volatile int Cal_x2=186;
volatile int Cal_y2=349; 

// 좌표 변환 값
volatile int Touch_x, Touch_y;
// Calibration 완료 값
volatile unsigned int Touch_config=1;

void Touch_ISR(void) __attribute__ ((interrupt ("IRQ")));

void Touch_ISR()
{
	rINTSUBMSK |= (0x1<<9);
	rINTMSK1 |= (0x1<<31);	
	
	/* TO DO: Pendng Clear on Touch */	
	rSUBSRCPND |= (0x1<<9);
	rSRCPND1 |= (0x1<<31);
	rINTPND1 |= (0x1<<31);
	
	// Touch UP
	if(rADCTSC&0x100)
	{
		rADCTSC&=0xff;
		Touch_pressed = 0;
	}
	// Touch Down
	else 
	{
		rADCTSC=(0<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(0);
		// SADC_ylus Down,Don't care,Don't care,Don't care,Don't care,XP pullup Dis,Auto,No operation
		rADCCON|=0x1;
		while(rADCCON & 0x1);
		while(!(0x8000&rADCCON));
		ADC_x=(int)(0x3ff&rADCDAT0);
		ADC_y=(int)(0x3ff&rADCDAT1);
		// Touch calibration complete
		if(Touch_config)
		{
			Touch_y=(ADC_y-Cal_y1)*(LCD_YSIZE-10)/(Cal_y2-Cal_y1)+5;
			Touch_x=(ADC_x-Cal_x2)*(LCD_XSIZE-10)/(Cal_x1-Cal_x2)+5;
			Touch_x=LCD_XSIZE-Touch_x;
			if(Touch_x<0) Touch_x=0;
			if(Touch_x>=LCD_XSIZE) Touch_x=LCD_XSIZE-1;
			if(Touch_y<0) Touch_y=0;
			if(Touch_y>=LCD_YSIZE) Touch_y=LCD_YSIZE-1;
		}
		// before calibration		
		else
		{
			Touch_x = ADC_x;
			Touch_y = ADC_y;
		}

		rADCTSC=(1<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2)|(3);
		// SADC_ylus Up,Don't care,Don't care,Don't care,Don't care,XP pullup En,Normal,Waiting mode
		Touch_pressed = 1; 

	}

    

	rINTSUBMSK &= ~(0x1<<9);
	rINTMSK1 &= ~(0x1<<31);
}
/* ---------------------------------------------------------------------------- */

unsigned char game_play; //1ready:2play:3clear:4restart

int Main(void)
{
    unsigned int  score = 0;
    unsigned char x, y, key = 0, t_ch = 0, current_tetris = 0, next_tetris;
    signed char pos_x, pos_y = 0, del_pos_x, del_pos_y;
    signed char tetris_width = 0, tetris_height = -1, del_line;

    signed char tetris_end_flag = 0, game_end_flag = 0, time_move_flag = 0, key_move_flag = 0;
    signed char left_limit_flag = 0, right_limit_flag = 0, y_limit_flag = 0, rotate_limit_flag = 0;
    signed char temp_tetris[4][4], del_tetris[4][4];

	game_play = 1;
    HW_Initial();
    Tick_Count_Start();
    Timer0_ISR_Init();

	Show_Welcome("LCD Test");

	Lcd_BackLight_Enable(0);
	Display_bitmap();
	Display_text();	
	Lcd_BackLight_Enable(1);

	Lcd_Copy(0,1);

	while(1)
	{
    	make_maze();
    	display_maze();

		game_play = 2;
	    while(1)
	    {
	    	if (game_play == 3)
	    	{

	    		Lcd_Printf(108,140,SILVER,BLACK,2,2,"GAME CLEAR !!!");
	    		break;
	    	}
	    }

	    while(1)
	    {
	    	if(game_play == 4)
	    	{
	    		free_maze();
	    		break;
	    	}
	    }
    }

}

void HW_Initial(void)
{
	// 필요한 초기화 루틴들을 기입한다
	Exception_Init();
	Uart_Init(115200);	

    Init_Key();
	Timer0_Init();
	Graphic_Init();
	
	//Touch_Isr_Init(Touch_ISR);
	MMU_Init();
}

void Show_Welcome(char * msg)
{
	int a, j; 
	
	Uart_Printf("\n%s\n", msg);
	
	for(a=0;a<2;a++)
	{
		Led_Display(7);
		for(j=0; j<0x0ffff; j++);
		Led_Display(0);
		for(j=0; j<0x0ffff; j++);
	}
}

/* ---------------------------------------------------------------------------- */

void Display_text(void)
{
	Lcd_Printf(10, 130, KHAKI, BLACK, 1, 1, "KEY6");
    Lcd_Printf(55, 130, ORANGE, BLACK, 1, 1, "<-");
    Lcd_Printf(10, 150, KHAKI, BLACK, 1, 1, "KEY7");
    Lcd_Printf(55, 150, ORANGE, BLACK, 1, 1, "Do");
    Lcd_Printf(10, 170, KHAKI, BLACK, 1, 1, "KEY8");
    Lcd_Printf(55, 170, ORANGE, BLACK, 1, 1, "Up");
    Lcd_Printf(10, 190, KHAKI, BLACK, 1, 1, "KEY9");
    Lcd_Printf(55, 190, ORANGE, BLACK, 1, 1, "->");
    Lcd_Printf(10, 210, KHAKI, BLACK, 1, 1, "KEY10");
    Lcd_Printf(10, 230, ORANGE, BLACK, 1, 1, "Restart");
}

void Display_bitmap(void)
{
    int xtmp, ytmp;
    int height_tmp=0;
	int width_tmp=0;

	Lcd_Select_Frame_Buffer(0);
	Lcd_Clr_Screen(BLACK);
	Lcd_Display_Frame_Buffer(0);

	Lcd_Select_Frame_Buffer(1);
	Lcd_Clr_Screen(BLACK);
	Lcd_Display_Frame_Buffer(1);

	Lcd_Select_Frame_Buffer(0);
	
    Lcd_Get_Info_BMP(&xtmp, &ytmp, BG);
    Lcd_Get_Info_BMP(&xtmp, &ytmp, tau);
    Lcd_Get_Info_BMP(&xtmp, &ytmp, eta);
	Lcd_Get_Info_BMP(&xtmp, &ytmp, epsilon);
	Lcd_Get_Info_BMP(&xtmp, &ytmp, alpha);
	Lcd_Get_Info_BMP(&xtmp, &ytmp, mu);
	Lcd_Get_Info_BMP(&xtmp, &ytmp, epsilon2);
	Lcd_Get_Info_BMP(&xtmp, &ytmp, zeta);
	Lcd_Get_Info_BMP(&xtmp, &ytmp, paw);
	Lcd_Get_Info_BMP(&xtmp, &ytmp, clear);

    Lcd_Draw_BMP(0, 0, BG);
	
	height_tmp = 20;

	Lcd_Draw_BMP(375, height_tmp, tau);

	Lcd_Draw_BMP(375, height_tmp+=37, eta);

	Lcd_Draw_BMP(375, height_tmp+=37, epsilon);
	
	height_tmp = 20;

	Lcd_Draw_BMP(420, height_tmp+=37, mu);

	Lcd_Draw_BMP(420, height_tmp+=37, alpha);

	Lcd_Draw_BMP(420, height_tmp+=37, zeta);

	Lcd_Draw_BMP(420, height_tmp+=37, epsilon2);

	Lcd_Draw_BMP(390, height_tmp+=37, paw);
	
	//Lcd_Draw_BMP(220,20, timer);
}
