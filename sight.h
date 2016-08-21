#ifndef _SIGHT_H
#define _SIGHT_H

#define POINT struct point

struct point{
	int x;
	int y;
};

POINT *findSelf(char *map);
void print(POINT *pt);

#endif
