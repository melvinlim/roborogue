#ifndef DECODE_H
#define DECODE_H

#define DEBUG
//#define RAWDATA
#define NUMBEREDMAP

#define ESC struct esc

struct esc{
	char type;
	int x;
	int y;
	char *p;
};

//ESC *handleESC(char *p);
//char *printScreen(int fdout);
//char *updateScreen(int fdout,char *map);

#endif
