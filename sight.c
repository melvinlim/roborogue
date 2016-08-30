#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sight.h>

#include<graph.h>

OBJECTS *createObjects(){
	OBJECTS *objs=NEW(OBJECTS);
	bzero(objs,sizeof(OBJECTS));
	objs->visitedDoors=createList();
	objs->visitedItems=createList();
	objs->visitedTunnels=createList();
	objs->deadEnds=createList();
	objs->seenDoors=createList();
	objs->seenItems=createList();
	objs->seenEnemies=createList();
	objs->seenStairs=createList();
	objs->seenTunnels=createList();
	return objs;
}

OBJECTS *scan(OBJECTS *objs){
	return objs;
}

char findFood(OBJECTS *objs){
	int i,j;
	char *loc;
	char *inv=objs->inventory;
	if(inv==0)	return 0;
	for(i=0;i<ROWS;i+=ROWS){
		loc=strstr(inv+i,"food");
		if(loc){
			loc=loc-((loc-inv)%COLS);
			loc=strstr(loc,")");
			return *(loc-1);
		}
		loc=strstr(inv+i,"slime");
		if(loc){
			loc=loc-((loc-inv)%COLS);
			loc=strstr(loc,")");
			return *(loc-1);
		}
	}
	return 0;
}

int updateArea(OBJECTS *objs){
	GRAPH *g;
	char *map;
	POINT *loc;
	if(objs==0){
		printf("updateArea requires initialized pointer\n");
		return 0;
	}
	updateScreen(objs);
	map=objs->map;
	if(map==0){
		printf("error, map==0\n");
		return 0;
	}

	free(objs->self);
	objs->self=findSelf(objs->map);
	if(objs->self==0){
/*
		printf("unable to find self\n");
		space(objs->fdin);
		while(1);
*/
		while(objs->self==0){
			space(objs->fdin);
			updateScreen(objs);
			objs->self=findSelf(objs->map);
		}
	}
	loc=objs->self;
	g=objs->graph;
	if(g==0){
		g=createGraph();
	}

	objs->graph=g;
	buildGraph(objs,loc);
	//buildGraph(g,map,loc);
	//fillGraph(g,map);
	//printGraph(g);

	return 1;
}

int moveToPoint(int fdin,OBJECTS *objs,POINT *pt){
	int result;
	result=updateArea(objs);
	if(result==0)	return 0;

	nearestPoint(objs,pt);
	return moveTowards(objs,pt);
}

OBJECTS *scanArea(OBJECTS *objs){
	int result;
	result=updateArea(objs);
	if(result==0)	return 0;

	if(objs->stairs==0){
		objs->stairs=nearestStairs(objs);
		if(objs->stairs){
			printf("stairs at:");
			print(objs->stairs);
		}
	}
/*
	free(objs->enemy);
	free(objs->item);
	free(objs->door);

	objs->door=nearestDoor(objs);
	objs->item=nearestItem(objs);
	objs->enemy=nearestEnemy(objs);
*/
	near(objs);	
//	freeGraph(g);
	return objs;
}

#define PRINTOBJS(x,s)		\
	printf("%s:\t",s);			\
	print(o->x);						\

void printObjs(OBJECTS *o){
	PRINTOBJS(self,"self");
	PRINTOBJS(enemy,"enemy");
	PRINTOBJS(item,"item");
	PRINTOBJS(door,"door");
}

POINT *findSelf(char *map){
	int i,j;
	POINT *pt=malloc(sizeof(POINT));
	map+=80;
	for(i=1;i<ROWS;i++){
		for(j=0;j<COLS;j++){
			if(*map++=='@'){
				pt->x=j;
				pt->y=i;
				return pt;
			}
		}
	}
	return 0;
}

void print(POINT *pt){
	if(pt)
		printf("(%d,%d)\n",pt->x,pt->y);
}

int isDoor(char loc){
	if(loc=='+')	return 1;
	return 0;
}

int isNothing(char loc){
	if(loc==' ')	return 1;
	return 0;
}

int isTunnel(char loc){
	if(loc=='#')	return 1;
	return 0;
}

int isFloor(char loc){
	if(loc=='.')	return 1;
	return 0;
}

int isItem(char loc){
	if(
		!(isalpha(loc))&&
		loc!=' '&&
		loc!='#'&&
		loc!='.'&&
		loc!='-'&&
		loc!='+'&&
		loc!='%'&&
		loc!='^'&&
		loc!='|'
		){
		return 1;
	}
	return 0;
}

int ofInterest(char loc){
//	printf("%c\n",loc);
	if(
		loc!='.'&&
		loc!='-'&&
		loc!='|'
		){
		return 1;
	}
	return 0;
}

int isStairs(char loc){
	return (loc=='%');
}

int isEnemy(char loc){
	return isalpha(loc);
}

POINT *nearestPoint(OBJECTS *objs,POINT *target){
	GRAPH *g=objs->graph;
	char *map=objs->map;
	POINT *loc=objs->self;
	VERTEX *vert;
	int vIndex=INDEX(loc->y,loc->x);
	int tIndex=INDEX(target->y,target->x);
	LIST *lp,*vp;
	POINT *pt=NEW(POINT);
	LIST *q=createList();
	VERTEX *s=g->vertex[vIndex];
	lp=g->vList[vIndex];
	int visited[ROWS*COLS];
	bzero(visited,4*ROWS*COLS);
	visited[s->val]=1;
	addList(q,s);
//	printList(q);
	while(!emptyList(q)){
		vp=dequeue(q);
		lp=g->vList[vp->v->val];
		while(lp->next){
			lp=lp->next;
			vert=lp->v;
			if(!(visited[vert->val])&&(vert!=s)){
				vert->pre=vp->v;
				if(tIndex==vert->val){
					freeList(q);
					pt->x=vert->val%80;
					pt->y=vert->val/80;
					POINT *nextStep=NEW(POINT);
				while(vert->pre){
					if(vert->pre==s){
									nextStep->x=vert->val%80;
									nextStep->y=vert->val/80;
#ifdef DEBUGSTEPS
									print(nextStep);
#endif
									objs->nextStep=nextStep;
//									printf("%c\n",map[INDEX(nextStep->y,nextStep->x)]);
									return pt;
					}else{
									nextStep->x=vert->val%80;
									nextStep->y=vert->val/80;
#ifdef DEBUGSTEPS
									print(nextStep);
#endif
						vert=vert->pre;
					}
				}
printf("unable to trace predecessors back to source.  bug in sight.c:nearest()\n");
return 0;
				}
				visited[vert->val]=1;
				addList(q,vert);
			}
		}
	}
	freeList(q);
	free(pt);
#ifdef DEBUG
//	printf("not found\n");
#endif
	return 0;
}

void near(OBJECTS *objs){
	freeList(objs->seenEnemies);
	freeList(objs->seenTunnels);
	freeList(objs->seenItems);
	freeList(objs->seenDoors);
	GRAPH *g=objs->graph;
	char *map=objs->map;
	int vIndex=objs->player->val;
	VERTEX *vert;
	LIST *lp,*vp;
	LIST *q=createList();
	VERTEX *s=g->vertex[vIndex];
	lp=g->vList[vIndex];
	int visited[ROWS*COLS];
	bzero(visited,4*ROWS*COLS);
	visited[s->val]=1;
	s->sDist=0;
	addList(q,s);
//	printList(q);
	while(!emptyList(q)){
		vp=dequeue(q);
		lp=g->vList[vp->v->val];
		//printf("%d?=%d\n",vIndex,vp->v->val);
		while(lp->next){
			lp=lp->next;
			vert=lp->v;
			if(!(visited[vert->val])&&(vert!=s)){
				vert->pre=vp->v;
				vert->sDist=vp->v->sDist+1;
//				if(f(map[vert->val])){
				char ch=map[vert->val];
				if(isTunnel(ch)){
					if(!findListValue(objs->visitedTunnels,(vert->val)))
						addList(objs->seenTunnels,(vert));
				}else if(isDoor(ch)){
				//if(isDoor(ch)){
					if(!findListValue(objs->visitedDoors,(vert->val))){
						addList(objs->seenDoors,(vert));
					}
				}else if(isEnemy(ch)){
					addList(objs->seenEnemies,(vert));
				}else if(isItem(ch)){
					if(!findListValue(objs->visitedItems,(vert->val))){
						addList(objs->seenItems,(vert));
					}
				}else if(isStairs(ch)){
					addList(objs->seenStairs,(vert));
				}
				visited[vert->val]=1;
				addList(q,vert);
			}
		}
	}
	printf("\n");
	printf("doors:");
	printList(objs->seenDoors);
	printf("tunnels:");
	printList(objs->seenTunnels);
	printf("enemies:");
	printList(objs->seenEnemies);
	printf("items:");
	printList(objs->seenItems);
	printf("stairs:");
	printList(objs->seenStairs);
	printf("visited tunnels:");
	printList(objs->visitedTunnels);
	freeList(q);
}


POINT *nearest(OBJECTS *objs,int f(char)){
	GRAPH *g=objs->graph;
	char *map=objs->map;
//	char *graph=objs->g;
	POINT *loc=objs->self;
	VERTEX *vert;
	int vIndex=INDEX(loc->y,loc->x);
	LIST *lp,*vp;
	POINT *pt=NEW(POINT);
	LIST *q=createList();
	//VERTEX *s=g->vertex[vIndex];
	VERTEX *s=g->vertex[objs->player->val];
	lp=g->vList[vIndex];
	int visited[ROWS*COLS];
	bzero(visited,4*ROWS*COLS);
	visited[s->val]=1;
	addList(q,s);
//	printList(q);
	while(!emptyList(q)){
		vp=dequeue(q);
		lp=g->vList[vp->v->val];
		//printf("%d?=%d\n",vIndex,vp->v->val);
		while(lp->next){
			lp=lp->next;
			vert=lp->v;
			if(!(visited[vert->val])&&(vert!=s)){
int tmpx0,tmpy0,tmpx1,tmpy1;
tmpx0=vp->v->val%COLS;
tmpy0=vp->v->val/COLS;
tmpx1=vert->val%COLS;
tmpy1=vert->val/COLS;
if(abs(tmpx0-tmpx1)>1){
printf("error\n");
}
if(abs(tmpy0-tmpy1)>1){
printf("error\n");
}
				vert->pre=vp->v;
if(	!findListValue(objs->visitedDoors,(vert->val)) &&
		!findListValue(objs->visitedItems,(vert->val)) &&
		!findListValue(objs->visitedTunnels,(vert->val))	)					//should maybe add another function to parameter list to not check these in every case.
				if(f(map[vert->val])){

					freeList(q);
					pt->x=vert->val%80;
					pt->y=vert->val/80;
					POINT *nextStep=NEW(POINT);
/*
					print(pt);
					printf("%c\n",map[INDEX(pt->y,pt->x)]);
					return pt;
*/

					while(vert->pre){
						if(vert->pre==s){
							nextStep->x=vert->val%80;
							nextStep->y=vert->val/80;
#ifdef DEBUGSTEPS
							print(nextStep);
							printf("%c\n",map[INDEX(nextStep->y,nextStep->x)]);
#endif
							objs->nextStep=nextStep;
							if(pt->y==0){
								printf("error, trying to return point (%d,%d) in nearest()\n",pt->x,pt->y);
								getchar();
							}
							return pt;
						}else{
							nextStep->x=vert->val%80;
							nextStep->y=vert->val/80;
#ifdef DEBUGSTEPS
							print(nextStep);
#endif
							vert=vert->pre;
						}
					}
					printf("unable to trace predecessors back to source.  bug in sight.c:nearest()\n");
					return 0;
				}
				visited[vert->val]=1;
				addList(q,vert);
			}
		}
	}
	freeList(q);
	free(pt);
#ifdef DEBUG
//	printf("not found\n");
#endif
	return 0;
}

POINT *nearestObject(OBJECTS *objs){
	return nearest(objs,ofInterest);
}
POINT *nearestItem(OBJECTS *objs){
	return nearest(objs,isItem);
}
POINT *nearestEnemy(OBJECTS *objs){
	return nearest(objs,isEnemy);
}
POINT *nearestDoor(OBJECTS *objs){
	return nearest(objs,isDoor);
}
POINT *nearestStairs(OBJECTS *objs){
	return nearest(objs,isStairs);
}
POINT *nearestTunnel(OBJECTS *objs){
	return nearest(objs,isTunnel);
}

char *lastStatus(char *map){
	char *last=malloc(80);
	strncpy(last,map+(23*COLS),80);
	return last;
}

char *lastMessage(char *map){
	char *last=malloc(80);
	strncpy(last,map,80);
	return last;
}

int checkInventory(OBJECTS *objs){
	int n;
	int fdin=objs->fdin;
	char buf='i';
	n=write(fdin,&buf,1);
	printf("wrote %d bytes\n",n);
	parseInventory(objs);
	space(objs->fdin);
}

int checkFound(char *map){
	char *last=lastMessage(map);
	if(strstr(last,"Nothing appropriate")){
		free(last);
		return 0;
	}
	free(last);
	return 1;
}

int checkWeak(char *map){
	char *last=lastStatus(map);
	if(strstr(last,"weak")){
		free(last);
		return 1;
	}
	free(last);
	return 0;
}

char *fillHP(char *p,OBJECTS *objs){
	int tmp;
	char buf[32];
	char *t;
	bzero(buf,32);
	p=strstr(p,"Hp:");
	if(p==0){
		printf("error\n");
		while(1);
	}
	p+=4;
	t=p;
	while(isdigit(*t)){
		t++;
	}
	strncpy(buf,p,(t-p));
	tmp=atoi(buf);
	objs->hp=tmp;

	p=t+1;
	t=p;
	while(isdigit(*t)){
		t++;
	}
	strncpy(buf,p,(t-p));
	tmp=atoi(buf);
	objs->maxhp=tmp;

	objs->hpratio=objs->hp*100/objs->maxhp;

	return t;
}

void parseStatus(OBJECTS *objs){
	char *map=objs->map;
	char *p=lastStatus(map);
	fillHP(p,objs);
	free(p);
}

int checkHungry(char *map){
	char *last=lastStatus(map);
	if(strstr(last,"hungry")){
		free(last);
		return 1;
	}
	free(last);
	return 0;
}

int checkTrapped(char *map){
	char *last=lastStatus(map);
	if(strstr(last,"bear trap")){
		free(last);
		return 1;
	}
	free(last);
	return 0;
}

int checkDescent(char *map){
	char *last=lastMessage(map);
	if(strstr(last,"no way down")){
		free(last);
		return 0;
	}
	free(last);
	return 1;
}

int checkItem(char *map){
	char *last=lastMessage(map);
	if(strstr(last,"moved onto")){
		free(last);
		return 1;
	}
	free(last);
	return 0;
}

int checkFaint(char *map){
	char *last=lastStatus(map);
	if(strstr(last,"faint")){
		free(last);
		return 1;
	}
	free(last);
	return 0;
}

int checkGameOver(char *map){
	getchar();
	char *last=map+3*COLS;
	if(strstr(last,"Rogueists")){
		return 1;
	}
	return 0;
}

int checkMore(char *map){
	char *last=lastMessage(map);
	if(strstr(last,"More")){
		free(last);
		return 1;
	}
	free(last);
	return 0;
}

int enemyDefeated(char *map){
	char *last=lastMessage(map);
	if(strstr(last,"Defeated")){
		free(last);
		return 1;
	}
	free(last);
	return 0;
}

char *getSurroundings(OBJECTS *objs){
	int x,y;
	x=objs->self->x;
	y=objs->self->y;
	POINT *self=objs->self;
	char *map=objs->map;
	char *results=malloc(4);
	results[0]=map[INDEX(self->y+1,self->x)];
	results[1]=map[INDEX(self->y-1,self->x)];
	results[2]=map[INDEX(self->y,self->x+1)];
	results[3]=map[INDEX(self->y,self->x-1)];
	return results;
}

void updateState(OBJECTS *objs){
	int i;
	char ch;
	char *surr=getSurroundings(objs);
	int nTunnel=0;
	int nFloor=0;
	int nNothing=0;
	int nOther=0;
	int nEnemy=0;

	if(checkHungry(objs->map)){
		printf("hungry\n");
		objs->status |= HUNGRY;
	}else if(checkFaint(objs->map)){
		printf("dying of starvation\n");
		objs->status |= STARVING;
	}else if(checkWeak(objs->map)){
		printf("weak with hunger\n");
		objs->status |= WEAK;
	}else{
		objs->status &= ~(HUNGRY|STARVING|WEAK);
	}

	if(checkMore(objs->map)){
		printf("cleared more prompt\n");
		space(objs->fdin);
		updateScreen(objs);
		if(checkGameOver(objs->map)){
			printf("game over\n");
			printf("should add game over state\n");
			space(objs->fdin);
			while(1);
		}
	}

	parseStatus(objs);

	if(checkItem(objs->map)){
		printf("inventory full and at item location.  marking item as visited.\n");
		//better to put this in decisions...
		if(tryQuaff(objs)){
			pickupItem(objs);
		}else if(tryRead(objs)){
			pickupItem(objs);
		}else{
			markItem(objs);
		}
	}

	for(i=0;i<4;i++){
		ch=surr[i];
		if(isTunnel(ch)){
			nTunnel++;
		}else if(isFloor(ch)){
			nFloor++;
		}else if(isNothing(ch)){
			nNothing++;
		}else if(isEnemy(ch)){
			nEnemy++;
		}else{
			nOther++;
		}
	}
	free(surr);
	if((nTunnel==1)&&(nNothing==3)){
		if(!findListValue(objs->deadEnds,INDEX(objs->self->y,objs->self->x))){
			objs->state=searching;
		}
		return;
	}else if((nTunnel==1)&&(nFloor==1)){
		markDoor(objs);
		return;
	//}else if(nOther>0){
	}else if(nEnemy>0){
		return;
	}else if(nFloor>1){
		return;
//	}
//	if((objs->state!=idle)&&(objs->state!=movingToStairs)){
	}else{
		printf("possibly in tunnel\n");
		markTunnel(objs);
//		objs->state=inTunnel;
//		objs->state=idle;
	}
}

int isInDeadEnd(OBJECTS *objs){
	int i;
	char ch;
	char *surr=getSurroundings(objs);
	int nTunnel=0;
	int nNothing=0;
	for(i=0;i<4;i++){
		ch=surr[i];
		if(isTunnel(ch)){
			nTunnel++;
		}else if(isNothing(ch)){
			nNothing++;
		}
	}
	free(surr);
	if((nTunnel==1)&&(nNothing==3)){
		return 1;
	}
	return 0;
}

int isInDoorway(OBJECTS *objs){
	int i;
	char ch;
	char *surr=getSurroundings(objs);
	int nTunnel=0;
	int nFloor=0;
	for(i=0;i<4;i++){
		ch=surr[i];
		if(isTunnel(ch)){
			nTunnel++;
		}else if(isFloor(ch)){
			nFloor++;
		}
	}
	free(surr);
	if((nTunnel==1)&&(nFloor==1)){
		return 1;
	}
	return 0;
}

int isInTunnel(OBJECTS *objs){
	int i;
	char ch;
	char *surr=getSurroundings(objs);
	for(i=0;i<4;i++){
		ch=surr[i];
		if(!isTunnel(ch)&&(!isNothing(ch))){
			free(surr);
			return 0;
		}
	}
	free(surr);
	return 1;
}

VERTEX *findPredecessor(OBJECTS *objs,VERTEX *vert){
	VERTEX *source=objs->player;
	while(vert->pre){
		if(vert->pre==source){
			return vert;
		}else{
			vert=vert->pre;
		}
	}
	printf("unable to trace predecessors back to source.  bug in sight.c:findPredecessor()\n");
	return 0;
}
