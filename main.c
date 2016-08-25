#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>

#include<sys/resource.h>

#include<sight.h>
#include<decode.h>
#include<graph.h>
#include<move.h>

#define MAXSEARCHES 50

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
	objs->fdin=fdin;
	objs->fdout=fdout;
	objs->state=idle;
	
	int tmp=0;	
	int prev=0;
/*
		if(isInTunnel(objs)){
			printf("possibly in tunnel\n");
			objs->state=inTunnel;
		}
*/
	POINT *prevLoc;
	int searches=0;

//	save(fdin,"save\n\0");
//	return 0;

	while(1){

		objs=scanArea(objs);
		printObjs(objs);
//return 0;

		if(checkFaint(objs->map)){
			printf("dying of starvation\n");
			objs->state=starving;
		}

		if(checkMore(objs->map)){
			printf("cleared more prompt\n");
			space(fdin);
		}

		if(objs->state!=attacking){
				if(objs->enemy){
					printf("transitioning to attack state\n");
					prevLoc=NEW(POINT);
					memcpy(prevLoc,objs->self,sizeof(POINT));
					free(objs->prevLoc);
					objs->prevLoc=prevLoc;
					objs->prevStep=prev;
					objs->prevState=objs->state;
					objs->state=attacking;
				}
		}

		switch(objs->state){
			case idle:
				if(objs->item){
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
			case starving:
				checkInventory(objs);
				char foodSlot=findFood(objs);
				if(foodSlot){
					consume(objs,foodSlot);
				}
				objs->state=idle;
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
				searches=0;
				prev=navigateTunnel(fdin,objs,prev);
			break;
			case searching:
				searches++;
				if(searches<=MAXSEARCHES){
					printf("searching (%d/%d)\n",searches,MAXSEARCHES);
					search(fdin);
					objs->state=inTunnel;	//navigateTunnel will change state to exitedTunnel or searching based on result.
					prev=navigateTunnel(fdin,objs,prev);
				}else{
					searches=0;
					printf("dead end.  need to assume tunnel was just entered in other direction\n");
					prev=opposite(prev);
					objs->state=inTunnel;
				}
			break;
			case exitedTunnel:
				searches=0;
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
