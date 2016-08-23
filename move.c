#include<move.h>
#include<definitions.h>

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
void moveTowards(int fdin,POINT *dst,POINT *src){
	int dx,dy;
	if((src==0)||(dst==0))	return;
	dx = abs(src->x - dst->x);
	dy = abs(src->y - dst->y);
	if(dx>=dy){
		if(src->x > dst->x){
			move(fdin,LEFT);
		}else{
			move(fdin,RIGHT);
		}
	}else{
		if(src->y > dst->y){
			move(fdin,UP);
		}else{
			move(fdin,DOWN);
		}
	}
}
