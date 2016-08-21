#include<stdio.h>
#include<stdlib.h>
#include<list.h>

LIST *createList(){
	LIST *p=malloc(sizeof(LIST));
	if(p==0){
		printf("error, could not allocate memory in createList()\n");
		return p;
	}
	p->next=0;
	p->v=0;
//	p->visited=0;
	return p;
}
LIST *addList(LIST *p,VERTEX *v){
	LIST *t=p;
	while(t->next)	t=t->next;
	t->next=malloc(sizeof(LIST));
	t=t->next;
	t->v=v;
//	t->visited=0;
	t->next=0;
	return p;
}
LIST *remList(LIST *root,LIST *target){
	LIST *t=root;
	while(t->next){
		if(t->next==target){
			t->next=target->next;
			free(target);
			return root;
		}
		t=t->next;
	}
	return root;
}
LIST *findList(LIST *p,VERTEX *v){
	LIST *t=p;
	while(t->next){
		t=t->next;
		if(t->v==v)	return t;
	}
	return 0;
}
LIST *lastList(LIST *p){
	while(p->next){
		p=p->next;
	}
	return p;
}
void freeList(LIST *root){
	LIST *target;
	while(root->next){
		target=lastList(root);
		remList(root,target);
	}
	free(root);
}
void printList(LIST *p){
	while(p->next){
		p=p->next;
		printf("[%d]->",p->v->val);
	}
	printf("0\n");
}
