#ifndef _MOVE_H
#define _MOVE_H

#include<definitions.h>

#define UP 'A'
#define DOWN 'B'
#define RIGHT 'C'
#define LEFT 'D'

void space(int fdin);
void move(int fdin,char dir);
void quit(int fdin);
void moveTowards(int fdin,POINT *dst,POINT *src);

#endif
