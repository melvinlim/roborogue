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
		g->v[i]=createList();
	}
	return g;
}

int validTile(char tile){
	if(tile!=' '){
		return 1;
	}
	return 0;
}

void checkTile(GRAPH *g,char *map,int i,int j,int k){
	int t;
	if((i<0)||(i>ROWS))	return;
	if((j<0)||(j>COLUMNS))	return;
	t=INDEX(i,j);
	if(validTile(map[t])){
		addList(g->v[k],t);
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
		p=g->v[i];
		if(p->next){
			printf("g->v[%d]:\t",i);
			printList(p);
		}
	}
}
