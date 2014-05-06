

#define PC_GLOBAL

#include "emos.h"



void put_char(u8 c)
{
	write(1, &c, 1);
}


void put_dec(u8 x2)
{
	u8 x0;
	u8 x1;

	x0 = (x2 % 10);
	x2 /= 10;
	x1 = (x2 % 10);
	x2 /= 10;
	if (x2)
	{
		put_char(x2 + '0');
	}

	if (x1 || x2)
	{
		put_char(x1 + '0');
	}

	put_char(x0 + '0');
}


int pc_getkey(void *x)
{
	return 0;
}


void pc_attr(u8 fg, u8 bg)
{
	put_char(0x1b);
	put_char('[');
	put_dec(30 + fg);
	put_char(';');
	put_dec(40 + bg);
	put_char('m');
}


void put_string(char *s)
{
	while(*s) 
	{
		put_char(*s++);
	}
}


void textpos(u8 x, u8 y)
{
	put_char(0x1b);
	put_char('[');
	put_dec(y);
	put_char(';');
	put_dec(x);
	put_char('H');
}


void pc_dispchar(u8 x, u8 y, u8 c, u8 fg, u8 bg)
{
	down(disp_str_sem);

	pc_attr(fg, bg);
	textpos(x, y);
	put_char(c);

	up(disp_str_sem);
}


void pc_dispstr(u8 x, u8 y, char *s, u8 fg, u8 bg)
{
	down(disp_str_sem);

	pc_attr(fg, bg);
	textpos(x, y);
	put_string(s);

	up(disp_str_sem);
}


void pc_dispclrstr(void)
{
	pc_attr(WHITE, BLACK);
	put_string("\x1b[2J");
}


u8 my_random(u8 n)
{
	rndnext = rndnext * 1103515245 + 12345;
	return ((u8)((rndnext / 256) % (n + 1)));
}
