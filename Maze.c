#include <stdlib.h>
#include <time.h>

#include "stdarg.h"
#include "option.h"

#include "2450addr.h"
#include "My_lib.h"

#include "melody_note.h"
#include "images.h"

//#define DEBUG_MAZE

typedef struct
{
	int x;
	int y;
} STRUCT_FRONTIER;

typedef struct
{
	int start_x;
	int start_y;
	int dest_x;
	int dest_y;
} STRUCT_POSITION;

#define NORTH 0x01
#define SOUTH 0x02
#define EAST 0x04
#define WEST 0x08

#define MAZE_START_X 110
#define MAZE_START_Y 60

#define MAZE_BLOCK_WIDTH	30
#define MAZE_BLOCK_HEIGHT	28

#define MAZE_CHA_SPACE_WIDTH	4
#define MAZE_CHA_SPACE_HEIGHT	3

#define MAZE_CHA_WIDTH	24
#define MAZE_CHA_HEIGHT	24

extern unsigned char game_play;
//common
int common_rand();

static unsigned long int next = 1;
unsigned int dogst = 0;

void common_rand_seed(void)
{
	unsigned long int tick_cnt;

	tick_cnt = (*(volatile unsigned long int *)0x57000090);

	next = next + tick_cnt;
}

int common_rand()
{
	common_rand_seed();
	next = next * 1103515245 + 12345;
	return (unsigned int) (next/65536) % 32768;	
}

void Tick_Count_Start(void)
{
	rRTCCON |= 1;	
	rRTCCON |= (1<<4);
	
	rTICNT |= (1<<7);
}

//maze init
void add_frontier(int x, int y, int** maze_board, STRUCT_FRONTIER* frontier);
void delete_frontier(int len, STRUCT_FRONTIER *save_frontier);
void mark(int x, int y, int ** maze_board, STRUCT_FRONTIER* frontier);
void neighbors(int x, int y, int ** maze_board);
int direction(int fx, int fy, int tx, int ty);
int opposite_direction(int direction);

//display
void maze_init();
void display_maze_info(int **maze_board);

//character
void maze_find_one_direction(int **maze_board);
void maze_set_startposition(int x, int y);
void maze_set_destposition(int x, int y);

void display_destHome(void);
void display_character_start(void);

#ifdef DEBUG_MAZE
void printf_maze_direction(int **maze_board);
#endif


#define MAZE_WIDTH 7
#define MAZE_HEIGHT 7

#define IN 0x10;
#define FRONTIER 0x20;

int frontier_count;
int neighbor_count;

int w = MAZE_WIDTH;
int h = MAZE_HEIGHT;

STRUCT_FRONTIER stFrontier[MAZE_WIDTH*MAZE_HEIGHT];
STRUCT_FRONTIER stNeighbors[4];

STRUCT_POSITION stCharacter_Position;	

int** maze_board;

void Maze_Debug_Printf(const char * fmt,...)
{
#ifdef DEBUG_MAZE
	va_list ap;
    char string[256];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    Uart_Send_String(string);
    va_end(ap);
#endif
}

//maze init
void add_frontier(int x, int y, int ** maze_board, STRUCT_FRONTIER *frontier)
{
	if ((x >= 0) && (y >= 0) && (x < w) && (y < h) && (maze_board[y][x] == 0))
	{
		maze_board[y][x] |= FRONTIER;
		frontier[frontier_count].x = x;
		frontier[frontier_count].y = y;
		frontier_count++;
	}
}

void delete_frontier(int len, STRUCT_FRONTIER* save_frontier)
{
	int i;

	save_frontier->x = stFrontier[len].x;
	save_frontier->y = stFrontier[len].y;

	for (i=len;i<frontier_count-1;i++)
	{
		stFrontier[i] = stFrontier[i+1];
	}

	frontier_count--;
}

void mark(int x, int y, int ** maze_board, STRUCT_FRONTIER* frontier)
{
	maze_board[y][x] |= IN;

	add_frontier(x-1,y,maze_board,frontier);
	add_frontier(x+1,y,maze_board,frontier);
	add_frontier(x,y-1,maze_board,frontier);
	add_frontier(x,y+1,maze_board,frontier);
}

void neighbors(int x, int y, int ** maze_board)
{
	int maze_value;
	int compare_value;
	
	neighbor_count = 0;

	if (x > 0)
	{
		maze_value = maze_board[y][x-1];
		compare_value = maze_value & IN;
	}

	if ((x > 0) && (compare_value != 0))
	{
		stNeighbors[neighbor_count].x = x-1;
		stNeighbors[neighbor_count].y = y;

		neighbor_count++;
	}

	if (x+1 < w)
	{
		maze_value = maze_board[y][x+1];
		compare_value = maze_value & IN;
	}

	if ((x+1 < w) && (compare_value != 0))
	{
		stNeighbors[neighbor_count].x = x+1;
		stNeighbors[neighbor_count].y = y;

		neighbor_count++;
	}

	if (y > 0)
	{
		maze_value = maze_board[y-1][x];
		compare_value = maze_value & IN;
	}

	if ((y > 0) && (compare_value != 0))
	{
		stNeighbors[neighbor_count].x = x;
		stNeighbors[neighbor_count].y = y-1;

		neighbor_count++;
	}

	if (y+1 < w)
	{
		maze_value = maze_board[y+1][x];
		compare_value = maze_value & IN;
	}

	if ((y+1 < h) && (compare_value != 0))
	{
		stNeighbors[neighbor_count].x = x;
		stNeighbors[neighbor_count].y = y+1;

		neighbor_count++;
	}
}

int direction(int fx, int fy, int tx, int ty)
{
	if (fx < tx)
	{
		return EAST;
	}

	if (fx > tx)
	{
		return WEST;
	}

	if (fy < ty)
	{
		return SOUTH;
	}

	if (fy > ty)
	{
		return NORTH;
	}
}

int opposite_direction(int direction)
{
	if (direction == EAST)
	{
		return WEST;
	}

	if (direction == WEST)
	{
		return EAST;
	}

	if (direction == NORTH)
	{
		return SOUTH;
	}

	if (direction == SOUTH)
	{
		return NORTH;
	}	
}

void make_maze() {
    int x, y, len;
	int nx, ny;
	int dir;
	int dir_opposite;
	
	STRUCT_FRONTIER temp_frontier;

#if 0	
	x = rand()%5;
	y = rand()%5;
#else
	x = common_rand()%MAZE_WIDTH;
	y = common_rand()%MAZE_WIDTH;
#endif

	maze_set_startposition(x,y);
	
    maze_init();
	
	mark(x,y,maze_board, &stFrontier[0]);

	while(frontier_count != 0)
	{
#if 0	
		len = rand() % frontier_count;
#else
		len = common_rand() % frontier_count;
#endif
		delete_frontier(len,&temp_frontier);
		x = temp_frontier.x;
		y = temp_frontier.y;

		maze_set_destposition(x,y);
		neighbors(x,y,maze_board);

#if 0
		len = rand() % neighbor_count;
#else
		len = common_rand() % neighbor_count;
#endif
		
		nx = stNeighbors[len].x;
		ny = stNeighbors[len].y;

		dir = direction(x,y,nx,ny);
		maze_board[y][x] |= dir;	

		dir_opposite = opposite_direction(dir);
		maze_board[ny][nx] |= dir_opposite;

		mark(x,y,maze_board, &stFrontier[0]);

		//Maze_Debug_Printf("[%d][%d]=0x%x [%d][%d]=0x%x\n",x,y,dir,nx,ny,dir_opposite);
	}

	maze_find_one_direction(maze_board);
#ifdef DEBUG_MAZE
	//printf_maze_direction(maze_board);
#endif
}

void free_maze(void)
{
	int i;

    for(i = 0 ; i<h ; i++) {
		free(maze_board[i]);        
    }
    free(maze_board);
}

void maze_init() {
	int i;
	int j;
	
#if 0	
    srand(time(NULL));
#else
	common_rand_seed();
#endif
	
    maze_board = (int**) malloc (sizeof(int*) * h);
    for(i = 0 ; i<h ; i++)
        maze_board[i] = (char*) malloc (sizeof(int) * w);

    for(i = 0 ; i<h ; i++) {
        for(j = 0 ; j<w ; j++) {
            maze_board[i][j] = 0x00;
        }
    }
}

#ifdef DEBUG_MAZE
void printf_maze_direction(int **maze_board)
{
	int i,j;
	
	for(i=0;i<MAZE_HEIGHT;i++)
	{
		for(j=0;j<MAZE_WIDTH;j++)
		{
			Maze_Debug_Printf("[%d][%d] = 0x%x\n",i,j,(maze_board[i][j] & 0xF));
		}
	}
}
#endif

//display
void display_maze(void)
{	
	Lcd_Select_Frame_Buffer(1);	

	Lcd_Clr_Area(MAZE_START_X,20,MAZE_START_X+(MAZE_WIDTH * MAZE_BLOCK_WIDTH),MAZE_START_Y+(MAZE_HEIGHT* MAZE_BLOCK_HEIGHT),0x0000);
	Lcd_Clr_Area(MAZE_START_X+(MAZE_WIDTH * MAZE_BLOCK_WIDTH),20,MAZE_START_X+(MAZE_WIDTH * MAZE_BLOCK_WIDTH)+10,170,0x0000);
	
	Lcd_Draw_BMP(220,20, timer);
	display_maze_info(maze_board);


	Lcd_Printf(270,30,0xFFFF,0x0000,1,1,"00:00");
	
	Lcd_Display_Frame_Buffer(1);	
}

void display_maze_info(int **maze_board)
{
	int i,j;
	int maze_value;	
	
	for(i=0;i<MAZE_HEIGHT;i++)
	{
		for(j=0;j<MAZE_WIDTH;j++)
		{
			maze_value = maze_board[i][j];
			
			if ((maze_value & WEST) == 0)
			{
				Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * j),MAZE_START_Y+(MAZE_BLOCK_HEIGHT * i),wall2);
			}

			if ((maze_value & EAST) == 0)
			{
				Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * j)+MAZE_BLOCK_WIDTH,MAZE_START_Y+(MAZE_BLOCK_HEIGHT * i),wall2);
			}

			if ((maze_value & NORTH) == 0)
			{
				Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * j),MAZE_START_Y+(MAZE_BLOCK_HEIGHT * i),wall);
			}

			if ((maze_value & SOUTH) == 0)
			{
				Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * j),MAZE_START_Y+(MAZE_BLOCK_HEIGHT * i)+MAZE_BLOCK_HEIGHT,wall);
			}
		}
	}
	display_destHome();
	display_character_start();
}

//characeter
void maze_find_one_direction(int **maze_board)
{
	int i,j;
	int dest_x;
	int dest_y;
	int dir;
	int maze_value;
	int one_cnt;

	int x_diff;
	int y_diff;
	int total_diff;
	int temp_diff;
	
	STRUCT_FRONTIER temp_position[MAZE_WIDTH*MAZE_HEIGHT];

	dest_x = stCharacter_Position.dest_x;
	dest_y = stCharacter_Position.dest_y;

	one_cnt = 0;
	total_diff = 0;
	temp_diff = 0;
	
	for(i=0;i<MAZE_HEIGHT;i++)
	{
		for(j=0;j<MAZE_WIDTH;j++)
		{
			dir = 0;
			
			maze_value = maze_board[i][j];

			if ((maze_value & WEST) == WEST)
			{
				dir++;
			}

			if ((maze_value & EAST) == EAST)
			{
				dir++;
			}

			if ((maze_value & NORTH) == NORTH)
			{
				dir++;
			}

			if ((maze_value & SOUTH) == SOUTH)
			{
				dir++;
			}

			if (dir == 1)
			{
				temp_position[one_cnt].x = j;
				temp_position[one_cnt].y = i;

				one_cnt++;
			}
		}
	}

	for(i=0;i<one_cnt;i++)
	{
		if (temp_position[i].x > stCharacter_Position.dest_x)
		{
			x_diff = temp_position[i].x - stCharacter_Position.dest_x;
		}
		else
		{
			x_diff = stCharacter_Position.dest_x - temp_position[i].x;
		}

		if (temp_position[i].y > stCharacter_Position.dest_y)
		{
			y_diff = temp_position[i].y - stCharacter_Position.dest_y;
		}
		else
		{
			y_diff = stCharacter_Position.dest_y - temp_position[i].y;
		}

		temp_diff = x_diff + y_diff;

		if (temp_diff > total_diff)
		{
			total_diff = temp_diff;
			maze_set_startposition(temp_position[i].x,temp_position[i].y);
		}
	}
}

void maze_set_startposition(int x, int y)
{
	stCharacter_Position.start_x = x;
	stCharacter_Position.start_y = y;
}

void maze_set_destposition(int x, int y)
{
	stCharacter_Position.dest_x = x;
	stCharacter_Position.dest_y = y;
}

void display_destHome(void)
{
	Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * stCharacter_Position.dest_x)+MAZE_CHA_SPACE_WIDTH,
	MAZE_START_Y+(MAZE_BLOCK_HEIGHT * stCharacter_Position.dest_y)+MAZE_CHA_SPACE_HEIGHT,HOME);
}

void display_character_start(void)
{	
	Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * stCharacter_Position.start_x)+MAZE_CHA_SPACE_WIDTH,
	MAZE_START_Y+(MAZE_BLOCK_HEIGHT * stCharacter_Position.start_y)+MAZE_CHA_SPACE_HEIGHT,LTDOG);
}

void maze_character_move_up(void)
{
	int current_x;
	int current_y;

	int move_x;
	int move_y;
	
	int maze_value;

	int move;

	move = 0;
	
	current_x = stCharacter_Position.start_x;
	current_y = stCharacter_Position.start_y;

	maze_value = maze_board[current_y][current_x];

//move check	
	if ((maze_value & NORTH) == NORTH)
	{
		move = 1;
		
		move_x = current_x;
		move_y = current_y -1;
	}

//move disp
	if (move == 1)
	{
		stCharacter_Position.start_x = move_x;
		stCharacter_Position.start_y = move_y;

		if ((stCharacter_Position.start_x == stCharacter_Position.dest_x)
			&& (stCharacter_Position.start_y == stCharacter_Position.dest_y))
		{
			game_play = 3; //clear

			Maze_Debug_Printf("Clear!!\n");
		}

		Lcd_Select_Frame_Buffer(1);
		
		Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * current_x)+MAZE_CHA_SPACE_WIDTH,
			MAZE_START_Y+(MAZE_BLOCK_HEIGHT * current_y)+MAZE_CHA_SPACE_HEIGHT,footup);
			
		if (game_play == 3)
		{
			Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * move_x)+MAZE_CHA_SPACE_WIDTH,
			MAZE_START_Y+(MAZE_BLOCK_HEIGHT * move_y)+MAZE_CHA_SPACE_HEIGHT,clear);
			dogst = 0;
		}
		else
		{			
			if(dogst){
				Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * move_x)+MAZE_CHA_SPACE_WIDTH,
				MAZE_START_Y+(MAZE_BLOCK_HEIGHT * move_y)+MAZE_CHA_SPACE_HEIGHT,RTDOG);
			}else{
				Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * move_x)+MAZE_CHA_SPACE_WIDTH,
				MAZE_START_Y+(MAZE_BLOCK_HEIGHT * move_y)+MAZE_CHA_SPACE_HEIGHT,LTDOG);
			}
		}
			
		Lcd_Display_Frame_Buffer(1);		
	}
}

void maze_character_move_down(void)
{
	int current_x;
	int current_y;

	int move_x;
	int move_y;
	
	int maze_value;

	int move;

	move = 0;
	
	current_x = stCharacter_Position.start_x;
	current_y = stCharacter_Position.start_y;

	maze_value = maze_board[current_y][current_x];

//move check
	if ((maze_value & SOUTH) == SOUTH)
	{
		move = 1;
		
		move_x = current_x;
		move_y = current_y + 1;
	}

//move disp
	if (move == 1)
	{
		stCharacter_Position.start_x = move_x;
		stCharacter_Position.start_y = move_y;

		if ((stCharacter_Position.start_x == stCharacter_Position.dest_x)
			&& (stCharacter_Position.start_y == stCharacter_Position.dest_y))
		{
			game_play = 3; //clear

			Maze_Debug_Printf("Clear!!\n");
		}
		
		Lcd_Select_Frame_Buffer(1);
			
		Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * current_x)+MAZE_CHA_SPACE_WIDTH,
			MAZE_START_Y+(MAZE_BLOCK_HEIGHT * current_y)+MAZE_CHA_SPACE_HEIGHT,footdo);
			
		if (game_play == 3)
		{
			Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * move_x)+MAZE_CHA_SPACE_WIDTH,
			MAZE_START_Y+(MAZE_BLOCK_HEIGHT * move_y)+MAZE_CHA_SPACE_HEIGHT,clear);
			dogst = 0;
		}
		else
		{
			if(dogst){
				Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * move_x)+MAZE_CHA_SPACE_WIDTH,
				MAZE_START_Y+(MAZE_BLOCK_HEIGHT * move_y)+MAZE_CHA_SPACE_HEIGHT,RTDOG);
			}else{
				Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * move_x)+MAZE_CHA_SPACE_WIDTH,
				MAZE_START_Y+(MAZE_BLOCK_HEIGHT * move_y)+MAZE_CHA_SPACE_HEIGHT,LTDOG);
			}
		}			
			
		Lcd_Display_Frame_Buffer(1);
	}
}

void maze_character_move_left(void)
{
	int current_x;
	int current_y;

	int move_x;
	int move_y;
	
	int maze_value;

	int move;

	move = 0;
	
	current_x = stCharacter_Position.start_x;
	current_y = stCharacter_Position.start_y;

	maze_value = maze_board[current_y][current_x];

//move check	
	if ((maze_value & WEST) == WEST)
	{
		move = 1;
		
		move_x = current_x - 1;
		move_y = current_y;
	}

//move disp
	if (move == 1)
	{
		stCharacter_Position.start_x = move_x;
		stCharacter_Position.start_y = move_y;

		if ((stCharacter_Position.start_x == stCharacter_Position.dest_x)
			&& (stCharacter_Position.start_y == stCharacter_Position.dest_y))
		{
			game_play = 3; //clear
			
			Maze_Debug_Printf("Clear!!\n");
		}

		Lcd_Select_Frame_Buffer(1);
					
		Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * current_x)+MAZE_CHA_SPACE_WIDTH,
			MAZE_START_Y+(MAZE_BLOCK_HEIGHT * current_y)+MAZE_CHA_SPACE_HEIGHT,footlt);
			
		if (game_play == 3)
		{
			Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * move_x)+MAZE_CHA_SPACE_WIDTH,
			MAZE_START_Y+(MAZE_BLOCK_HEIGHT * move_y)+MAZE_CHA_SPACE_HEIGHT,clear);
			dogst = 0;
		}
		else
		{
			Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * move_x)+MAZE_CHA_SPACE_WIDTH,
			MAZE_START_Y+(MAZE_BLOCK_HEIGHT * move_y)+MAZE_CHA_SPACE_HEIGHT,LTDOG);
			dogst = 0;
		}
		
		Lcd_Display_Frame_Buffer(1);
	}
}

void maze_character_move_right(void)
{
	int current_x;
	int current_y;

	int move_x;
	int move_y;
	
	int maze_value;

	int move;

	move = 0;
	
	current_x = stCharacter_Position.start_x;
	current_y = stCharacter_Position.start_y;

	maze_value = maze_board[current_y][current_x];

//move check	
	if ((maze_value & EAST) == EAST)
	{
		move = 1;
		
		move_x = current_x + 1;
		move_y = current_y;
	}

//move disp
	if (move == 1)
	{
		stCharacter_Position.start_x = move_x;
		stCharacter_Position.start_y = move_y;

		if ((stCharacter_Position.start_x == stCharacter_Position.dest_x)
			&& (stCharacter_Position.start_y == stCharacter_Position.dest_y))
		{
			game_play = 3; //clear
			
			Maze_Debug_Printf("Clear!!\n");
		}
		
		Lcd_Select_Frame_Buffer(1);
			
		Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * current_x)+MAZE_CHA_SPACE_WIDTH,
			MAZE_START_Y+(MAZE_BLOCK_HEIGHT * current_y)+MAZE_CHA_SPACE_HEIGHT,footrt);
			
		if (game_play == 3)
		{
			Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * move_x)+MAZE_CHA_SPACE_WIDTH,
			MAZE_START_Y+(MAZE_BLOCK_HEIGHT * move_y)+MAZE_CHA_SPACE_HEIGHT,clear);
			dogst = 0;
		}
		else
		{
			Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * move_x)+MAZE_CHA_SPACE_WIDTH,
			MAZE_START_Y+(MAZE_BLOCK_HEIGHT * move_y)+MAZE_CHA_SPACE_HEIGHT,RTDOG);
			dogst = 1;
		}
		
		Lcd_Display_Frame_Buffer(1);
	}
}

void maze_buzz_clear(void)
{
	Beep(NOTE_C5,250);
	Beep(NOTE_G4,250);
	Beep(NOTE_E4,250);
	Beep(NOTE_A4,250);
	Beep(NOTE_B4,250);
	Beep(NOTE_AS4,250);
	Beep(NOTE_A4,250);
}
