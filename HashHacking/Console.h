#pragma once

#include <Windows.h>

void gotoxy(int column, int line)
{
	COORD coord;
	coord.X = column;
	coord.Y = line;
	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		coord);
}

int wherex()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD result;
	if (!GetConsoleScreenBufferInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&csbi)
		)
		return -1;
	//return result.X;
	return csbi.dwCursorPosition.X;
}

int wherey()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD result;
	if (!GetConsoleScreenBufferInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&csbi)
		)
		return -1;
	//return result.Y;
	return csbi.dwCursorPosition.Y;
}

void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}