#include <stdlib.h>
#include <time.h>
#include "My_lib.h"

typedef struct
{
	int x;
	int y;
} STRUCT_FRONTIER;

#define NORTH 0x01
#define SOUTH 0x02
#define EAST 0x04
#define WEST 0x08

void add_frontier(int x, int y, char** maze_board, STRUCT_FRONTIER* frontier);
void delete_frontier(int len, STRUCT_FRONTIER *save_frontier);
void mark(int x, int y, char ** maze_board, STRUCT_FRONTIER* frontier);
void neighbors(int x, int y, char ** maze_board);
int direction(int fx, int fy, int tx, int ty);
int opposite_direction(int direction);
void maze_init();

#define MAZE_WIDTH 7
#define MAZE_HEIGHT 7

#define IN 0x10;
#define FRONTIER 0x20;

int frontier_count;
int neighbor_count;

int w = MAZE_WIDTH;
int h = MAZE_HEIGHT;


STRUCT_FRONTIER stFrontier[49];
STRUCT_FRONTIER stNeighbors[4];

int** maze_board;

void add_frontier(int x, int y, char ** maze_board, STRUCT_FRONTIER *frontier)
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


void mark(int x, int y, char ** maze_board, STRUCT_FRONTIER* frontier)
{
	maze_board[y][x] |= IN;

	Uart_Printf("Mark Start\n");
	add_frontier(x-1,y,maze_board,frontier);
	add_frontier(x+1,y,maze_board,frontier);
	add_frontier(x,y-1,maze_board,frontier);
	add_frontier(x,y+1,maze_board,frontier);
	Uart_Printf("Mark End\n");
}

void neighbors(int x, int y, char ** maze_board)
{
	int maze_value;
	int compare_value;
	
	neighbor_count = 0;

	if (x > 0)
	{
		maze_value = maze_board[y][x-1];
		compare_value = maze_value & IN;
	}

	Uart_Printf("neighbors 1st %d %d %d %d\n",y,x,maze_value,neighbor_count);
#if 1	
	if ((x > 0) && (compare_value != 0))
	{
		stNeighbors[neighbor_count].x = x-1;
		stNeighbors[neighbor_count].y = y;

		neighbor_count++;
	}
#endif	

	if (x+1 < w)
	{
		maze_value = maze_board[y][x+1];
		compare_value = maze_value & IN;
	}

	Uart_Printf("neighbors 2nd %d %d %d %d %d\n",y,x,maze_value,neighbor_count, compare_value);
#if 1	
	if ((x+1 < w) && (compare_value != 0))
	{
		stNeighbors[neighbor_count].x = x+1;
		stNeighbors[neighbor_count].y = y;

		neighbor_count++;
	}
#endif	

	if (y > 0)
	{
		maze_value = maze_board[y-1][x];
		compare_value = maze_value & IN;
	}

	Uart_Printf("neighbors 3rd %d %d %d %d\n",y,x,maze_value,neighbor_count);
#if 1	
	if ((y > 0) && (compare_value != 0))
	{
		stNeighbors[neighbor_count].x = x;
		stNeighbors[neighbor_count].y = y-1;

		neighbor_count++;
	}
#endif	

	if (y+1 < w)
	{
		maze_value = maze_board[y+1][x];
		compare_value = maze_value & IN;
	}

	Uart_Printf("neighbors 4th %d %d %d %d\n",y,x,maze_value,neighbor_count);
#if 1	
	if ((y+1 < h) && (compare_value != 0))
	{
		stNeighbors[neighbor_count].x = x;
		stNeighbors[neighbor_count].y = y+1;

		neighbor_count++;
	}
#endif	
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
	if (direction == SOUTH)
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
	int cnt = 0;
	
	STRUCT_FRONTIER temp_frontier;
	
	x = rand()%5;
	y = rand()%5;
	
    maze_init();

	Uart_Printf("maze_init OK!!\n");
	
	mark(x,y,maze_board, &stFrontier[0]);

	while(frontier_count != 0)
	{
		len = rand() % frontier_count;
		delete_frontier(len,&temp_frontier);
		x = temp_frontier.x;
		y = temp_frontier.y;
		
		Uart_Printf("delete_frontier OK!!\n");
		
		neighbors(x,y,maze_board);

		len = rand() % neighbor_count;
		
		nx = stNeighbors[len].x;
		ny = stNeighbors[len].y;

		Uart_Printf("neighbors OK!!\n");
		
		dir = direction(x,y,nx,ny);
		maze_board[y][x] |= dir;

		dir_opposite = opposite_direction(dir);
		maze_board[ny][nx] |= dir_opposite;

		mark(x,y,maze_board, &stFrontier[0]);
		cnt++;
	}
	Uart_Printf("maze check cnt %d\n",cnt);
}

void maze_init() {
	int i;
	int j;
	
    srand(time(NULL));
	
    maze_board = (int**) malloc (sizeof(int*) * h);
    for(i = 0 ; i<h ; i++)
        maze_board[i] = (char*) malloc (sizeof(int) * w);

    for(i = 0 ; i<h ; i++) {
        for(j = 0 ; j<w ; j++) {
            maze_board[i][j] = 0;
        }
    }
}
