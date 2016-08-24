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
	objs->fd=fdout;
	objs->state=idle;
	
	int tmp=0;	
	int prev=0;
/*
		if(isInTunnel(objs)){
			printf("possibly in tunnel\n");
			objs->state=inTunnel;
		}
*/
	while(1){

		objs=scanArea(objs);
		printObjs(objs);

		if(checkMore(objs->map)){
			printf("cleared more prompt\n");
			space(fdin);
		}
		
		switch(objs->state){
			case idle:
				if(objs->enemy){
					printf("transitioning to attack state\n");
					POINT *prevLoc=NEW(POINT);
					memcpy(prevLoc,objs->self,sizeof(POINT));
					objs->prevLoc=prevLoc;
					objs->prevStep=prev;
					objs->prevState=objs->state;
					objs->state=attacking;
				}else if(objs->item){
					printf("moving to item\n");
					moveTowards(fdin,objs,objs->item);
				}else if(objs->door){
					printf("moving to door\n");
					prev=(moveTowards(fdin,objs,objs->door));
					if(prev){
						printf("in front of door.  should mark door on next step then travel through tunnel.\n");
						printf("previous step: %c\n",prev);
						objs->state=atDoor;
					}
				}
			break;
			case attacking:
				if(enemyDefeated(objs->map)){
					printf("enemy defeated, returning to previous location.\n");
					objs->state=returningToPrevLoc;
				}else{
					printf("moving towards / attacking enemy\n");
					moveTowards(fdin,objs,objs->enemy);
				}
			break;
			case returningToPrevLoc:
printf("prev loc:");
print(objs->prevLoc);
				if(moveToPoint(fdin,objs,objs->prevLoc)){
printf("restoring old state\n");
					objs->state=objs->prevState;
					objs->nextStep=objs->prevStep;
				}
			break;
			case atDoor:
				markDoor(objs);
				objs->state=inTunnel;
			break;
			case inTunnel:
				prev=navigateTunnel(fdin,objs,prev);
				if(prev==0){
					printf("exiting tunnel.  should mark exit as visited at next step.\n");
					objs->state=exitedTunnel;
				}else if(prev==-1){
					printf("dead end.  searching to try and find door\n");
					for(i=0;i<15;i++){
						search(fdin);
					}
					printf("dead end.  need to assume tunnel was just entered in other direction\n");
					prev=opposite(prev);
				}
			break;
			case exitedTunnel:
				markDoor(objs);
				objs->state=idle;
			break;
		}
//return 0;
		sleep(2);
	}

	quit(fdin);

	return 0;
}
