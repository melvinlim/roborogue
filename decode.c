#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<decode.h>
#include<definitions.h>

void printMap(char *screen){
	int i,j;
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
		}
		putchar('\n');
	}
}

void clearScreen(){
	char clearStr[4];
	clearStr[0]=0x1b;
	clearStr[1]='[';
	clearStr[2]='H';
	clearStr[3]=0;
	printf("%s",clearStr);
	clearStr[0]=0x1b;
	clearStr[1]='[';
	clearStr[2]='J';
	clearStr[3]=0;
	printf("%s",clearStr);
}

void parseInventory(OBJECTS *objs){
	char ch;
	int fdout=objs->fdout;
	char *inventory=objs->inventory;
	char *p,*pEnd;
	int n,i,j,a,b;
	char buffer[BUFSZ+1];
//	int offset=objs->offset;
	int offset=0;
	if(inventory==0){
		inventory=malloc(ROWS*COLS);
		bzero(inventory,ROWS*COLS);
		memset(inventory,(char)' ',ROWS*COLS);
	}
	buffer[BUFSZ]=0;
	int pBLen=strlen(objs->prevBuffer);
	strncpy(buffer,objs->prevBuffer,pBLen);
	n=read(fdout,buffer+pBLen,BUFSZ-pBLen);
	bzero(objs->prevBuffer,pBLen);
	//n=read(fdout,buffer,BUFSZ);
	printf("read %d bytes\n",n);
	p=buffer;
	//if(n>=BUFSZ/2){
	if(n+pBLen>=BUFSZ/2){
		pEnd=memchr( (p+BUFSZ/2) , 0x1b , BUFSZ/2 );
		if(pEnd==0){
			printf("ERROR: could not find 0x1b.\n");
			return;
		}
	}else{
		pEnd=buffer+n+pBLen;
	}

//#ifdef RAWDATA
	i=0;
	while(p<pEnd){
		if(iscntrl(*p)){
			if(i++ > 40){
				i=0;
				getchar();
			}
		printf("\n[0x%02x]",*p);
		}else{
		printf("%c",*p);
		}
		p++;
	}
	if(n>=BUFSZ){
		parseInventory(objs);
	}
//#endif
	putchar('\n');

	p=buffer;
	while(p<pEnd){
		while(!isalpha(*p)){
			p++;
		}
		p++;
		while(!iscntrl(*p)){
			inventory[offset++]=*p;
			p++;
		}
		offset = (offset/80)*80+80;
	}

	objs->inventory=inventory;
	
	for(i=0;i<ROWS;i++){
		printf("line %02i:\t",i);
		for(j=0;j<80;j++){
			printf("%c",inventory[i*80+j]);
		}
		putchar('\n');
	}

}

void updateScreen(OBJECTS *objs){
	int ti,tj;
	char ch;
	int fdout=objs->fdout;
	char *screen=objs->map;
	char *p,*pEnd;
	int n,i,j,a,b;
	char buffer[BUFSZ+1];
	int offset=objs->offset;
	if(screen==0){
		screen=malloc(ROWS*COLS);
		bzero(screen,ROWS*COLS);
		memset(screen,(char)' ',ROWS*COLS);
	}
	buffer[BUFSZ]=0;
	int pBLen=strlen(objs->prevBuffer);
strncpy(buffer,objs->prevBuffer,pBLen);
	n=read(fdout,buffer+pBLen,BUFSZ-pBLen);
bzero(objs->prevBuffer,pBLen);
	//n=read(fdout,buffer,BUFSZ);
	printf("read %d bytes\n",n);
	p=buffer;
	//if(n>=BUFSZ/2){
	if(n+pBLen>=BUFSZ/2){
		pEnd=memchr( (p+BUFSZ/2) , 0x1b , BUFSZ/2 );
		if(pEnd==0){
			printf("ERROR: could not find 0x1b.\n");
			return;
		}
	}else{
		pEnd=buffer+n+pBLen;
	}
#ifdef RAWDATA
i=0;
while(p<pEnd){
if(iscntrl(*p)){
if(i++ > 40){
	i=0;
	//printMap(screen);
	getchar();
}
printf("\n[0x%02x]",*p);
}else{
printf("%c",*p);
}
p++;
}
if(n>=BUFSZ){
	updateScreen(objs);
}
return;
#endif
	while(p<pEnd){
		ch=*p;
if(iscntrl(ch)){
printf("\n[0x%02x]",ch);
}else{
printf("%c",ch);
}
		switch(ch){
			case 0x1b:
				p++;
				ch=*p;
				if(ch!='['){
					switch(ch){
						case'M':	//scroll screen up
							for(ti=objs->scrollBottom;ti>objs->scrollTop;ti--){
								for(tj=0;tj<COLS;tj++){
									screen[(ti)*COLS+tj]=screen[(ti-1)*COLS+tj];
								}
							}
/*
objs->offset+=80;
printf("[0x1b]M\n");
getchar();
*/
						break;
						case'7':	//save cursor position
							objs->savedPosition=objs->offset;
						break;
						case'8':	//restore cursor position
							objs->offset=objs->savedPosition;
						break;
						default:
							printf("error, [0x1b][0x%x]\n",ch);
							getchar();
					}
					break;
				}
				putchar(ch);
				p++;
				a=0;
if(!isdigit(*p)){
	printf("not a digit: %c / %d\n",*p,*p);
}
				while(isdigit(*p)){
					a*=10;
					a+=*p-48;
					p++;
				}
				a--;
printf("%d,",a);
				ch=*p;
				if(ch!=';'){
printf("%c,",ch);
					switch(ch){
						case 24:
							printf("CANCEL\n");
						break;
						case'A':									//move cursor vertically up/down -/+ a units.
							offset+=80*(a);
						break;
						case'd':									//move cursor down to line number a.
							offset=offset%80;
							offset+=80*(a);
						break;
						case'G':									//move cursor right to column a.
							offset += a - (offset%80);
						break;
						case'H':									//cursor home set to: [1,1] and? status line cleared?
//							memset(screen,' ',80);
							offset=0;
						break;
						case'K':									//status line cleared?
//							memset(screen,' ',80);
//							memset(screen+(offset-offset%80),' ',80);
							if(a>=0){
								memset(screen+(offset)-(offset%80)+a,' ',80-a);
							}else{
								memset(screen+(offset),' ',80-(offset%80));
							}
						break;
						case'X':									//clear a characters?
							memset(screen+offset,' ',a+1);
						break;
						case'J':									//clear entire screen?
							memset(screen,' ',ROWS*COLS);
						break;
					}
				}else{
					p++;
					b=0;
					while(isdigit(*p)){
						b*=10;
						b+=*p-48;
						p++;
					}
					b--;
printf("%d,",b);
					ch=*p;
printf("%c,",ch);
					switch(ch){
						case'r':		//scrolling enabled from rows [a,b]
							objs->scrollTop=a;
							objs->scrollBottom=b;
						break;
						case'R':		//cursor was reported at [a,b]
						break;
						case'H':		//set cursor home to [a,b]
							offset=80*(a);
							offset+=b;
						break;
						case'f':		//cursor forced to [a,b]
						break;
					}
				}
			break;
			case 0x08:		//ASCII backspace:				\b
				offset--;
			break;
			case 0x0d:		//ASCII carriage return:	\r
				offset = offset-(offset%80);
			break;
/*
			case ' ':			//ASCII space
				//offset++;
				screen[offset++]=' ';
			break;
*/
			case 0x0a:
				for(ti=objs->scrollTop;ti<objs->scrollBottom;ti++){
					for(tj=0;tj<COLS;tj++){
						screen[ti*COLS+tj]=screen[(ti+1)*COLS+tj];
					}
				}
//			offset++;
			break;
			default:
if(iscntrl(ch)){
printf("error: control character which is not ESC\n");
printf("[%d]\n",ch);
getchar();
}
				screen[offset++]=ch;
		}
		p++;
	}
//	printf("\27[5B");	//move down 5 lines.

//	printMap(screen);
//	printf("n=%d\n",n);
	objs->offset=offset;
	objs->map=screen;
	if(pEnd < buffer+n+pBLen){		//have to make sure to finish reading remnants of buffer.
		printf("buffer getting full, copying remainder (end pointer -> end of buffer) to special buffer and calling updateScreen again\n");
		printf("%d\n",n+pBLen-(pEnd-buffer));
		bzero(objs->prevBuffer,BUFSZ);
		memcpy(objs->prevBuffer,pEnd,n+pBLen-(pEnd-buffer));
		updateScreen(objs);
	}
}
