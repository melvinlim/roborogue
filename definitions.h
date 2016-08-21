#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

#define COLUMNS 80
#define ROWS 24
#define INDEX(n,m) ((n*COLUMNS)+m)
#define NEW(x) malloc(sizeof(x))

#define VERTEX struct vertex
VERTEX{
	int val;
	int visited;
};

#endif
