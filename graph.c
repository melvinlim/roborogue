#include<stdlib.h>
#include<definitions.h>
#include<graph.h>
#include<list.h>

GRAPH *createGraph(){
	int i;
	GRAPH *g=malloc(sizeof(GRAPH));
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

void fillGraph(GRAPH *g,char *map){
	int i,j,k=0;
	int t;
	for(i=0;i<ROWS;i++){
		for(j=0;j<COLUMNS;j++){
			if(map[k]!=' '){
				t=INDEX(i,j);
				if(validTile(map[t])){
					addList(g->v[k],t);
				}
			}
			k++;
		}
	}
}
