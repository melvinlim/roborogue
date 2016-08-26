#ifndef _SIGHT_H
#define _SIGHT_H

#include<definitions.h>

POINT *findSelf(char *map);
void print(POINT *pt);
/*
POINT *nearestObject(char *map,GRAPH *g,POINT *loc);
POINT *nearestItem(char *map,GRAPH *g,POINT *loc);
POINT *nearestEnemy(char *map,GRAPH *g,POINT *loc);
POINT *nearestDoor(char *map,GRAPH *g,POINT *loc);
*/
POINT *nearestPoint(OBJECTS *objs,GRAPH *g,POINT *target);
/*
POINT *nearestObject(OBJECTS *objs,GRAPH *g);
POINT *nearestItem(OBJECTS *objs,GRAPH *g);
POINT *nearestEnemy(OBJECTS *objs,GRAPH *g);
POINT *nearestDoor(OBJECTS *objs,GRAPH *g);
POINT *nearestStairs(OBJECTS *objs,GRAPH *g);
*/
POINT *nearestObject(OBJECTS *objs);
POINT *nearestItem(OBJECTS *objs);
POINT *nearestEnemy(OBJECTS *objs);
POINT *nearestDoor(OBJECTS *objs);
POINT *nearestStairs(OBJECTS *objs);
char *lastMessage(char *map);
int checkMore(char *map);

#endif
