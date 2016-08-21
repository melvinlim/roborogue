#ifndef _LIST_H
#define _LIST_H

#include<definitions.h>

#define LIST struct list

LIST{
	LIST *next;
	VERTEX *v;
};

LIST *createList();
LIST *addList(LIST *p,VERTEX *v);
LIST *remList(LIST *root,LIST *target);
LIST *findList(LIST *p,VERTEX *v);
LIST *lastList(LIST *p);
void freeList(LIST *root);

#endif
