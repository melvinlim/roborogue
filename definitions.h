#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

#define DEBUG
#define DECODE_DEBUG

#define BUFSZ 4096

#define UP 'A'
#define DOWN 'B'
#define RIGHT 'C'
#define LEFT 'D'

#define COLUMNS 80
#define ROWS 25
#define INDEX(n,m) (((n)*COLUMNS)+(m))
#define NEW(x) malloc(sizeof(x))

#define VERTEX struct vertex
VERTEX{
	int val;
};

#define LIST struct list
LIST{
	LIST *next;
	VERTEX *v;
};

#define POINT struct point

#define GRAPH struct graph
struct graph{
	LIST *vList[ROWS*COLUMNS];
	VERTEX *vertex[ROWS*COLUMNS];
};

struct point{
	int x;
	int y;
};

#define STATE enum State
STATE{
	idle,
	atDoor,
	inTunnel,
	exitedTunnel
};

#define OBJECTS struct objects
OBJECTS{
	int fd;
	char *map;
	int offset;
	STATE state;
	POINT *self;
	POINT *enemy;
	POINT *item;
	POINT *door;
	POINT *stairs;
	LIST *visitedDoors;
};

#endif
