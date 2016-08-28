#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

//#define SAVE

#define DEBUG
#define DECODE_DEBUG

#define BUFSZ 4096

#define NORTH 'A'
#define SOUTH 'B'
#define EAST 'C'
#define WEST 'D'

#define COLS 80
//#define ROWS 25
#define ROWS 24
#define INDEX(n,m) (((n)*COLS)+(m))
#define NEW(x) malloc(sizeof(x))

#define VERTEX struct vertex
VERTEX{
	int val;
	VERTEX *pre;	//predecessor
};

#define LIST struct list
LIST{
	LIST *next;
	VERTEX *v;
};

#define POINT struct point

#define GRAPH struct graph
struct graph{
	LIST *vList[ROWS*COLS];
	VERTEX *vertex[ROWS*COLS];
};

struct point{
	int x;
	int y;
};

#define STATE enum State
STATE{
	idle,
	atDoor,
	atStairs,
	inTunnel,
	starving,
	attacking,
	searching,
	searchingForFood,
	exitedTunnel,
	returningToPrevLoc,
	movingToStairs
};

#define HUNGRY 2
#define STARVING 4
#define WEAK 8

#define OBJECTS struct objects
OBJECTS{
	int fdin;
	int fdout;
	char *map;
	char *inventory;
	int offset;
	int savedPosition;
	
	int prevStep;
	POINT *prevLoc;

	STATE state;
	STATE prevState;

	POINT *self;
	POINT *enemy;
	POINT *item;
	POINT *door;
	POINT *stairs;
	POINT *nextStep;
	LIST *visitedDoors;
	LIST *visitedTunnels;
	LIST *deadEnds;
	LIST *visitedItems;

	char prevBuffer[BUFSZ];
	GRAPH *graph;

	int scrollTop;
	int scrollBottom;
	int maxSearches;

	int status;
	int searches;
};

#endif
