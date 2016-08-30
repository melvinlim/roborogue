#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<definitions.h>

void decision(OBJECTS *objs){
	POINT *prevLoc;
	int n,foodSlot,prev;
	int fdin=objs->fdin;
	STATE state=objs->state;

	if((state!=attacking)&&(state!=returningToPrevLoc)){
			//if(objs->enemy){
			if(objs->seenEnemies->next){
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
				state=attacking;
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
			prev=moveTowards(objs,objs->stairs);
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
			if(checkDescent(objs->map)){
				freeGraph(objs->graph);
				objs->graph=0;
				free(objs->stairs);
				objs->stairs=0;
				freeList(objs->visitedDoors);
				freeList(objs->visitedTunnels);
			}
			objs->state=idle;
		break;
		case idle:
			if((!objs->status&(HUNGRY|STARVING|WEAK))&&objs->hpratio<100){
				printf("resting\n");
				restOneTurn(objs);
			}else if(objs->seenItems->next){
				printf("moving to item\n");
//				moveTowards(objs,objs->item);
				moveToV(objs,objs->seenItems->next->v);
			//}else if(objs->door){
			}else if(objs->seenDoors->next){
				printf("moving to door\n");
//				prev=(moveTowards(objs,objs->door));
				prev=moveToV(objs,objs->seenDoors->next->v);
				if(prev){
					printf("in front of door.  should mark door on next step then travel through tunnel.\n");
//						printf("previous step: %c\n",prev);
					objs->state=atDoor;
				}
			//}else if(objs->stairs){
			}else if(objs->seenStairs->next){
				objs->state=movingToStairs;
			}
/*
printf("tunnel 93 in decision.c unimplemented\n");
while(1);
				LIST *tmpNode;
				VERTEX *tmpV;
				while(!emptyList(objs->deadEnds)){
					tmpNode=dequeue(objs->deadEnds);
					tmpV=findListValue(objs->visitedTunnels,tmpNode->v->val);
					remList(objs->visitedTunnels,tmpV);
				}
				objs->state=inTunnel;
				navTunnel(fdin,objs);
			}
*/
		break;
		case attacking:
			if(enemyDefeated(objs->map)){
				printf("enemy defeated\n");
				//printf("enemy defeated, returning to previous location.\n");
				remList(objs->seenEnemies,objs->seenEnemies->next);
				objs->state=returningToPrevLoc;
				objs->state=idle;
			//}else if(objs->enemy==0){
			}else if(objs->seenEnemies->next==0){
				objs->state=returningToPrevLoc;
				objs->state=idle;
			}else{
				printf("moving towards / attacking enemy\n");
//				moveTowards(objs,objs->enemy);
				moveToV(objs,objs->seenEnemies->next->v);
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
				moveTowards(objs,objs->item);
			}else if(objs->door){
				printf("moving to door\n");
				prev=(moveTowards(objs,objs->door));
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
			if(objs->seenTunnels->next)
				moveToV(objs,objs->seenTunnels->next->v);
			objs->state=inTunnel;
		break;
		case inTunnel:
			objs->searches=0;
			markTunnel(objs);
			if(objs->seenTunnels->next)
				moveToV(objs,objs->seenTunnels->next->v);
			else
				objs->state=idle;
			//navTunnel(fdin,objs);
		break;
		case searching:
			objs->searches++;
			if(objs->searches<=objs->maxSearches){
				printf("searching (%d/%d)\n",objs->searches,objs->maxSearches);
				search(fdin);
			}else if(!isInDeadEnd(objs)){
				objs->searches=0;
				printf("successful search\n");
				objs->state=idle;
			}else{
				objs->searches=0;
				printf("giving up search\n");
				markTunnel(objs);
				markDeadEnd(objs);
				objs->state=idle;
			}
				
/*
				printf("searching (%d/%d)\n",objs->searches,objs->maxSearches);
				search(fdin);
				objs->state=inTunnel;	//navigateTunnel will change state to exitedTunnel or searching based on result.
				navTunnel(fdin,objs);
			}else{
				objs->searches=0;
				printf("dead end.  need to assume tunnel was just entered in other direction\n");
				objs->state=idle;
				//prev=opposite(prev);
				//objs->state=inTunnel;
			}
*/
		break;
		case exitedTunnel:
			objs->searches=0;
			markDoor(objs);
			objs->state=idle;
		break;
	}
}
