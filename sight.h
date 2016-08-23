#ifndef _SIGHT_H
#define _SIGHT_H

#include<definitions.h>

POINT *findSelf(char *map);
void print(POINT *pt);
POINT *nearestObject(char *map,GRAPH *g,POINT *loc);
POINT *nearestItem(char *map,GRAPH *g,POINT *loc);
POINT *nearestEnemy(char *map,GRAPH *g,POINT *loc);
POINT *nearestDoor(char *map,GRAPH *g,POINT *loc);
char *lastMessage(char *map);
int checkMore(char *map);

#endif
