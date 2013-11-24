#include "display.h"
using namespace std;

Display::Display()
{
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
			display[i][j] = defaultFloor[i][j];
	}
}
void Display::setChar(int row, int col, char c)
{
	display[row][col] = c;
}
char Display::getChar(int row, int col)
{
	return display[row][col];
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
	{
		for (int j = 0; j < WIDTH; j++)
			out << d.display[i][j];
		out << endl;
	}
	return out;
}
