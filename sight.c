#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sight.h>

POINT *findSelf(char *map){
	int i,j;
	POINT *pt=malloc(sizeof(POINT));
	for(i=0;i<24;i++){
		for(j=0;j<80;j++){
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
