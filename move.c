#include<stdio.h>
#include<string.h>
#include<move.h>
#include<definitions.h>

void space(int fdin){
	char buf;
	int n;
	buf=' ';
	n=write(fdin,&buf,1);
	printf("wrote %d bytes\n",n);
}
void move(int fdin,char *map,char dir){
	char buf[3];
	int n;
	buf[0]=27;	//ESC
	buf[1]='[';
	buf[2]=dir;
	n=write(fdin,buf,3);
#ifdef DEBUG
	switch(dir){
		case UP:
			printf("moving UP\n");
		break;
		case DOWN:
			printf("moving DOWN\n");
		break;
		case LEFT:
			printf("moving LEFT\n");
		break;
		case RIGHT:
			printf("moving RIGHT\n");
		break;
	}
	printf("wrote %d bytes\n",n);
#endif
}
void quit(int fdin){
	int n;
	char buf[5];
	strcpy(buf,"Qy\n  ");
	n=write(fdin,buf,5);
	printf("wrote %d bytes\n",n);
}
int moveTowardsX(int fdin,char *map,POINT *dst,POINT *src){
	if(src->x > dst->x){
		if(!validTile(map[INDEX(src->y,src->x-1)]))	return 0;
		move(fdin,map,LEFT);
		return 1;
	}else if(src->x < dst->x){
		if(!validTile(map[INDEX(src->y,src->x+1)]))	return 0;
		move(fdin,map,RIGHT);
		return 1;
	}
	return 0;
}
int moveTowardsY(int fdin,char *map,POINT *dst,POINT *src){
	if(src->y > dst->y){
		if(!validTile(map[INDEX(src->y-1,src->x)]))	return 0;
		move(fdin,map,UP);
		return 1;
	}else if(src->y < dst->y){
		if(!validTile(map[INDEX(src->y+1,src->x)]))	return 0;
		move(fdin,map,DOWN);
		return 1;
	}
	return 0;
}
//void moveTowards(int fdin,char *map,POINT *dst,POINT *src){
void moveTowards(int fdin,OBJECTS *objs,POINT *dst){
	int dx,dy;
	char *map=objs->map;
	POINT *src=objs->self;
	if((src==0)||(dst==0))	return;
	dx = abs(src->x - dst->x);
	dy = abs(src->y - dst->y);
//	if(random()%2==1){
	if(dx>=dy){
		if(moveTowardsX(fdin,map,dst,src)){
			return;
		}else{
			moveTowardsY(fdin,map,dst,src);
		}
	}else{
		if(moveTowardsY(fdin,map,dst,src)){
			return;
		}else{
			moveTowardsX(fdin,map,dst,src);
		}
	}
}
