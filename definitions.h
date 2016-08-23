#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

#define BUFSZ 4096

#define COLUMNS 80
#define ROWS 25
#define INDEX(n,m) ((n*COLUMNS)+m)
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

#define OBJECTS struct objects
OBJECTS{
	POINT *self;
	POINT *enemy;
	POINT *item;
	POINT *door;
};

#endif
