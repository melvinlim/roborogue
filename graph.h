#ifndef _GRAPH_H
#define _GRAPH_H

#include<list.h>
#include<definitions.h>

#define GRAPH struct graph

struct graph{
	LIST *vList[ROWS*COLUMNS];
	VERTEX *vertex[ROWS*COLUMNS];
};

GRAPH *createGraph();

#endif
