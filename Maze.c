#include <stdlib.h>
#include <time.h>

#include "stdarg.h"
#include "option.h"

#include "2450addr.h"
#include "My_lib.h"

#include "images.h"

#define DEBUG_MAZE

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

#define MAZE_START_X 80
#define MAZE_START_Y 70

#define MAZE_BLOCK_WIDTH	30
#define MAZE_BLOCK_HEIGHT	28

#define MAZE_CHA_SPACE_WIDTH	4
#define MAZE_CHA_SPACE_HEIGHT	3

//common
int common_rand();

static unsigned long int next = 1;
static unsigned long int tick;

void common_srand(void)
{
	next = (*(volatile unsigned long int *)0x57000090);
}

int common_rand()
{
	common_srand();
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
void maze_set_startposition(int x, int y);
void maze_set_destposition(int x, int y);

void display_destHome(void);
void display_character_start(void);

#ifdef DEBUG_MAZE
void printf_maze_direction(int **maze_board);
#endif


#define MAZE_WIDTH 6
#define MAZE_HEIGHT 6

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

		Maze_Debug_Printf("[%d][%d]=0x%x [%d][%d]=0x%x\n",x,y,dir,nx,ny,dir_opposite);
	}

#ifdef DEBUG_MAZE
	printf_maze_direction(maze_board);
#endif
}

void maze_init() {
	int i;
	int j;
	
#if 0	
    srand(time(NULL));
#else
	common_srand();
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
	display_maze_info(maze_board);
}

void display_maze_info(int **maze_board)
{
	int i,j;
	
	Lcd_Select_Frame_Buffer(0);

	for(i=0;i<MAZE_HEIGHT;i++)
	{
		for(j=0;j<MAZE_WIDTH;j++)
		{
			if ((maze_board[i][j] & WEST) == 0)
			{
				Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * j),MAZE_START_Y+(MAZE_BLOCK_HEIGHT * i),wall2);
			}

			if ((maze_board[i][j] & EAST) == 0)
			{
				Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * j)+MAZE_BLOCK_WIDTH,MAZE_START_Y+(MAZE_BLOCK_HEIGHT * i),wall2);
			}

			if ((maze_board[i][j] & NORTH) == 0)
			{
				Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * j),MAZE_START_Y+(MAZE_BLOCK_HEIGHT * i),wall);
			}

			if ((maze_board[i][j] & SOUTH) == 0)
			{
				Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * j),MAZE_START_Y+(MAZE_BLOCK_HEIGHT * i)+MAZE_BLOCK_HEIGHT,wall);
			}
		}
	}
	display_destHome();
	display_character_start();

	while(1)
	{		
		Uart_Printf("0x%X\n",(*(volatile unsigned long int *)0x57000090));
	}
}

//characeter
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
	Lcd_Select_Frame_Buffer(1);
	Lcd_Copy(0,1);
	
	Lcd_Draw_BMP(MAZE_START_X+(MAZE_BLOCK_WIDTH * stCharacter_Position.start_x)+MAZE_CHA_SPACE_WIDTH,
	MAZE_START_Y+(MAZE_BLOCK_HEIGHT * stCharacter_Position.start_y)+MAZE_CHA_SPACE_HEIGHT,LTDOG);

	Lcd_Display_Frame_Buffer(1);
}