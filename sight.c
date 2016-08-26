#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sight.h>

#include<graph.h>

OBJECTS *createObjects(){
	OBJECTS *objs=NEW(OBJECTS);
	bzero(objs,sizeof(OBJECTS));
	objs->visitedDoors=createList();
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
			loc=strstr(inv+i,")");
			return *(loc-1);
		}
	}
	return 0;
}

int moveToPoint(int fdin,OBJECTS *objs,POINT *pt){
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

	//nearestPoint(objs,g,pt);
	nearestPoint(objs,pt);
	return moveTowards(fdin,objs,pt);
}

OBJECTS *scanArea(OBJECTS *objs){
	GRAPH *g;
	char *map;
	POINT *loc;
	if(objs==0){
		printf("scanArea requires initialized pointer\n");
		return 0;
	}
	//map=updateScreen(objs);
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

	if(objs->stairs==0){
		objs->stairs=nearestStairs(objs);
		if(objs->stairs){
			printf("stairs at:");
			print(objs->stairs);
		}
		//objs->stairs=nearestStairs(objs,g);
	}

	free(objs->enemy);
	free(objs->item);
	free(objs->door);
/*
	objs->door=nearestDoor(objs,g);
	objs->item=nearestItem(objs,g);
	objs->enemy=nearestEnemy(objs,g);
*/
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

int isTunnel(char loc){
	if(loc=='#')	return 1;
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
//		loc!='>'&&
//		loc!='<'&&
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

//POINT *nearestPoint(OBJECTS *objs,GRAPH *g,POINT *target){
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

//POINT *nearest(OBJECTS *objs,GRAPH *g,int f(char)){
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
if(!findListValue(objs->visitedDoors,(vert->val)))					//should maybe add another function to parameter list to not check this in every case.
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

//POINT *nearestObject(OBJECTS *objs,GRAPH *g){
POINT *nearestObject(OBJECTS *objs){
	return nearest(objs,ofInterest);
	//return nearest(objs,g,ofInterest);
}
//POINT *nearestItem(OBJECTS *objs,GRAPH *g){
POINT *nearestItem(OBJECTS *objs){
	return nearest(objs,isItem);
	//return nearest(objs,g,isItem);
}
//POINT *nearestEnemy(OBJECTS *objs,GRAPH *g){
POINT *nearestEnemy(OBJECTS *objs){
	return nearest(objs,isEnemy);
	//return nearest(objs,g,isEnemy);
}
//POINT *nearestDoor(OBJECTS *objs,GRAPH *g){
POINT *nearestDoor(OBJECTS *objs){
	return nearest(objs,isDoor);
	//return nearest(objs,g,isDoor);
}

//POINT *nearestStairs(OBJECTS *objs,GRAPH *g){
POINT *nearestStairs(OBJECTS *objs){
	return nearest(objs,isStairs);
	//return nearest(objs,g,isStairs);
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

int isInTunnel(OBJECTS *objs){
	char *map=objs->map;
	POINT *self=objs->self;
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
}
