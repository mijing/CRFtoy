#ifdef WINDOWS
#include<windows.h>
#endif
#include<string>
#include<assert.h>
#include<iostream>
#include"print.hpp"
void PrintError(std::string str)
{
#ifdef WINDOWS
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO   csbi;

	assert(GetConsoleScreenBufferInfo(hConsole, &csbi));
	SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED);
#endif
	std::cout << str << std::endl;
#ifdef WINDOWS
	SetConsoleTextAttribute(hConsole, csbi.wAttributes);
#endif
}
