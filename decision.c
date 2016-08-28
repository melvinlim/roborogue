#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<definitions.h>

void decision(OBJECTS *objs){
	POINT *prevLoc;
	int n,foodSlot,searches,prev;
	int fdin=objs->fdin;
	STATE state=objs->state;

	if((state!=attacking)&&(state!=returningToPrevLoc)){
			if(objs->enemy){
				printf("clearing message (first) line in case of prior enemy defeated message\n");
				memset(objs->map,' ',80);
				printf("transitioning to attack state\n");
				prevLoc=NEW(POINT);
				memcpy(prevLoc,objs->self,sizeof(POINT));
				free(objs->prevLoc);
				objs->prevLoc=prevLoc;
				objs->prevStep=prev;
				objs->prevState=state;
				objs->state=attacking;
			}
	}

	if(objs->status&(HUNGRY|STARVING|WEAK)){
		printf("trying to eat\n");
		tryToEat(objs);
/*
		checkInventory(objs);
		foodSlot=findFood(objs);
		if(foodSlot){
			consume(objs,foodSlot);
			objs->state=idle;
		}else{
			objs->state=searchingForFood;
		}
*/
	}

	switch(state){
		case movingToStairs:
			printf("moving to stairs\n");
			nearestStairs(objs);
			if(objs->stairs==0){
				printf("error: objs->stairs should not be 0 here.\n");
			}
			prev=moveTowards(fdin,objs,objs->stairs);
			if(prev){
				printf("in front of stairs.  should erase graph and travel down stairs on next step.\n");
				objs->state=atStairs;
			}
		break;
		case atStairs:
			n=descend(fdin);
			if(n!=1){
				printf("failed to descend stairs\n");
				getchar();
			}
//getchar();
			freeGraph(objs->graph);
			free(objs->stairs);
			freeList(objs->visitedDoors);
			freeList(objs->visitedTunnels);
			objs->graph=0;
			objs->state=idle;
		break;
		case idle:
			if(objs->item){
				printf("moving to item\n");
				moveTowards(fdin,objs,objs->item);
			}else if(objs->door){
				printf("moving to door\n");
				prev=(moveTowards(fdin,objs,objs->door));
				if(prev){
					printf("in front of door.  should mark door on next step then travel through tunnel.\n");
//						printf("previous step: %c\n",prev);
					objs->state=atDoor;
				}
			}else{
				objs->state=movingToStairs;
			}
		break;
		case attacking:
			if(enemyDefeated(objs->map)){
				printf("enemy defeated\n");
				//printf("enemy defeated, returning to previous location.\n");
				objs->state=returningToPrevLoc;
				objs->state=idle;
			}else if(objs->enemy==0){
				objs->state=returningToPrevLoc;
				objs->state=idle;
			}else{
				printf("moving towards / attacking enemy\n");
				moveTowards(fdin,objs,objs->enemy);
			}
		break;
		case starving:
			checkInventory(objs);
			foodSlot=findFood(objs);
			if(foodSlot){
				consume(objs,foodSlot);
				objs->state=idle;
			}else{
				objs->state=searchingForFood;
			}
		break;
		case searchingForFood:

			checkInventory(objs);
			foodSlot=findFood(objs);
			if(foodSlot){
				consume(objs,foodSlot);
				objs->state=idle;
			}

			if(objs->stairs){
				objs->state=movingToStairs;
			}
			if(objs->item){
				printf("moving to item\n");
				moveTowards(fdin,objs,objs->item);
			}else if(objs->door){
				printf("moving to door\n");
				prev=(moveTowards(fdin,objs,objs->door));
				if(prev){
					printf("in front of door.  should mark door on next step then travel through tunnel.\n");
//						printf("previous step: %c\n",prev);
					objs->state=atDoor;
				}
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
//				markDoor(objs);
			objs->state=inTunnel;
		break;
		case inTunnel:
			searches=0;
			markTunnel(objs);
			navTunnel(fdin,objs);
//				prev=navigateTunnel(fdin,objs,prev);
		break;
		case searching:
			searches++;
			if(searches<=objs->maxSearches){
				printf("searching (%d/%d)\n",searches,objs->maxSearches);
				search(fdin);
			}else{
				searches=0;
				printf("giving up search\n");
				markTunnel(objs);
				markDeadEnd(objs);
				objs->state=idle;
			}
			if(!isInDeadEnd(objs)){
				searches=0;
				printf("successful search\n");
				objs->state=inTunnel;
			}
				
/*
				printf("searching (%d/%d)\n",searches,objs->maxSearches);
				search(fdin);
				objs->state=inTunnel;	//navigateTunnel will change state to exitedTunnel or searching based on result.
				navTunnel(fdin,objs);
				//prev=navigateTunnel(fdin,objs,prev);
			}else{
				searches=0;
				printf("dead end.  need to assume tunnel was just entered in other direction\n");
				objs->state=idle;
				//prev=opposite(prev);
				//objs->state=inTunnel;
			}
*/
		break;
		case exitedTunnel:
			searches=0;
			markDoor(objs);
			objs->state=idle;
		break;
	}
	sleep(1);
}
