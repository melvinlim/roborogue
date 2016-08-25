#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<move.h>
#include<definitions.h>

void markDoor(OBJECTS *o){
	VERTEX *v=NEW(VERTEX);
	v->val=INDEX(o->self->y,o->self->x);
	addList(o->visitedDoors,v);
	printf("visitedDoors:\n");
	printList(o->visitedDoors);
}
void search(int fdin){
	char buf;
	int n;
	buf='s';
	n=write(fdin,&buf,1);
	printf("searching.  wrote %d bytes\n",n);
}
void consume(OBJECTS *objs,char slot){
	char buf;
	int fdin=objs->fdin;
	int n;
	buf='e';
	n=write(fdin,&buf,1);
	buf=slot;
	n=write(fdin,&buf,1);
	printf("consuming item in slot %c.  wrote %d bytes\n",slot,n);
}
void save(int fdin,char *name){
	char buf;
	int n;
	buf='S';
	n=write(fdin,&buf,1);
	printf("wrote %d bytes\n",n);
	n=write(fdin,name,strlen(name));
	printf("wrote %d bytes\n",n);
}
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
			printf("exiting tunnel.  in move.c:navigateTunnel().  need to mark door as visited on next step.\n");
			objs->state=exitedTunnel;
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
			printf("exiting tunnel.  in move.c:navigateTunnel().  need to mark door as visited on next step.\n");
			objs->state=exitedTunnel;
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
			printf("exiting tunnel.  in move.c:navigateTunnel().  need to mark door as visited on next step.\n");
			objs->state=exitedTunnel;
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
			printf("exiting tunnel.  in move.c:navigateTunnel().  need to mark door as visited on next step.\n");
			objs->state=exitedTunnel;
			return 0;
		}
	}
	printf("at dead end.  in move.c:navigateTunnel().  changing to search state\n");
	objs->state=searching;
	return prev;
//	return -1;
}
int moveTowards(int fdin,OBJECTS *objs,POINT *dst){
	int dx,dy;
	int dir;
	char *map=objs->map;
	POINT *src=objs->self;
	if((src==0)||(dst==0))	return;
	dx = abs(src->x - dst->x);
	dy = abs(src->y - dst->y);
/*
	if(dx>=dy){
		dir=(moveTowardsX(fdin,map,dst,src));
		if(dir){
		}else{
			dir=moveTowardsY(fdin,map,dst,src);
		}
	}else{
		dir=(moveTowardsY(fdin,map,dst,src));
		if(dir){
		}else{
			dir=moveTowardsX(fdin,map,dst,src);
		}
	}
*/
	if((objs->nextStep==0)){
		printf("error in move.c:moveTowards(), objs->nextStep==0\n");
		return;
	}
	dir=(moveTowardsX(fdin,map,objs->nextStep,src));
	if(dir==0){
		dir=moveTowardsY(fdin,map,objs->nextStep,src);
	}
	
	if(	(dx==1)&&(dy==0)	||
			(dx==0)&&(dy==1)	){
		return dir;
	}
	return 0;
}
