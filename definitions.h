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

#endif
