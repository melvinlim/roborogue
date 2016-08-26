#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<definitions.h>
#include<graph.h>
#include<list.h>

GRAPH *createGraph(){
	int i;
	GRAPH *g=malloc(sizeof(GRAPH));
	if(g==0){
		printf("error: could not allocate memory\n");
		return g;
	}
	for(i=80;i<(COLS*ROWS);i++){
		g->vList[i]=createList();
		g->vertex[i]=0;
	}
	return g;
}

int validTile(char tile){
#ifdef DEBUG
//	printf("checking tile: %c\n",tile);
#endif
	//if(tile!=' '){
	if(
		(tile!=' ')&&
		(tile!='|')&&
		(tile!='-')
		){
		return 1;
	}
	return 0;
}

void freeGraph(GRAPH *g){
	int i;
	for(i=80;i<(COLS*ROWS);i++){
		freeList(g->vList[i]);
		free(g->vertex[i]);
	}
	free(g);
}

VERTEX *checkTile(GRAPH *g,char *map,int i,int j,int k){
	int t;
	VERTEX *v;
	if((i<1)||(i>=ROWS))		return 0;		//first row is status bar
	if((j<0)||(j>=COLS))	return 0;
	t=INDEX(i,j);
/*
	v=findListValue(g->vList[k],t);
	if(v){
		return v;
	}
*/
	if(validTile(map[t])){
		if(g->vertex[t]==0){
			v=NEW(VERTEX);
			v->val=t;
			g->vertex[t]=v;
		}else{
			v=g->vertex[t];
		}
		addList(g->vList[k],v);
		return v;
	}
	return 0;
}

void buildGraph(GRAPH *g,char *map,POINT *loc){
	int visited[ROWS*COLS];
	int i,j,k=0;
	LIST *next,*q;
	VERTEX *v;
	i=loc->y;
	j=loc->x;
	k=INDEX(i,j);
	q=createList();
	v=checkTile(g,map,i+0,j+0,k);
	addList(q,v);

	bzero(visited,ROWS*COLS*4);

	while(!emptyList(q)){
		next=dequeue(q);
		v=next->v;

		if(!visited[v->val]){

			i=v->val/COLS;
			j=v->val%COLS;
			k=INDEX(i,j);

			v=checkTile(g,map,i+1,j+0,k);
			if(v){
				addList(q,v);
			}
			v=checkTile(g,map,i+0,j+1,k);
			if(v){
				addList(q,v);
			}
			v=checkTile(g,map,i-0,j-1,k);
			if(v){
				addList(q,v);
			}
			v=checkTile(g,map,i-1,j-0,k);
			if(v){
				addList(q,v);
			}

			visited[k]=1;

		}
		
	}

}

void fillGraph(GRAPH *g,char *map){
	int i,j,k=0;
	int t;
	k=80;
	for(i=1;i<ROWS;i++){
		for(j=0;j<COLS;j++){
			if(map[k]!=' '){
				checkTile(g,map,i+1,j+0,k);
				checkTile(g,map,i+0,j+1,k);
				checkTile(g,map,i-0,j-1,k);
				checkTile(g,map,i-1,j-0,k);
			}
			k++;
		}
	}
}

void printGraph(GRAPH *g){
	int i;
	LIST *p;
	for(i=80;i<(COLS*ROWS);i++){
		p=g->vList[i];
		if(p->next){
			printf("g->v[%4d/(%2d,%2d)]:\t",i,i/80,i%80);
			printList(p);
		}
	}
}
