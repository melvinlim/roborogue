#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<decode.h>
#include<definitions.h>

ESC *handleESC(char *p){
	int a,b;
	int t,u;
	char ch;
	p++;
	if(*p!='['){
#ifdef DEBUG
		printf("\nESC %c detected\n",*(p+1));
#endif
		return 0;
	}
	ESC *esc=malloc(sizeof(ESC));
	p++;
	a=0;
	while(isdigit(*p)){
		a*=10;
		a+=*p-48;
		p++;
	}
	a--;
	if(*p!=';'){
		switch(*p){
			case 24:
				printf("CANCEL\n");
				esc=malloc(sizeof(ESC));
				esc->p=p;
				esc->type='u';
				return esc;
			break;
			case'A':									//move cursor vertically up/down -/+ a units.
				esc=malloc(sizeof(ESC));
				esc->y=a;
				esc->p=p;
				esc->type='A';
				return esc;
			break;
			case'd':									//move cursor down to line number a.
				esc=malloc(sizeof(ESC));
				esc->y=a;
				esc->p=p;
				esc->type='d';
				return esc;
			break;
			case'G':									//move cursor right to column a.
				esc->type='G';
				esc->x=a;
				esc->p=p;
				return esc;
			break;
			case'H':
				esc->type='.';
				esc->p=p;
				return esc;
			break;
		}
#ifdef DEBUG
		printf("\na!=;,a=%d\n",a);
		printf("p!=\';\'\n");
#endif
		while(!isalpha(*p)){
#ifdef DEBUG
			printf("%c not alpha\n",*p);
#endif
			p++;
		}
#ifdef DEBUG
		printf("*p==%c\n",*p);
#endif
		esc->type='u';
		esc->p=p;
		return esc;
	}
	p++;
	b=0;
	while(isdigit(*p)){
		b*=10;
		b+=*p-48;
		p++;
	}
	b--;
	ch=*(p);
	//printf("c=%c\n",ch);
	switch(ch){
		case'r':		//scrolling enabled from rows [a,b]
			esc->type='r';
			esc->y=a;
			esc->x=b;
		break;
		case'R':		//cursor was reported at [a,b]
			esc->type='R';
			esc->y=a;
			esc->x=b;
		break;
		case'H':		//set cursor home to [a,b]
			esc->type='H';
			esc->y=a;
			esc->x=b;
		break;
		case'f':		//cursor forced to [a,b]
			esc->type='f';
			esc->y=a;
			esc->x=b;
		break;
	}
	esc->p=p;
	putchar('\n');
	return esc;
}

void scrollDown(char *map){
	int i,j;
	for(i=0;i<ROWS-1;i++){
		for(j=0;j<COLUMNS;j++){
			map[INDEX(i,j)]=map[INDEX(i+1,j)];
		}
	}
}
void updateScreen(OBJECTS *objs){
//	int fdout;
//	char *screen;
	int fdout=objs->fd;
	char *screen=objs->map;
	char *pLevel;
	char *pDots;
	ESC *esc;
	char *p,*pEnd;
	int n,i,j;
	char buffer[BUFSZ+1];
	//int offset=0;
	int offset=objs->offset;
/*
	if(objs){
	}else{
		fdout=0;
		screen=0;
	}
*/
	if(screen==0){
		screen=malloc(ROWS*COLUMNS);
		bzero(screen,ROWS*COLUMNS);
		memset(screen,(char)' ',ROWS*COLUMNS);
	}
	//lseek(fdout,0,SEEK_SET);	//alternatively store offset.
	buffer[BUFSZ]=0;
	n=read(fdout,buffer,BUFSZ);
	printf("read %d bytes\n",n);
	p=buffer;
	pEnd=buffer+n;
#ifdef RAWDATA
while(p<pEnd){
if(iscntrl(*p)){
printf("[0x%02x]",*p);
}else{
printf("%c",*p);
}
p++;
}
return;
//return 0;
#endif
	pLevel=strstr(buffer,"Level");
	pDots=strstr(buffer,"...");
//	p=pDots+3;
//	while(*p!=0){
	//while(p!=pLevel){
	while(p<pEnd){
		if(*p==27){
			esc=handleESC(p);
			if(esc){
				p=esc->p;
				switch(esc->type){
					case'A':									//move cursor vertically up/down -/+ a units.
						offset+=80*(esc->y);
#ifdef DECODE_DEBUG
						printf("\nmoved cursor to: [%d,%d]\n",offset/80,offset%80);
#endif
					break;
					case'G':									//move cursor right to column a.
#ifdef DECODE_DEBUG
						printf("\nmoved cursor to: [%d,%d]\n",offset/80,esc->x);
#endif
						offset += esc->x - (offset%80);
					break;
					case'R':
#ifdef DECODE_DEBUG
						printf("\ncursor reported at: [%d,%d]\n",esc->y,esc->x);
#endif
					break;
					case'f':
#ifdef DECODE_DEBUG
						printf("\ncursor forced to: [%d,%d]\n",esc->y,esc->x);
#endif
					break;
					case'd':
						offset=offset%80;
#ifdef DECODE_DEBUG
						printf("\nmoved cursor to: [%d,%d]\n",esc->y,offset);
#endif
						offset+=80*(esc->y);
					break;
					case'r':
#ifdef DECODE_DEBUG
						printf("\nscrolling enabled from rows: [%d,%d]\n",esc->y,esc->x);
#endif
					break;
					case'.':
						memset(screen,' ',80);
#ifdef DECODE_DEBUG
						printf("\ncursor home set to: [1,1] and status line cleared\n");
						//printf("\ncursor home set to: [%d,%d]\n",1,1);
#endif
						offset=0;
					break;
					case'H':
#ifdef DECODE_DEBUG
						printf("\ncursor home set to: [%d,%d]\n",esc->y,esc->x);
						printf("(n,m)=");
						printf("%d,",80*(esc->y));
						printf("%d\n",(esc->x));
#endif
						offset=esc->x;
						offset+=80*(esc->y);
					break;
				}
				free(esc);
			}else{
#ifdef DEBUG
printf("error in updateScreen (handleESC returned 0)\n");
//return 0;
#endif
			}
		}else if(*p==0x08){		//ASCII backspace:				\b
			offset--;
		}else if(*p==13){			//ASCII carriage return:	\r
			offset = offset-(offset%80);
		}else if(*p==' '){		//ASCII space
			offset++;
			printf(" ");
		}else	if(!iscntrl(*p)){
			screen[offset++]=*p;
#ifdef DECODE_DEBUG
			printf("%c",*p);
//		printf("(%d:%c) ",offset,*p);
#endif
		}else{
#ifdef DECODE_DEBUG
			printf("detected control character: %d\n",*p);
#endif
		}
		p++;
	}
	//while(p!=0){
/*
	while(p<=pEnd){
		if(*p==27){
			esc=handleESC(p);
			if(esc){
				p=esc->p;
			}
		}else{
			printf("%c",*p);
		}
		p++;
	}
*/
/*
	while(*p!=27){
		if(!iscntrl(*p))
			screen[offset++]=*p;
		p++;
	}
*/
//	printf("\27[5B");	//move down 5 lines.
	printf("\n");
	printf("\t\t");
	for(i=0;i<80;i++){
		printf("%d",i%10);
	}
	printf("\n");
	for(i=0;i<ROWS;i++){
		printf("line %02i:\t",i);
		for(j=0;j<80;j++){
			if(screen[i*80+j]==' '){
#ifdef NUMBEREDMAP
				printf("%d",j%10);
#else
				printf(" ");
#endif
			}else{
				printf("%c",screen[i*80+j]);
			}
			//printf("%c",buffer[i*80+j]);
		}
		putchar('\n');
	}
	printf("n=%d\n",n);
	objs->offset=offset;
	if(n>=BUFSZ){
		printf("buffer was full, calling updateScreen again\n");
		objs->map=screen;
		//screen=updateScreen(objs);
		updateScreen(objs);
	}
//	return screen;
}
