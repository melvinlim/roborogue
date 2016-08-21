#include<stdio.h>
#include<fcntl.h>
#include<string.h>

#define UP 'A'
#define DOWN 'B'
#define RIGHT 'C'
#define LEFT 'D'

int handleESC(char *p){
	int a,b;
	int t,u;
	char *x;
	char ch;
	if(*(p+1)!='[')	return 0;
	//printf("p+1=%c",*(p+1));
	//printf("\nESC:");
	p+=2;
	t=*p-48;
	a=0;
	while(t>=0&&t<=9){
		a*=10;
		a+=t;
		p++;
		t=*p-48;
	}
	if(*p!=';')	return 0;
	p++;
	u=*p-48;
	b=0;
	while(u>=0&&u<=9){
		b*=10;
		b+=u;
		p++;
		u=*p-48;
	}
		ch=*(p);
		//printf("c=%c\n",ch);
		switch(ch){
			case'r':
			case'R':
				printf("\ncursor reported at: [%d,%d]\n",a,b);
			break;
			case'h':
			case'H':
				printf("\ncursor home set to: [%d,%d]\n",a,b);
			break;
			case'f':
			case'F':
				printf("\ncursor forced to: [%d,%d]\n",a,b);
			break;
		}
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
	char *p;
	int n,i,j;
	char output[24*80];
	n=read(fdout,output,24*80);
	printf("read %d bytes\n",n);
	for(i=0;i<n;i++){
		if(output[i]==27){
p=output+i;
handleESC(p);
			i++;
			if(output[i]=='['){
/*
				i++;
				printf("%d",output[i]);
				i++;
				switch(output[i]){
					case';':
						i++;
						printf("%d\n",output[i]);
						i++;
					break;
				}
*/
			}
		}
		printf("%c",output[i]);
	}
/*
	for(i=0;i<24;i++){
		for(j=0;i<80;j++){
			printf("%c",output[i*80+j]);
		}
	}
*/
/*
	for(i=0;i<512;i++){
		if(output[i]=='\n')	printf("i=%d\n",i);
		printf("%c",output[i]);
	}
*/
//	printf("\27[5B");	//move down 5 lines.
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
