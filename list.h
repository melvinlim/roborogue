#ifndef _LIST_H
#define _LIST_H

#define LIST struct list

LIST{
	LIST *next;
	int val;
	int visited;
};

LIST *createList();
LIST *addList(LIST *p,int val);
LIST *remList(LIST *root,LIST *target);
LIST *findList(LIST *p,int val);
LIST *lastList(LIST *p);
void freeList(LIST *root);

#endif
