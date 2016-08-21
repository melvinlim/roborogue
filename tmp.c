#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>

#define DEBUG

#define UP 'A'
#define DOWN 'B'
#define RIGHT 'C'
#define LEFT 'D'

#define ESC struct esc

struct esc{
	char type;
	int x;
	int y;
	char *p;
};

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
	if(*p!=';'){
		printf("\na!=;,a=%d\n",a);
		if(*(p)==24){
printf("CANCEL\n");
			esc=malloc(sizeof(ESC));
			esc->p=p;
			esc->type='u';
			return esc;
		}
		if(*(p)=='d'){
			printf("*p==d\n");
			esc=malloc(sizeof(ESC));
			esc->x=a;
//			esc->x=80;
			esc->p=p;
			esc->type='d';
			return esc;
		}
#ifdef DEBUG
		printf("\np!=\';\'\n");
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
	ch=*(p);
	//printf("c=%c\n",ch);
	switch(ch){
		case'r':
#ifdef DEBUG
			printf("\nscrolling enabled from rows: [%d,%d]\n",a,b);
#endif
			esc->type='r';
			esc->y=a;
			esc->x=b;
		break;
		case'R':
#ifdef DEBUG
			printf("\ncursor reported at: [%d,%d]\n",a,b);
#endif
			esc->type='R';
			esc->y=a;
			esc->x=b;
		break;
		case'H':
#ifdef DEBUG
			printf("\ncursor home set to: [%d,%d]\n",a,b);
#endif
			esc->type='H';
			esc->y=a;
			esc->x=b;
		break;
		case'f':
#ifdef DEBUG
			printf("\ncursor forced to: [%d,%d]\n",a,b);
#endif
			esc->type='f';
			esc->y=a;
			esc->x=b;
		break;
	}
	esc->p=p;
putchar('\n');
	return esc;
}
void space(int fdin){
	char buf;
	int n;
	buf=' ';
	n=write(fdin,&buf,1);
	printf("wrote %d bytes\n",n);
}
void move(int fdin,char dir){
	char buf[3];
	int n;
	buf[0]=27;	//ESC
	buf[1]='[';
	buf[2]=dir;
	n=write(fdin,buf,3);
	printf("wrote %d bytes\n",n);
}
void quit(int fdin){
	int n;
	char buf[5];
	strcpy(buf,"Qy\n  ");
	n=write(fdin,buf,5);
	printf("wrote %d bytes\n",n);
}
void printScreen(int fdout){
	ESC *esc;
	char *p;
	int n,i,j,x0,y0;
	char output[24*80+1];
	char screen[24*80];
	int offset=0;
	bzero(screen,24*80);
	memset(screen,(char)' ',24*80);
	//memset(screen,32,24*80);
	n=read(fdout,output,24*80);
	printf("read %d bytes\n",n);
	p=output;
	output[24*80]=0;
	while(*p!=0){
		if(*p==27){
			esc=handleESC(p);
			if(esc){
				p=esc->p;
				switch(esc->type){
					case'd':
//						offset-=esc->x;
						offset+=80;
						offset-=offset%80;
					break;
					case'r':
					break;
					case'H':
#ifdef DEBUG
						printf("(n,m)=");
						printf("%d,",80*(esc->y-1));
						printf("%d\n",(esc->x-1));
#endif
						offset=esc->x - 1;
						offset+=80*(esc->y - 1);
					break;
				}
			}else{
#ifdef DEBUG
printf("error in printScreen\n");
return;
#endif
			}
		}else{
			if(!iscntrl(*p))
				screen[offset++]=*p;
//			printf("(%d:%c) ",offset,*p);
			printf("%c",*p);
		}
		p++;
	}
//	printf("\27[5B");	//move down 5 lines.
	for(i=0;i<24;i++){
		for(j=0;j<80;j++){
			printf("%c",screen[i*80+j]);
			//printf("%c",output[i*80+j]);
		}
		putchar('\n');
	}
	//printf("n=%d\n",n);
}
int main(int argc,char *argv[]){
	int fdin,fdout,i,j,n;
	char buf[256];
	char ch;
	if(argc<=2){
		printf("usage: %s [output file] [process number]\n",argv[0]);
		return 0;
	}
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
	fdin=open(buf,O_WRONLY);
	if(fdin<0){
		printf("unable to open %s\n",buf);
		return 0;
	}
	printf("fdin=%d\n",fdin);
//	space(fdin);
//	move(fdin,DOWN);
	printScreen(fdout);
//	printf("%d %d\n",fdin,fdout);
//	quit(fdin);
	return 0;
}
