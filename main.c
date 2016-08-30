#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>

#include<sys/resource.h>

#include<sight.h>
#include<decode.h>
#include<graph.h>
#include<move.h>

int main(int argc,char *argv[]){
	int fdin,fdout,i,j,n;
	char *map;
	char buf[256];
	char ch;
	char foodSlot;
	OBJECTS *objs;

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

	objs=createObjects();
	objs->fdin=fdin;
	objs->fdout=fdout;
	objs->state=idle;
	
	int tmp=0;	
	int prev=0;

	POINT *prevLoc;
	int searches=0;

#ifdef SAVE
	save(fdin,"save\n\0");
	return 0;
#endif

	objs->maxSearches=5;
		
	objs=scanArea(objs);
	//printMap(objs->map);

//	objs->state=movingToStairs;
	while(1){

		objs=scanArea(objs);

		if(objs){

			printMap(objs->map);
			//printObjs(objs);
			updateState(objs);
			decision(objs);

		}
//		sleep(1);
		getchar();
		clearScreen();
	}

	quit(fdin);

	return 0;
}
