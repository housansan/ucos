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


#endif
