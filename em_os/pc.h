#ifndef _PC_H
#define _PC_H

#ifdef PC_GLOBAL
#define PC_EXT
#else
#define PC_EXT	extern
#endif


// attribute
#define RESET		0
#define BRIGHT		1
#define DIM		2
#define UNDERLINE	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8


// color
#define BLACK		0
#define RED			1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define WHITE		7


PC_EXT u32 rndnext;
PC_EXT struct event *disp_str_sem;
PC_EXT struct event *random_sem;


extern void pc_dispchar(u8 x, u8 y, u8 c, u8 fg, u8 bg);
extern void pc_dispstr(u8 x, u8 y, char *s, u8 fg, u8 bg);
extern u8 my_random(u8 n);
extern void put_dec(u8 x2);
extern void pc_dispclrstr();


#endif
