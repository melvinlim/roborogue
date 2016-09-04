#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>

#include<unistd.h>
#include<termios.h>

#include<sys/resource.h>

#include<sight.h>
#include<decode.h>
#include<graph.h>
#include<move.h>

char *cmd[] = {"./rogue", NULL};

int main(int argc,char *argv[]){
	int fdin,fdout,i,j,n;
	char *map;
	char buf[256];
	char ch;
	char foodSlot;
	OBJECTS *objs;

	srand(time(0));
/*
	if(argc<=2){
		printf("usage: %s [output file] [process number]\n",argv[0]);
		return 0;
	}
*/
	int tcattr;
	struct termios tos;
	tcgetattr(1,&tos);
//printf("%x %x %x %x\n",tos.c_iflag,tos.c_oflag,tos.c_cflag,tos.c_lflag);
//return 0;
    
	int writepipe[2],readpipe[2];
	pid_t pid;
	pipe(writepipe);
	pipe(readpipe);
	pid=fork();
	if (pid == 0){
		dup2(writepipe[0],0);		//writepipe is what parent writes to.  duplicating read end of writepipe to child stdin.
		dup2(readpipe[1],1);		//write end of readpipe to child stdout.
		close(writepipe[0]);
		close(readpipe[1]);
		execvp(cmd[0],cmd);
		printf("error, execvp failed\n");
		return(0);
	}
//	printf("pipes: %d %d %d %d\n",writepipe[0],writepipe[1],readpipe[0],readpipe[1]);
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
/*
	sprintf(buf,"%s",argv[1]);
	printf("target file: %s\n",buf);
	//fdout=open(buf,O_RDONLY);
*/
fdout=readpipe[0];
	if(fdout<0){
		printf("unable to open %s\n",buf);
		return 0;
	}
	printf("fdout=%d\n",fdout);
/*
	sprintf(buf,"/proc/%s/fd/0",argv[2]);
	printf("target process: %s\n",buf);
//	fdin=open(buf,O_WRONLY|O_SYNC);
*/
fdin=writepipe[1];
	if(fdin<0){
		printf("unable to open %s\n",buf);
		return 0;
	}
	printf("fdin=%d\n",fdin);
//	printf("%d %d\n",fdin,fdout);

	fcntl(fdin,F_SETFL,fcntl(fdin,F_GETFL)|O_NONBLOCK);
	fcntl(fdout,F_SETFL,fcntl(fdin,F_GETFL)|O_NONBLOCK);

	tos.c_oflag=tos.c_oflag|ONLCR;	
	tcsetattr(1,TCSANOW,&tos);

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
sync();

		objs=scanArea(objs);

		if(objs){

			printMap(objs->map);
			printInfo(objs);
			//printObjs(objs);
			updateState(objs);
			decision(objs);

		}
//		sleep(1);
		getchar();
		clearScreen();
sync();
	}

	quit(fdin);

	return 0;
}
