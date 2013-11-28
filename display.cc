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
void Display::fixCharacters()
{
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (display[i][j] >= '0' && display[i][j] <= '5')
				display[i][j] = 'P';
			else if (display[i][j] >= '6' && display[i][j] <= '9')
				display[i][j] = 'G';
			else if (display[i][j] == 'E')
				display[i][j] = 'D'; // 'E' is a temporary assignment for satisfied Dragons
									 // it is possible because nothing in the game uses 'E'
		}
	}
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
