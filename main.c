#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>

#include<sight.h>
#include<decode.h>
#include<graph.h>

#define DEBUG

#define UP 'A'
#define DOWN 'B'
#define RIGHT 'C'
#define LEFT 'D'

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

int main(int argc,char *argv[]){
	int fdin,fdout,i,j,n;
	char *map;
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
	map=printScreen(fdout);
//	printf("%d %d\n",fdin,fdout);
//	quit(fdin);
	if(map==0){
		printf("error\n");
		return 0;
	}
	POINT *self=findSelf(map);
	print(self);
	GRAPH *g=createGraph();
	fillGraph(g,map);
	return 0;
}
