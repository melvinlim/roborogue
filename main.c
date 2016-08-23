#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>

#include<sys/resource.h>

#include<sight.h>
#include<decode.h>
#include<graph.h>
#include<move.h>

#define DEBUG

int main(int argc,char *argv[]){
	int fdin,fdout,i,j,n;
	char *map;
	char buf[256];
	char ch;
	if(argc<=2){
		printf("usage: %s [output file] [process number]\n",argv[0]);
		return 0;
	}

	struct rlimit rlim;
	getrlimit(RLIMIT_STACK,&rlim);
	rlim.rlim_cur=rlim.rlim_max;
	setrlimit(RLIMIT_STACK,&rlim);
	getrlimit(RLIMIT_DATA,&rlim);
	rlim.rlim_cur=rlim.rlim_max;
	setrlimit(RLIMIT_DATA,&rlim);
	getrlimit(RLIMIT_AS,&rlim);
	rlim.rlim_cur=rlim.rlim_max;
	setrlimit(RLIMIT_AS,&rlim);
	//rlim.rlim_cur=rlim.rlim_max=RLIM_INFINITY;
	//setrlimit(RLIMIT_STACK,&rlim);
	//setrlimit(RLIMIT_AS,&rlim);

	sprintf(buf,"%s",argv[1]);
	printf("target file: %s\n",buf);
	fdout=open(buf,O_RDONLY);
	if(fdout<0){
		printf("unable to open %s\n",buf);
		return 0;
	}
	printf("fdout=%d\n",fdout);
	sprintf(buf,"/proc/%s/fd/0",argv[2]);
	printf("target process: %s\n",buf);
	fdin=open(buf,O_WRONLY);
	if(fdin<0){
		printf("unable to open %s\n",buf);
		return 0;
	}
	printf("fdin=%d\n",fdin);

//	move(fdin,UP);

//	space(fdin);
//	move(fdin,DOWN);
	//map=printScreen(fdout);
	map=updateScreen(fdout,0);
//	printf("%d %d\n",fdin,fdout);
//	quit(fdin);
	if(map==0){
		printf("error\n");
		return 0;
	}
	POINT *loc=findSelf(map);
	POINT *enemyLoc,*itemLoc,*doorLoc;
	printf("player location found at: ");
	print(loc);
	GRAPH *g=createGraph();
	fillGraph(g,map);
//	printGraph(g);
	nearestObject(map,g,loc);
	itemLoc=nearestItem(map,g,loc);
	enemyLoc=nearestEnemy(map,g,loc);
	doorLoc=nearestDoor(map,g,loc);
/*
	moveTowards(fdin,itemLoc,loc);
	enemyLoc=nearestEnemy(map,g,loc);
	moveTowards(fdin,enemyLoc,loc);
	map=updateScreen(fdout,map);
	freeGraph(g);
	g=createGraph();
	fillGraph(g,map);
	//printGraph(g);
	loc=findSelf(map);
	print(loc);
	nearestObject(map,g,loc);
	itemLoc=nearestItem(map,g,loc);
	enemyLoc=nearestEnemy(map,g,loc);

*/
print(doorLoc);
	if(checkMore(map)){
		space(fdin);
	}else{
		//moveTowards(fdin,enemyLoc,loc);
		//moveTowards(fdin,doorLoc,loc);
		moveTowards(fdin,itemLoc,loc);
	}

	map=updateScreen(fdout,map);
	freeGraph(g);
	g=createGraph();
	fillGraph(g,map);
//	printGraph(g);
	loc=findSelf(map);
	print(loc);
	nearestObject(map,g,loc);
	itemLoc=nearestItem(map,g,loc);
	enemyLoc=nearestEnemy(map,g,loc);

//	quit(fdin);

	return 0;
}
