#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sight.h>

#include<graph.h>

OBJECTS *createObjects(){
	OBJECTS *objs=NEW(OBJECTS);
	bzero(objs,sizeof(OBJECTS));
	objs->visitedDoors=createList();
	objs->visitedTunnels=createList();
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
		printf("scanArea requires initialized pointer\n");
		return 0;
	}
	updateScreen(objs);
	map=objs->map;
	if(map==0){
		printf("error, map==0\n");
		return 0;
	}

	free(objs->self);
	objs->self=findSelf(map);
	if(objs->self==0){
		space(objs->fdin);
	}
	loc=objs->self;
	g=objs->graph;
	if(g==0){
		g=createGraph();
	}

	buildGraph(g,map,loc);
	//fillGraph(g,map);
	//printGraph(g);

	objs->graph=g;
	return 1;
}

int moveToPoint(int fdin,OBJECTS *objs,POINT *pt){
	int result;
	result=updateArea(objs);
	if(result==0)	return 0;

	nearestPoint(objs,pt);
	return moveTowards(fdin,objs,pt);
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

	free(objs->enemy);
	free(objs->item);
	free(objs->door);

	objs->door=nearestDoor(objs);
	objs->item=nearestItem(objs);
	objs->enemy=nearestEnemy(objs);
	
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
		//printf("%d?=%d\n",vIndex,vp->v->val);
		while(lp->next){
			lp=lp->next;
			vert=lp->v;
			if(!(visited[vert->val])&&(vert!=s)){
				vert->pre=vp->v;
if(	!findListValue(objs->visitedDoors,(vert->val)) &&
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

/*
//POINT *nearestUnvisitedDoor(char *map,GRAPH *g,POINT *loc){
POINT *nearestUnvisitedDoor(OBJECTS *objs,GRAPH *g){
	return nearest(objs,g,isDoor);
	//return nearest(map,g,loc,isDoor);
}
*/
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

int checkHungry(char *map){
	char *last=lastStatus(map);
	if(strstr(last,"hungry")){
		free(last);
		return 1;
	}
	free(last);
	return 0;
}

int checkFaint(char *map){
	char *last=lastMessage(map);
	if(strstr(last,"faint")){
		free(last);
		return 1;
	}
	free(last);
	return 0;
}

int checkGameOver(char *map){
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
/*
	POINT *self=objs->self;
	char *map=objs->map;
	char ch;
	ch=map[INDEX(self->y+1,self->x)];
	if(!isTunnel(ch)&&(!isNothing(ch))){
		return 0;
	}
	ch=map[INDEX(self->y-1,self->x)];
	if(!isTunnel(ch)&&(!isNothing(ch))){
		return 0;
	}
	ch=map[INDEX(self->y,self->x+1)];
	if(!isTunnel(ch)&&(!isNothing(ch))){
		return 0;
	}
	ch=map[INDEX(self->y,self->x-1)];
	if(!isTunnel(ch)&&(!isNothing(ch))){
		return 0;
	}
	return 1;
	if(isFloor(map[INDEX(self->y+1,self->x)])){
		return 0;
	}
	if(isFloor(map[INDEX(self->y-1,self->x)])){
		return 0;
	}
	if(isFloor(map[INDEX(self->y,self->x+1)])){
		return 0;
	}
	if(isFloor(map[INDEX(self->y,self->x-1)])){
		return 0;
	}
	if(isDoor(map[INDEX(self->y+1,self->x)])){
		return 0;
	}
	if(isDoor(map[INDEX(self->y-1,self->x)])){
		return 0;
	}
	if(isDoor(map[INDEX(self->y,self->x+1)])){
		return 0;
	}
	if(isDoor(map[INDEX(self->y,self->x-1)])){
		return 0;
	}
	if(isTunnel(map[INDEX(self->y+1,self->x)])){
		return 1;
	}
	if(isTunnel(map[INDEX(self->y-1,self->x)])){
		return 1;
	}
	if(isTunnel(map[INDEX(self->y,self->x+1)])){
		return 1;
	}
	if(isTunnel(map[INDEX(self->y,self->x-1)])){
		return 1;
	}
	return 0;
*/
}
