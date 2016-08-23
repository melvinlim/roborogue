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
	srand(time(0));
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
	fdin=open(buf,O_WRONLY|O_SYNC);
	if(fdin<0){
		printf("unable to open %s\n",buf);
		return 0;
	}
	printf("fdin=%d\n",fdin);
//	printf("%d %d\n",fdin,fdout);

	map=updateScreen(fdout,0);
	if(map==0){
		printf("error\n");
		return 0;
	}
	GRAPH *g=createGraph();
	fillGraph(g,map);
//	printGraph(g);

	OBJECTS *objs;
	if(checkMore(map)){
		space(fdin);
	}else{
		objs=scanArea(map,g);
		printObjs(objs);
		if(objs->enemy){
printf("moving to enemy\n");
			moveTowards(fdin,objs->enemy,objs->self);
		}else if(objs->item){
printf("moving to item\n");
			moveTowards(fdin,objs->item,objs->self);
		}else if(objs->door){
printf("moving to door\n");
			moveTowards(fdin,objs->door,objs->self);
		}
	}
	free(objs);

	map=updateScreen(fdout,map);
	freeGraph(g);
	g=createGraph();
	fillGraph(g,map);
//	printGraph(g);
	objs=scanArea(map,g);
	printObjs(objs);

//	quit(fdin);

	return 0;
}
