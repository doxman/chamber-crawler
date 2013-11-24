#include "display.h"
using namespace std;

Display::Display()
{
	display = new char* [HEIGHT];
	for (int i = 0; i < HEIGHT; i++)
	{
		display[i] = new char [WIDTH];
		for (int j = 0; j < WIDTH; j++)
			display[i][j] = defaultFloor[i][j];
	}
}
Display::~Display()
{
	for (int i = 0; i < HEIGHT; i++)
		delete[] display[i];
}
char** Display::getDisplay()
{
	return display;
}
void Display::setChar(int row, int col, char c)
{
	display[row][col] = c;
}
char* Display::getChar(int row, int col)
{
	return &display[row][col];
}
istream& operator>>(istream &in, Display &d)
{
	string temp;
	for (int i = 0; i < HEIGHT; i++)
	{
		getline(in, temp);
		for (int j = 0; j < WIDTH; j++)
			d.display[i][j] = temp[j];
	}
	return in;
}
ostream& operator<<(ostream &out, const Display &d)
{
	for (int i = 0; i < HEIGHT; i++)
		out << d.display[i] << endl;
	return out;
}
