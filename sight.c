#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sight.h>

#include<graph.h>

POINT *findSelf(char *map){
	int i,j;
	POINT *pt=malloc(sizeof(POINT));
	for(i=0;i<ROWS;i++){
		for(j=0;j<COLUMNS;j++){
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

int isItem(char loc){
	if(
		loc!='.'&&
		loc!='-'&&
		loc!='+'&&
		loc!='>'&&
		loc!='<'&&
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

int isEnemy(char loc){
	return isalpha(loc);
}
/*
POINT *nearestEnemy(char *map,GRAPH *g,POINT *loc){
	VERTEX *vert;
	int vIndex=INDEX(loc->y,loc->x);
	LIST *lp,*vp;
	POINT *pt=NEW(POINT);
	LIST *q=createList();
	VERTEX *s=g->vertex[vIndex];
	lp=g->vList[vIndex];
	//s->visited=1;
	int visited[ROWS*COLUMNS];
	bzero(visited,4*ROWS*COLUMNS);
	addList(q,s);
//	printList(q);
	while(!emptyList(q)){
		vp=dequeue(q);
		lp=g->vList[vp->v->val];
		//printf("%d?=%d\n",vIndex,vp->v->val);
		while(lp->next){
			lp=lp->next;
			vert=lp->v;
			//if((vert->visited==0)&&(vert!=s)){
			if(!(visited[vert->val])&&(vert!=s)){
				if(isEnemy(map[vert->val])){
					pt->x=vert->val%80;
					pt->y=vert->val/80;
					print(pt);
					printf("%c\n",map[INDEX(pt->y,pt->x)]);
					freeList(q);
					return pt;
				}
				//vert->visited=1;
				visited[vert->val]=1;
				addList(q,vert);
			}
		}
	}
	freeList(q);
	pt->x=pt->y=0;
	print(pt);
	return pt;
}
*/
/*
POINT *nearestItem(char *map,GRAPH *g,POINT *loc){
	VERTEX *vert;
	int vIndex=INDEX(loc->y,loc->x);
	LIST *lp,*vp;
	POINT *pt=NEW(POINT);
	LIST *q=createList();
	VERTEX *s=g->vertex[vIndex];
	lp=g->vList[vIndex];
	int visited[ROWS*COLUMNS];
	bzero(visited,4*ROWS*COLUMNS);
	visited[s->val]=1;
//	s->visited=1;
	addList(q,s);
//	printList(q);
	while(!emptyList(q)){
		vp=dequeue(q);
		lp=g->vList[vp->v->val];
		//printf("%d?=%d\n",vIndex,vp->v->val);
		while(lp->next){
			lp=lp->next;
			vert=lp->v;
			//if((vert->visited==0)&&(vert!=s)){
			if(!(visited[vert->val])&&(vert!=s)){
				if(isItem(map[vert->val])){
					pt->x=vert->val%80;
					pt->y=vert->val/80;
					print(pt);
					printf("%c\n",map[INDEX(pt->y,pt->x)]);
					freeList(q);
					return pt;
				}
				visited[vert->val]=1;
				//vert->visited=1;
				addList(q,vert);
			}
		}
	}
	freeList(q);
	pt->x=pt->y=0;
	print(pt);
	return pt;
}
*/

//POINT *nearest(char *map,GRAPH *g,POINT *loc){
POINT *nearest(char *map,GRAPH *g,POINT *loc,int f(char)){
	VERTEX *vert;
	int vIndex=INDEX(loc->y,loc->x);
	LIST *lp,*vp;
	POINT *pt=NEW(POINT);
	LIST *q=createList();
	VERTEX *s=g->vertex[vIndex];
	lp=g->vList[vIndex];
	int visited[ROWS*COLUMNS];
	bzero(visited,4*ROWS*COLUMNS);
	visited[s->val]=1;
//	s->visited=1;
	addList(q,s);
//	printList(q);
	while(!emptyList(q)){
		vp=dequeue(q);
		lp=g->vList[vp->v->val];
		//printf("%d?=%d\n",vIndex,vp->v->val);
		while(lp->next){
			lp=lp->next;
			vert=lp->v;
			//if((vert->visited==0)&&(vert!=s)){
			if(!(visited[vert->val])&&(vert!=s)){
				//if(ofInterest(map[vert->val])){
				if(f(map[vert->val])){
					pt->x=vert->val%80;
					pt->y=vert->val/80;
					print(pt);
					printf("%c\n",map[INDEX(pt->y,pt->x)]);
					freeList(q);
					return pt;
				}
				visited[vert->val]=1;
				//vert->visited=1;
				addList(q,vert);
			}
		}
	}
	freeList(q);
	pt->x=pt->y=0;
	print(pt);
	return pt;
}

POINT *nearestObject(char *map,GRAPH *g,POINT *loc){
	return nearest(map,g,loc,ofInterest);
}
POINT *nearestItem(char *map,GRAPH *g,POINT *loc){
	return nearest(map,g,loc,isItem);
}
POINT *nearestEnemy(char *map,GRAPH *g,POINT *loc){
	return nearest(map,g,loc,isEnemy);
}

char *lastMessage(char *map){
	char *last=malloc(80);
	strncpy(last,map,80);
	return last;
}

int checkMore(char *map){
	char *last=lastMessage(map);
	if(strstr(last,"More")){
		return 1;
	}
	return 0;
}
