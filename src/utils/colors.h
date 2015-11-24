#ifndef WCOLORS_H
#define WCOLORS_H
//===================================
//Name: WLib (colors.h)
//Desc: Useful when using colors in console
//Author: Dominik 'ZaKlaus' Madarasz
//Version: 0.1a
//License: GNU/GPLv3
//===================================
//For educational purposes
#include <windows.h>


enum {
	black,
	dark_blue,
	dark_green,
	dark_cyan,
	dark_red,
	dark_magenta,
	dark_yellow,
	light_gray,
	dark_gray,
	light_blue,
	light_green,
	light_cyan,
	light_red,
	light_magenta,
	light_yellow,
	white
};

///FUNCTIONS

WORD concatcolors(WORD color1, WORD color2){
	WORD color3 = 0x01;
	color3 = color1;
	for(int i = 0; i < sizeof(color2);i++){
		color3 *= 10;
	}
	color3 += color2;
	return color3;
}

int textcolor(WORD color){
	HANDLE hstdout = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( hstdout, &csbi );
	SetConsoleTextAttribute( hstdout, color );
	return 0;
}

///MACROS
#define CPRINT(c1,c2,x) textcolor(c1); std::cout << x; textcolor(c2)

#endif