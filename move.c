#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<move.h>
#include<definitions.h>

int restOneTurn(OBJECTS *o){
	char buf;
	int n;
	buf='.';
	n=write(o->fdin,&buf,1);
	printf("wrote %d bytes\n",n);
	return n;
}
int pickupItem(OBJECTS *o){
	char buf;
	int n;
	buf=',';
	n=write(o->fdin,&buf,1);
	printf("wrote %d bytes\n",n);
	return n;
}
void markDeadEnd(OBJECTS *o){
	addList(o->deadEnds,o->player);
	printf("DeadEnds:\n");
	printList(o->deadEnds);
}
LIST *markTunnel(OBJECTS *o){
	LIST *newNode;
	newNode=addList(o->visitedTunnels,o->player);
	printf("visitedTunnels:\n");
	printList(o->visitedTunnels);
	return newNode;
}
void markItem(OBJECTS *o){
	addList(o->visitedItems,o->player);
	printf("visitedItems:\n");
	printList(o->visitedItems);
}
void markDoor(OBJECTS *o){
	addList(o->visitedDoors,o->player);
	printf("visitedDoors:\n");
	printList(o->visitedDoors);
}
int descend(int fdin){
	char buf;
	int n;
	buf='>';
	n=write(fdin,&buf,1);
	printf("descending.  wrote %d bytes\n",n);
	return n;
}
void search(int fdin){
	char buf;
	int n;
	buf='s';
	n=write(fdin,&buf,1);
	printf("searching.  wrote %d bytes\n",n);
}
int tryQuaff(OBJECTS *objs){
	char buf;
	int fdin=objs->fdin;
	int n;
	buf='q';
	n=write(fdin,&buf,1);
	scanArea(objs);
	if(checkFound(objs->map)){
		buf='a';
		n=write(fdin,&buf,1);
		printf("wrote %d bytes\n",n);
		return 1;
	}
	return 0;
}
int tryRead(OBJECTS *objs){
	char buf;
	int fdin=objs->fdin;
	int n;
	buf='r';
	n=write(fdin,&buf,1);
	scanArea(objs);
	if(checkFound(objs->map)){
		buf='a';
		n=write(fdin,&buf,1);
		printf("wrote %d bytes\n",n);
		return 1;
	}
	return 0;
}
void tryToEat(OBJECTS *objs){
	char buf;
	int fdin=objs->fdin;
	int n;
	buf='e';
	n=write(fdin,&buf,1);
	buf='a';
	n=write(fdin,&buf,1);
//	printf("wrote %d bytes\n",slot,n);
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
void sendSpace(int fdin){
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
		case NORTH:
			printf("moving NORTH\n");
		break;
		case SOUTH:
			printf("moving SOUTH\n");
		break;
		case WEST:
			printf("moving WEST\n");
		break;
		case EAST:
			printf("moving EAST\n");
		break;
	}
	printf("wrote %d bytes\n",n);
#endif
}
void runAway(OBJECTS *objs){
	char *map=objs->map;
	int tmp;
	int maxDir=-1;
	int max=0;
	int loc;
	loc=objs->player->val+1;
	if(validTile(map[loc])){
		tmp=objs->graph->vList[loc]->next->v->eDist;
		if(tmp > max){
			max=tmp;
			maxDir=EAST;
		}
	}
	loc=objs->player->val-1;
	if(validTile(map[loc])){
		tmp=objs->graph->vList[loc]->next->v->eDist;
		if(tmp > max){
			max=tmp;
			maxDir=WEST;
		}
	}
	loc=objs->player->val+COLS;
	if(validTile(map[loc])){
		tmp=objs->graph->vList[loc]->next->v->eDist;
		if(tmp > max){
			max=tmp;
			maxDir=SOUTH;
		}
	}
	loc=objs->player->val-COLS;
	if(validTile(map[loc])){
		tmp=objs->graph->vList[loc]->next->v->eDist;
		if(tmp > max){
			max=tmp;
			maxDir=NORTH;
		}
	}
	if(maxDir>=0){
		move(objs->fdin,map,maxDir);
	}
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
		move(fdin,map,WEST);
		return WEST;
	}else if(src->x < dst->x){
		if(!validTile(map[INDEX(src->y,src->x+1)]))	return 0;
		move(fdin,map,EAST);
		return EAST;
	}
	return 0;
}
int moveTowardsY(int fdin,char *map,POINT *dst,POINT *src){
	if(src->y > dst->y){
		if(!validTile(map[INDEX(src->y-1,src->x)]))	return 0;
		move(fdin,map,NORTH);
		return NORTH;
	}else if(src->y < dst->y){
		if(!validTile(map[INDEX(src->y+1,src->x)]))	return 0;
		move(fdin,map,SOUTH);
		return SOUTH;
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
int navTunnel(int fdin,OBJECTS *objs){
	POINT *dst;
	if(!findListValue(objs->visitedDoors,INDEX(objs->self->y-1,objs->self->x)))
		if(isDoor(objs->map[INDEX(objs->self->y-1,objs->self->x)])){
			move(fdin,objs->map,'A');
			printf("exiting tunnel.  in move.c:navigateTunnel().  need to mark door as visited on next step.\n");
			objs->state=exitedTunnel;
			objs->state=idle;
			return 0;
		}
	if(!findListValue(objs->visitedDoors,INDEX(objs->self->y+1,objs->self->x)))
		if(isDoor(objs->map[INDEX(objs->self->y+1,objs->self->x)])){
			move(fdin,objs->map,'B');
			printf("exiting tunnel.  in move.c:navigateTunnel().  need to mark door as visited on next step.\n");
			objs->state=exitedTunnel;
			objs->state=idle;
			return 0;
		}
	if(!findListValue(objs->visitedDoors,INDEX(objs->self->y,objs->self->x+1)))
		if(isDoor(objs->map[INDEX(objs->self->y,objs->self->x+1)])){
			move(fdin,objs->map,'C');
			printf("exiting tunnel.  in move.c:navigateTunnel().  need to mark door as visited on next step.\n");
			objs->state=exitedTunnel;
			objs->state=idle;
			return 0;
		}
	if(!findListValue(objs->visitedDoors,INDEX(objs->self->y,objs->self->x-1)))
		if(isDoor(objs->map[INDEX(objs->self->y,objs->self->x-1)])){
			move(fdin,objs->map,'D');
			printf("exiting tunnel.  in move.c:navigateTunnel().  need to mark door as visited on next step.\n");
			objs->state=exitedTunnel;
			objs->state=idle;
			return 0;
		}
	dst=nearestTunnel(objs);
	if(dst==0){
		if((objs->door)||(objs->item)){
			objs->state=idle;
			return 0;
		}else if(objs->stairs){
			objs->state=movingToStairs;
			return 0;
		}
		printf("at dead end.  in move.c:navigateTunnel().  changing to search state\n");
		objs->state=searching;
		return 0;
	}
	moveTowards(objs,dst);
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
int moveToV(OBJECTS *objs,VERTEX *vert){
	int dx,dy;
	int dir;
	int fdin=objs->fdin;
	char *map=objs->map;
	VERTEX *p=findPredecessor(objs,vert);
	POINT *src=objs->self;
//	int dstx=vert->val%COLS;
//	int dsty=vert->val/COLS;
	if((src==0))	return;
	//dx = abs(src->x - dstx);
	//dy = abs(src->y - dsty);
	dx = abs(src->x - vert->x);
	dy = abs(src->y - vert->y);
	objs->nextStep=malloc(sizeof(POINT));
	objs->nextStep->x=p->x;
	objs->nextStep->y=p->y;
	dir=(moveTowardsX(fdin,map,objs->nextStep,src));
	if(dir==0){
		dir=moveTowardsY(fdin,map,objs->nextStep,src);
	}
	
	if(	(dx==1)&&(dy==0)	||
			(dx==0)&&(dy==1)	){
if(vert->sDist!=1)	while(1);
		return dir;
	}
	return 0;
}
int moveTowards(OBJECTS *objs,POINT *dst){
	int dx,dy;
	int dir;
	int fdin=objs->fdin;
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
		return 0;
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
