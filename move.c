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
		return LEFT;
	}else if(src->x < dst->x){
		if(!validTile(map[INDEX(src->y,src->x+1)]))	return 0;
		move(fdin,map,RIGHT);
		return RIGHT;
	}
	return 0;
}
int moveTowardsY(int fdin,char *map,POINT *dst,POINT *src){
	if(src->y > dst->y){
		if(!validTile(map[INDEX(src->y-1,src->x)]))	return 0;
		move(fdin,map,UP);
		return UP;
	}else if(src->y < dst->y){
		if(!validTile(map[INDEX(src->y+1,src->x)]))	return 0;
		move(fdin,map,DOWN);
		return DOWN;
	}
	return 0;
}
int opposite(int dir){
	switch(dir){
		case'A':
			return'B';
		break;
		case'B':
			return'A';
		break;
		case'C':
			return'D';
		break;
		case'D':
			return'C';
		break;
	}
	return 0;
}
int navigateTunnel(int fdin,OBJECTS *objs,int prev){
	int opp=opposite(prev);
	if(opp!='A'){
		if(isTunnel(objs->map[INDEX(objs->self->y-1,objs->self->x)])){
			move(fdin,objs->map,'A');
			return 'A';
		}
		if(isDoor(objs->map[INDEX(objs->self->y-1,objs->self->x)])){
			move(fdin,objs->map,'A');
			return 0;
		}
	}
	if(opp!='B'){
		if(isTunnel(objs->map[INDEX(objs->self->y+1,objs->self->x)])){
			move(fdin,objs->map,'B');
			return 'B';
		}
		if(isDoor(objs->map[INDEX(objs->self->y+1,objs->self->x)])){
			move(fdin,objs->map,'B');
			return 0;
		}
	}
	if(opp!='C'){
		if(isTunnel(objs->map[INDEX(objs->self->y,objs->self->x+1)])){
			move(fdin,objs->map,'C');
			return 'C';
		}
		if(isDoor(objs->map[INDEX(objs->self->y,objs->self->x+1)])){
			move(fdin,objs->map,'C');
			return 0;
		}
	}
	if(opp!='D'){
		if(isTunnel(objs->map[INDEX(objs->self->y,objs->self->x-1)])){
			move(fdin,objs->map,'D');
			return 'D';
		}
		if(isDoor(objs->map[INDEX(objs->self->y,objs->self->x-1)])){
			move(fdin,objs->map,'D');
			return 0;
		}
	}
	return 0;
}
//void moveTowards(int fdin,char *map,POINT *dst,POINT *src){
int moveTowards(int fdin,OBJECTS *objs,POINT *dst){
	int dx,dy;
	int dir;
	char *map=objs->map;
	POINT *src=objs->self;
	if((src==0)||(dst==0))	return;
	dx = abs(src->x - dst->x);
	dy = abs(src->y - dst->y);
//	if(random()%2==1){
	if(dx>=dy){
//		if(moveTowardsX(fdin,map,dst,src)){
		dir=(moveTowardsX(fdin,map,dst,src));
		if(dir){
		}else{
			dir=moveTowardsY(fdin,map,dst,src);
		}
	}else{
//		if(moveTowardsY(fdin,map,dst,src)){
		dir=(moveTowardsY(fdin,map,dst,src));
		if(dir){
		}else{
			dir=moveTowardsX(fdin,map,dst,src);
		}
	}
	if(	(dx==1)&&(dy==0)	||
			(dx==0)&&(dy==1)	){
		return dir;
	}
	return 0;
}
