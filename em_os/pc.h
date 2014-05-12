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
#define COLOR_BLACK			0
#define COLOR_RED			1
#define COLOR_GREEN			2
#define COLOR_YELLOW		3
#define COLOR_BLUE			4
#define COLOR_MAGENTA		5
#define COLOR_CYAN			6
#define COLOR_WHITE			7


PC_EXT u32 rndnext;
PC_EXT struct event *disp_str_sem;
PC_EXT struct event *random_sem;


extern void pc_dispchar(u8 x, u8 y, u8 c, u8 fg, u8 bg);
extern void pc_dispstr(u8 x, u8 y, char *s, u8 fg, u8 bg);
extern u8 my_random(u8 n);
extern void put_dec(u8 x2);
extern void pc_dispclrstr();


#endif
