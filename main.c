#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>

#include<sys/resource.h>

#include<sight.h>
#include<decode.h>
#include<graph.h>

#define DEBUG

#define UP 'A'
#define DOWN 'B'
#define RIGHT 'C'
#define LEFT 'D'

void space(int fdin){
	char buf;
	int n;
	buf=' ';
	n=write(fdin,&buf,1);
	printf("wrote %d bytes\n",n);
}
void move(int fdin,char dir){
	char buf[3];
	int n;
	buf[0]=27;	//ESC
	buf[1]='[';
	buf[2]=dir;
	n=write(fdin,buf,3);
	printf("wrote %d bytes\n",n);
}
void quit(int fdin){
	int n;
	char buf[5];
	strcpy(buf,"Qy\n  ");
	n=write(fdin,buf,5);
	printf("wrote %d bytes\n",n);
}
void moveTowards(int fdin,POINT *dst,POINT *src){
	int dx,dy;
	if((src==0)||(dst==0))	return;
	dx = abs(src->x - dst->x);
	dy = abs(src->y - dst->y);
	if(dx>=dy){
		if(src->x > dst->x){
			move(fdin,LEFT);
		}else{
			move(fdin,RIGHT);
		}
	}else{
		if(src->y > dst->y){
			move(fdin,UP);
		}else{
			move(fdin,DOWN);
		}
	}
}
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
	POINT *enemyLoc,*itemLoc;
	print(loc);
	GRAPH *g=createGraph();
	fillGraph(g,map);
//	printGraph(g);
	nearestObject(map,g,loc);
	itemLoc=nearestItem(map,g,loc);
	enemyLoc=nearestEnemy(map,g,loc);
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

	if(checkMore(map)){
		space(fdin);
	}else{
printf("enemyLoc:");
print(enemyLoc);
		moveTowards(fdin,enemyLoc,loc);
	}

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

	return 0;
}
