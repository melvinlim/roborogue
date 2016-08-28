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
	return p;
}
LIST *addList(LIST *p,VERTEX *v){
	LIST *t=p;

	if(findListValue(p,v->val)){
		return p;
	}

	while(t->next)	t=t->next;
	t->next=malloc(sizeof(LIST));
	t=t->next;
	t->v=v;
	t->next=0;
	return t;
//	return p;
}
LIST *remList(LIST *root,LIST *target){
	LIST *t=root;
	while(t->next){
		if(t->next==target){
			t->next=target->next;
//			free(target->v);
			free(target);
			return root;
		}
		t=t->next;
	}
	return root;
}
LIST *findListValue(LIST *p,int val){
	LIST *t=p;
	while(t->next){
		t=t->next;
		if(t->v->val==val)	return t;
	}
	return 0;
}
LIST *findList(LIST *p,VERTEX *v){
	LIST *t=p;
	while(t->next){
		t=t->next;
		if(t->v==v)	return t;
	}
	return 0;
}
LIST *lastList(LIST *t){
	LIST *p=t;
	if(p==0)	return p;
	while(p->next){
		p=p->next;
	}
	return p;
}
void freeList(LIST *root){
	LIST *target;
//	if(root==0)	return;
	while(root->next){
		target=lastList(root);
		root=remList(root,target);
	}
	root->next=0;
//	free(root);
}
void printList(LIST *p){
	while(p->next){
		p=p->next;
		printf("[%d]->",p->v->val);
	}
	printf("0\n");
}
LIST *dequeue(LIST *p){
	LIST *t=p->next;
	if(t){
		p->next=t->next;
	}
	return t;
}
int emptyList(LIST *p){
	if(p->next)
		return 0;
	else
		return 1;
}
