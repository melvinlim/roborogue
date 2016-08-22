#include<stdio.h>
#include<stdlib.h>
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
	for(i=0;i<(COLUMNS*ROWS);i++){
		g->vList[i]=createList();
		g->vertex[i]=0;
		//g->vertex[i]=NEW(VERTEX);
	}
	return g;
}

int validTile(char tile){
	if(tile!=' '){
		return 1;
	}
	return 0;
}

void freeGraph(GRAPH *g){
	int i;
	for(i=0;i<(COLUMNS*ROWS);i++){
		freeList(g->vList[i]);
		free(g->vertex[i]);
	}
	free(g);
}

void checkTile(GRAPH *g,char *map,int i,int j,int k){
	int t;
	VERTEX *v;
	if((i<0)||(i>=ROWS))	return;
	if((j<0)||(j>=COLUMNS))	return;
	t=INDEX(i,j);
	if(validTile(map[t])){
		if(g->vertex[t]==0){
			v=NEW(VERTEX);
			v->val=t;
			g->vertex[t]=v;
		}else{
			v=g->vertex[t];
		}
		addList(g->vList[k],v);
	}
}

void fillGraph(GRAPH *g,char *map){
	int i,j,k=0;
	int t;
	for(i=0;i<ROWS;i++){
		for(j=0;j<COLUMNS;j++){
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
	for(i=0;i<(COLUMNS*ROWS);i++){
		p=g->vList[i];
		if(p->next){
			printf("g->v[%d]:\t",i);
			printList(p);
		}
	}
}
