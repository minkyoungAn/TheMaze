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
void Display_score(unsigned int score);			// display score
void Draw_game_tetris(void);			// draw tetromino of game
void Draw_next_tetris(signed char next);		// draw next tetromino
void Add_tetris(signed char (*src)[4], signed char *xPos, signed char *yPos, unsigned char number); // add tetromino
void Delete_tetris(signed char (*src)[4], signed char *xPos, signed char *yPos);	// delete tetromino
void Rotate_tetris(signed char (*src)[4], unsigned char size, signed char *width, signed char *height); // rotate tetromino
signed char Drop_tetris(signed char (*src)[4], signed char xPos, signed char yPos, signed char width, signed char height); // drop tetromino
signed char Check_line(signed char yPos, signed char height);	// check if line is OK
void Check_limit(signed char (*src)[4], signed char *x, signed char *y, signed char *tetris_end, signed char *left_limit,
                 signed char *right_limit, signed char *y_limit, signed char *rotate_limit, signed char current);

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

    if (left_press == 1) {
        touch_key = LEFT;
        left_press = 0; 
    } else if (right_press == 1) {
        touch_key = RIGHT;
        right_press = 0;
    } else if (down_press == 1) {
        touch_key = DOWN;
        down_press = 0;
    } else if (up_press == 1) {
        touch_key = UP;
        up_press = 0;
    } else if (start_press == 0) {
        start_press = 1;
    } else if (reset_press == 0) {
        reset_press = 1;
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

    while(1)
    {
		Display_bitmap();
		Display_text();
    	make_maze();
    	display_maze();

		game_play = 2;
	    while(1)
	    {
	    	if (game_play == 3)
	    	{

	    		Lcd_Printf(85,140,SILVER,BLACK,2,2,"GAME CLEAR !!!");
	    		break;
	    	}
	    }

	    while(1)
	    {
	    	if(game_play == 4)
	    	{
	    		break;
	    	}
	    }
    }

    //TFT_string(6,18,Green,Black, "Press KEY3");	// wait KEY3 to start
    //TFT_string(6,20,Green,Black, "to start !");
    //Lcd_Printf(6, 18, WHITE, BLACK, 1, 1, "Press KEY3");
    //Lcd_Printf(85, 150, WHITE, BLACK, 1, 1, "to start !");


    //while(key != Rotate) {
    //    key = Key_input();
    //}
    //while (start_press == 0) {
    //    if(Touch_pressed == 1) {
    //        if((Touch_x >= 330) && (Touch_x <= 380) && (Touch_y >= 180) && (Touch_y <= 230)) {
    //            Uart_Printf("\n To Start !"); 
    //        }
    //    }
    //}

    //srand(time(NULL));					// initialize random number
#if 0
    while (1) {
        for(y=0; y<20; y++)	{		// initialize game
            for(x=0; x<10; x++) {
                screen[y][x] = '2';
            }
        }

        Touch_ISR_Enable(1);
        score = 0;
        del_line = tetris_height = -1;
        game_end_flag = tetris_end_flag = y_limit_flag = rotate_limit_flag = 0;
        next_tetris = rand() % 7;	

        Draw_board();				// draw game board
        Display_bitmap();

        do {
            if(tetris_end_flag == 1) {
                Delete_tetris(del_tetris,&del_pos_x,&del_pos_y);
            } else {
                tetris_end_flag = left_limit_flag = right_limit_flag = y_limit_flag = rotate_limit_flag = 0;
            }

            if(y_limit_flag < 2) {
                left_limit_flag = right_limit_flag = rotate_limit_flag = 0;
            }

            if(tetris_end_flag == 0) {
                del_line = Check_line(pos_y,tetris_height);
                score += (del_line < 0) ? 0 : (del_line == 0) ? 10 : (del_line*del_line)*100;
                Display_score(score);		// display current score

                pos_x = 4;				// initialize start of current tetromino
                pos_y = 0;
                tetris_width = 0;
                tetris_height = -1;
                current_tetris = next_tetris;

                next_tetris = rand() % 7;		// get next tetromino number
                if(next_tetris == current_tetris) {
                    next_tetris = rand() % 7;
                }
                Draw_next_tetris(next_tetris);

                for(y=0; y<4; y++) {		// get tetromino
                    for(x=0; x<4; x++) {
                        temp_tetris[y][x] = tetris[current_tetris][y][x];
                        if(temp_tetris[y][x] == '1') {
                            if(tetris_width < x) tetris_width = x;
                            if(tetris_height < y) tetris_height = y;
                        }
                    }
                }
            } 

            Check_limit(temp_tetris,&pos_x,&pos_y,&tetris_end_flag,&left_limit_flag,&right_limit_flag,&y_limit_flag,&rotate_limit_flag,current_tetris);

            if(tetris_end_flag == 0 && pos_y == 0) {
                game_end_flag = 1;
                break;
            }
            if(y_limit_flag > 0 && key_move_flag == 1 && time_move_flag == 0) {
                y_limit_flag = 1;
            }

            if(pos_x+tetris_width > 9) {
                pos_x = 9-tetris_width;
            }
            Add_tetris(temp_tetris,&pos_x,&pos_y,current_tetris);

            del_pos_x = pos_x;
            del_pos_y = pos_y;
            
            for(y = 0; y < 4; y++) {
                for(x = 0; x < 4; x++) {
                    del_tetris[y][x] = temp_tetris[y][x];
                }
            }

            Draw_game_tetris();			// draw game status

            time_move_flag = 0;
            key_move_flag = 0;

            while(time_move_flag == 0 && key_move_flag == 0) {	
                //Delay_ms(10);
                Timer0_Delay(10); 
                if(t_ch < 12) {
                    t_ch++;		// check time move
                } else {
                	time_move_flag = 1;
                    t_ch = 0;
                    if(y_limit_flag == 0) pos_y++;
                }

                if(time_move_flag == 0 && key_move_flag == 0) {	// check key move
                	//key = Key_input();
                    if(Touch_pressed == 1) {
                        if((Touch_x >= 400) && (Touch_x <= 450) && (Touch_y >= 200) && (Touch_y <= 250)) {
                            Uart_Printf("\n LEFT Pressed"); 
                            left_press = 1;
                        }
                    }
                    if(Touch_pressed == 1) {
                        if((Touch_x >= 400) && (Touch_x <= 450) && (Touch_y >= 50) && (Touch_y <= 100)) {
                            Uart_Printf("\n RIGHT Pressed"); 
                            right_press = 1;
                        }
                    }
                    if(Touch_pressed == 1) {
                        if((Touch_x >= 400) && (Touch_x <= 450) && (Touch_y >= 140) && (Touch_y <= 190)) {
                            Uart_Printf("\n DOWN Pressed"); 
                            down_press = 1;
                        }
                    }
                    if(Touch_pressed == 1) {
                        if((Touch_x >= 330) && (Touch_x <= 380) && (Touch_y >= 140) && (Touch_y <= 190)) {
                            Uart_Printf("\n ROTATE Pressed"); 
                            rotate_press = 1;
                        }
                    }
                    //if(Touch_pressed == 1) {
                    //    if((Touch_x >= 330) && (Touch_x <= 380) && (Touch_y >= 25) && (Touch_y <= 75)) {
                    //        Uart_Printf("\n Reset"); 
                    //    }
                    //}
                    //if (reset_press == 1) break;

                    switch(touch_key) {
                        case LEFT:  
                            if(left_limit_flag == 0) {
                                pos_x--;
                                key_move_flag = 1;
                            }
                            break;

                        case RIGHT:	
                            if(right_limit_flag == 0) {
                                pos_x++;
                                key_move_flag = 1;
                            }
                            break;

                        case DOWN:  
                            if(y_limit_flag == 0) {
                                pos_y = Drop_tetris(temp_tetris,pos_x,pos_y,tetris_width,tetris_height);
                                //Beep();
                            }
                            key_move_flag = 1; 
                            break;

                        case ROTATE:
                            if(rotate_limit_flag == 0) {
                                if(current_tetris == 1) {
                                    Rotate_tetris(temp_tetris,4,&tetris_width,&tetris_height);	
                                } else {
                                    Rotate_tetris(temp_tetris,3,&tetris_width,&tetris_height);
                                    key_move_flag = 1;
                                }	
                            }
                            break;

                        default:	
                            key_move_flag = 0;
                            break;
                    }
                    touch_key = -1;
                }
            }
        } while(game_end_flag == 0);

        //TFT_string(5,38,Magenta,Black, "GAME OVER !!!");	// display game over
        Lcd_Printf(85, 240, MAGENTA, BLACK, 1, 1, "GAME OVER !!!");
        //Beep_3times();

        //TFT_string(6,18,Green,Black, "Press KEY3"); // wait KEY3 to start
        //TFT_string(6,20,Green,Black, "to start !");
        //Lcd_Printf(6, 18, GREEN, BLACK, 1, 1, "Press KEY3");
        //Lcd_Printf(6, 20, GREEN, BLACK, 1, 1, "to start !");

        //key = 0;

        //while(key != Rotate) {
        //    key = Key_input();
        //}

    }

	#endif
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

unsigned short block[2][12] = 
	{{ 0xFFF0,0xC030,0xA050,0x9090,0x8910,0x8610,	// block □
	   0x8610,0x8910,0x9090,0xA050,0xC030,0xFFF0 },
	 { 0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,	// block ■
	   0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0 }};


void Draw_block(unsigned short xPos,unsigned short yPos,unsigned short color,unsigned char type)	/* draw a block */
{
    unsigned char x, y;
    int i=0, j=0;

    for(y=0; y<12; y++) {
        //TFT_GRAM_address(xPos+1, yPos+y+1);
        i = xPos+1;
        j = yPos+y+1;
        for(x=0; x<12; x++) {
            if((block[type][y] << x) & 0x8000) Lcd_Put_Rvs_Pixel(i,j,color);
            else                               Lcd_Put_Rvs_Pixel(i,j,BLACK);
            i++;
        }
    }
}

void Draw_board(void)				/* draw outline of game board */
{
    unsigned char x, y, array;

    //TFT_color_screen(Black);			// clear screen
	Lcd_Clr_Screen(BLACK);

    //TFT_string(2,0,White,Blue, " OK-TFT 키트  TETRIS 게임 "); // display title
    Lcd_Printf(10, 0, WHITE, BLUE, 1, 1, "TETRIS TETRIS TETRIS TETRIS");

    for(y=0; y<22; y++) {			// main board box
        if(y == 0 || y == 21) array = 0;
        else                  array = 1;

        for(x=0; x<12; x++) {
            if(board[array][x] == '3') {
                Draw_block(Xbase+x*13, Ybase+y*13, WHITE, 0);
            }
        }
    }

    for(x=0; x<4; x++) {			// next tetromino box
        Draw_block(Xbase+12*13+x*13, Ybase+0,    WHITE, 0);
        Draw_block(Xbase+12*13+x*13, Ybase+7*13, WHITE, 0);
    }
    for(y = 0; y < 8; y++) {
        Draw_block(Xbase+16*13, Ybase+y*13, WHITE, 0);
    }

    //TFT_string(22,19,White, Magenta," 점수 ");	// display score message
    Lcd_Printf(180, 150, WHITE, MAGENTA, 1, 1, " 점수 ");

    //TFT_string(22,28,Green,  Black, "KEY1");	// display key
    //TFT_string(27,28,Magenta,Black, "<-");
    Lcd_Printf(170, 200, GREEN, BLACK, 1, 1, " KEY1 ");
    Lcd_Printf(210, 200, MAGENTA, BLACK, 1, 1, " <- ");

    //Line(217,231, 230,231, Magenta);
    //Lcd_Vline(217,217,230,WHITE);

    //TFT_string(22,30,Green,  Black, "KEY2");
    //TFT_string(27,30,Magenta,Black, "Dn");
    //TFT_string(22,32,Green,  Black, "KEY3");
    //TFT_string(27,32,Magenta,Black, "Ro");
    //TFT_string(22,34,Green,  Black, "KEY4");
    //TFT_string(27,34,Magenta,Black, "->");
    Lcd_Printf(178, 220, GREEN, BLACK, 1, 1, "KEY2");
    Lcd_Printf(218, 220, MAGENTA, BLACK, 1, 1, "Dn");
    Lcd_Printf(178, 240, GREEN, BLACK, 1, 1, "KEY3");
    Lcd_Printf(218, 240, MAGENTA, BLACK, 1, 1, "Ro");
    Lcd_Printf(178, 260, GREEN, BLACK, 1, 1, "KEY4");
    Lcd_Printf(218, 260, MAGENTA, BLACK, 1, 1, "->");
    
    //Line(217,279, 230,279, Magenta);
    //Lcd_Vline(217,217,230,WHITE);
}
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
    Lcd_Printf(10, 230, ORANGE, BLACK, 1, 1, "Reset");
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
	
	Lcd_Draw_BMP(120,20, timer);

	Lcd_Draw_BMP(120, 20, timer);
}

void Display_score(unsigned int score)			// display score
{
    //TFT_xy(22, 22);  TFT_color(Yellow,Black);
    //TFT_unsigned_decimal(score, 0, 5);
    Lcd_Printf(180, 180, YELLOW, BLACK, 1, 1, "%05d\n", score);
}

void Draw_game_tetris(void)			/* draw tetromino of game */
{
    unsigned char x, y;
    unsigned int color;

    for (y=0; y<sizeof(screen)/sizeof(*screen); y++) {
        for (x=0; x<10; x++) {
        	switch (screen[y][x]) {
                case '4'+0: color = WHITE;		break;
                case '4'+1: color = BLUE;		break;
                case '4'+2: color = YELLOW;		break;
                case '4'+3: color = MAGENTA;	break;
                case '4'+4: color = RED;		break;
                case '4'+5: color = GREEN;		break;
                case '4'+6: color = CYAN;		break;
                default:    color = BLACK;		break;
            }
            Draw_block(Xbase+13+x*13, Ybase+13+y*13, color, 1);
        }
    }
}

void Draw_next_tetris(signed char next)			/* draw next tetromino */
{
    unsigned char x, y, xcenter, ycenter;
    unsigned int  color = 0;
    int i=0, j=0;

    for(y=0; y<5*13; y++) {			// clear space for next tetromino
        //TFT_GRAM_address(Xbase+12*13, Ybase+2*13+y);
        i = Xbase+12*13;
        j = Ybase+2*13+y;
        for(x=0; x<4*13; x++) {
            //TFT_data(Black);
            Lcd_Put_Rvs_Pixel(i,j,BLACK);
            i++;
        }
    }

    switch(next) {					// get position of next tetromino
        case 0:	    xcenter = 13*13;   ycenter = 3*13;	    break;
        case 1:	    xcenter = 13*13+6; ycenter = 2*13;	    break;
        default:	xcenter = 13*13;   ycenter = 2*13+6;	break;
    }

    switch(next) {					// get color of next tetromino
        case 0:	color = WHITE;		break;
        case 1:	color = BLUE;		break;
        case 2:	color = YELLOW;		break;
        case 3:	color = MAGENTA;	break;
        case 4:	color = RED;		break;
        case 5:	color = GREEN;		break;
        case 6:	color = CYAN;		break;
    }

    for(y=0; y<4; y++) {			// draw next tetromino
        for(x=0; x<4; x++) {
            if(tetris[next][y][x] == '1') {
                Draw_block(Xbase+xcenter+x*13, Ybase+ycenter+y*13, color, 1);
            }
        }
    }
}

/* ---------------------------------------------------------------------------- */

void Add_tetris(signed char (*src)[4], signed char *xPos, signed char *yPos, unsigned char number)	/* add tetromino */
{
    signed char x, y, x1, y1 = -1, block_cnt = 0;

    for(y=3; y>=0; y--) {
        if(block_cnt < 4)   y1++;
        if(*yPos-y1 < 0)    break;

        block_cnt = 0;
        x1 = 0;

        for(x = 0; x < 4; x++) {
            switch(src[y][x]) {
                case '0':	
                    block_cnt++;
                    break;

                case '3':	
                    x1++;
                    break;

                default:	
                    screen[*yPos-y1][(*xPos)+x1] = '4'+number;
                    x1++;
                    break;
            }
        }
    }
}

void Delete_tetris(signed char (*src)[4], signed char *xPos, signed char *yPos)	/* delete tetromino */
{
    signed char x, y, x1, y1 = -1, block_cnt = 0;

    for(y=3; y>=0; y--) {
        if(block_cnt < 4) y1++;
        if(*yPos-y1 < 0)  break;

        block_cnt = 0;
        x1 = 0;
 
        for(x=0; x<4; x++) {
            switch(src[y][x]) {
                case '0': 
                    block_cnt++;
                    break;
                    
                case '3':	
                    x1++;
                    break;

                default:	
                    screen[*yPos-y1][(*xPos)+x1] = '2';
                    x1++;
                    break;
            }
        }
    }
}

void Rotate_tetris(signed char (*src)[4],unsigned char size, signed char *width, signed char *height)	/* rotate tetromino */
{
    unsigned char y, x, block_cnt, w_temp, h_temp, temp[4][4];

    w_temp = h_temp = 0;

    for(y=0; y<size; y++) {
        block_cnt = 0;
        for(x=0; x<size; x++) {
            temp[y][size-1-x] = src[x][y];
            if(temp[y][size-1-x] == '1' || temp[y][size-1-x] == '3') {
                block_cnt++;
                if(h_temp < y) {
                    h_temp = y;
                }
            }
        }
        if(w_temp < block_cnt-1) {
            w_temp = block_cnt-1;
        }
    }

    for(y=0; y<size; y++) {
        for(x=0; x<size; x++) {
            src[y][x] = temp[y][x];
        }
    }

    *width = w_temp;
    *height = h_temp;
}

signed char Drop_tetris(signed char (*src)[4], signed char xPos, signed char yPos, signed char width, signed char height)	/* drop tetromino */
{
    signed char x, y, block_cnt, xb_cnt = 0, return_y = 20, y_pos, y_pos_arr[4];

    for(x=0; x<4; x++) {
        block_cnt = 0;
        for(y=height; y>=0; y--) {
            if(src[y][x] == '1') {
                y_pos_arr[xb_cnt++] = yPos-(height-(y+block_cnt));
                break;
            } else if(src[y][x] == '0') {
                block_cnt++;	
            }
        }
    }

    for(x=xPos; x<=xPos+width; x++) {
        if(y_pos_arr[x-xPos] < 0) {
            y_pos = yPos;
        } else { 
            y_pos = y_pos_arr[x-xPos];
        }

        for(y=y_pos+1; y<20; y++) {
            if(screen[y][x] > '3') {
                y_pos_arr[x-xPos] = y-1+(yPos-y_pos_arr[x-xPos]);
                break;
            }
        }

        if(y > 19) {
            y_pos_arr[x-xPos] = 19;
        }
    }

    for(x=0; x<=width; x++) {
        if(return_y > y_pos_arr[x]) {
            return_y = y_pos_arr[x];
        }
    }

    return return_y;
}

signed char Check_line(signed char yPos, signed char height)	/* check if line is OK */
{
    signed char x, y, line, no_blank_flag = 0, del_line_count = 0;
    signed char del_line[4], temp[4][10];

    if(height < 0) return -1;

    for(y=yPos; y>=yPos-height; y--) {
        no_blank_flag = 0;
        for(x=0; x<10; x++) {			// check if horizontal line is OK
            if(screen[y][x] == '2') {
                no_blank_flag = 1;
            }
        }
        if(no_blank_flag == 0) {			// if yes, delete and sound
            del_line[del_line_count++] = y;
            //Beep_3times();
        }
    }

    if(del_line_count > 0) {	
        for(y=0; y<del_line_count; y++) {
            for(x=0; x<10; x++) {
                temp[y][x] = screen[del_line[y]][x];
            }
        }
    
        for(line=0; line<3; line++) {
            for(y=0; y<del_line_count; y++) {
                if(!(line % 2)) {
                    for(x=0; x<10; x++) {
                        screen[del_line[y]][x] = '2';
                    }
                } else {
                    for(x=0; x<10; x++) {
                        screen[del_line[y]][x] = temp[y][x];
                    }
                }
            }
            Draw_game_tetris();
            //Delay_ms(100);
            Timer0_Delay(100);
        }

        for(line=0; line<del_line_count; line++) {
            for(y=del_line[line]+line; y>0; y--) {
                for(x=0; x<10; x++) {
                    screen[y][x] = screen[y-1][x];
                }
                for(x=0; x<10; x++) {
                    screen[0][x] = '2';			
                }
            }
        }
    }

    return del_line_count;
}

void Check_limit(signed char (*src)[4], signed char *xPos, signed char *yPos, signed char *tetris_end, signed char *left_limit, 
                 signed char *right_limit, signed char *y_limit, signed char *rotate_limit, signed char current)
{
    signed char x, y, x_pos, y_pos = -1, width, height, size, block_cnt = 0;
    signed char default_y_limit = 0, blank_y_limit = 0, y_pos_limit = 0;
    signed char temp[4][4];

    if(current != 0) {
        if(current == 1) size = 4;
        else             size = 3;

        for(y=0; y<size; y++) {
            for(x=0; x<size; x++) {
                temp[y][x] = src[y][x];
            }
        }

        Rotate_tetris(temp,size,&width,&height);

        if(*xPos+width > 9) x_pos = 9-width;
        else                x_pos = *xPos/2;

        for(y=height; y>=0; y--) {
            if(*yPos+y-height < 0) break;

            for(x=0; x<=width; x++) {
                if(temp[y][x] == '1' && screen[*yPos+y-height][x_pos+x] > '3') {
                    *rotate_limit = 1;
                }
            }

            if(*rotate_limit == 1) break;
        }
    } else {
        *rotate_limit = 1;
    }
 
    for(y=3; y>=0; y--) {
        if(block_cnt < 4) y_pos++;
        if(*yPos-y_pos < 0) break; 
        block_cnt = 0;
        x_pos = 0;

        for(x=0; x<4; x++) {
            switch(src[y][x]) {
                case '0': 
                    block_cnt++;
                    break;

                case '1':	
                    if(*left_limit == 0) {
                        if((*xPos)+x_pos-1 >= 0) {
                            if(screen[*yPos-y_pos][(*xPos)+x_pos-1] > '3') {
                                *left_limit = 1;	
                            }
                        } else {
                            *left_limit = 1;
                        }
                    }
                    if(*right_limit == 0){
                        if((*xPos)+x_pos+1 <= 9) {
                            if(screen[*yPos-y_pos][(*xPos)+x_pos+1] > '3') {
                                *right_limit = 1;
                            }
                        } else {
                            *right_limit = 1;
                        }
                    }
                    if(*yPos+1 < 20) {
                        if(screen[*yPos-y_pos+1][(*xPos)+x_pos] > '3') {
                            y_pos_limit = 1;
                            default_y_limit = 1;
                        }
                        if(screen[*yPos-y_pos+1][(*xPos)+x_pos] == '2' && default_y_limit == 0) {
                            blank_y_limit = 1;
                        }
                    } else {
                        y_pos_limit = 1;
                    }
                    x_pos++;
                    break;

                default:	
                    x_pos++;
                    break;
            }
        }
    }

    if(blank_y_limit == 1 && default_y_limit == 0) {
        *y_limit = 0;
    }
    if(y_pos_limit == 1) {
        (*y_limit)++;
    }

    if(*y_limit < 2) *tetris_end = 1;
    else             *tetris_end = 0;
}
