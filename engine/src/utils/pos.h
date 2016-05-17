#ifndef WPOS_H
#define WPOS_H
//===================================
//Name: WLib (pos.h)
//Desc: Coordination system
//Author: Dominik 'ZaKlaus' Madarasz
//Version: 0.1a
//License: GNU/GPLv3
//===================================
//For educational purposes



//#include <windows.h>
/*
void GotoXY(int x, int y){
	COORD pos = {x, y};
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(output, pos);
}

COORD GetXY() {
	COORD pos;
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hstdout, &csbi);

	pos.X = csbi.dwCursorPosition.X;
	pos.Y = csbi.dwCursorPosition.Y;

	return pos;
}
*/
#endif