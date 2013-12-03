#include "display.h"
using namespace std;


bool posn::operator==(posn other)
{
	if (row == other.row && col == other.col)
		return true;
	return false;
}

posn posn::dirAdjacent(int dir)
{
	posn p = nullPosn;
	if (dir == NORTHWEST)
		p.row = row - 1, p.col = col - 1;
	else if (dir == NORTH)
		p.row = row - 1, p.col = col;
	else if (dir == NORTHEAST)
		p.row = row - 1, p.col = col + 1;
	else if (dir == WEST)
		p.row = row, p.col = col - 1;
	else if (dir == EAST)
		p.row = row, p.col = col + 1;
	else if (dir == SOUTHWEST)
		p.row = row + 1, p.col = col - 1;
	else if (dir == SOUTH)
		p.row = row + 1, p.col = col;
	else if (dir == SOUTHEAST)
		p.row = row + 1, p.col = col + 1;
	if (p == nullPosn)
		cerr << "ERROR: Cannot find adjacent position, invalid direction given" << endl;
	return p;
}
int posn::findDir(posn other)
{
	if (*this == other)
	{
		cerr << "ERROR: Cannot determine direction, position is same as origin" << endl;
		return nullDir;
	}
	if (other.col > col && other.row > row)
		return SOUTHEAST;
	else if (other.col > col && other.row == row)
		return EAST;
	else if (other.col > col && other.row < row)
		return NORTHEAST;
	else if (other.col == col && other.row > row)
		return SOUTH;
	else if (other.col == col && other.row < row)
		return NORTH;
	else if (other.col < col && other.row > row)
		return SOUTHWEST;
	else if (other.col < col && other.row == row)
		return WEST;
	return NORTHWEST;
}

void Display::flood(char floodChar, int r, int c, int chamberNum)
{
	// Changes the character at the current coordinates
	posn temp = {r, c};
	chambers[chamberNum].tiles.push_back(temp);
	display[r][c] = floodChar;
	
	// Tries to change the characters directly to the north, south, east and west
	if (r > 0 && display[r-1][c]==FLOOR)
		flood(floodChar, r-1, c, chamberNum);
	if (r < (HEIGHT - 1) && display[r+1][c]==FLOOR)
		flood(floodChar, r+1, c, chamberNum);
	if (c > 0 && display[r][c-1]==FLOOR)
		flood(floodChar, r, c-1, chamberNum);
	if (c < (WIDTH - 1) && display[r][c+1]==FLOOR)
		flood(floodChar, r, c+1, chamberNum);
}

Display::Display()
{
	// Initializes display (to default grid)
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
			display[i][j] = defaultFloor[i][j];
	}
	// Initializes chambers (to empty vectors)
	for (int i = 0; i < numChambers; i++)
		chambers[i].tiles = vector<posn>();
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
			if (display[i][j] >= '0' && display[i][j] <= '5') // Fix potion characters
				display[i][j] = POTION;
			else if (display[i][j] >= '6' && display[i][j] <= '9') // Fix gold characters
				display[i][j] = GOLD;
			else if (display[i][j] == 'E')
				display[i][j] = ENEMY; // 'E' is a temporary assignment 
		}							   // for satisfied Dragons.
	}
}
void Display::floodGrid ()
{
	char floodChar = 'a';
	int chamberNum = floodChar - 'a';
	for (int r = 0; r < HEIGHT; r++)
	{
		for (int c = 0; c < WIDTH; c++)
		{
			// If algorithm finds an unmarked floor tile, mark it (using flood)
			// Since flood() is recursive, this will only happen for separated tiles
			// Therefore due to rules about number of chambers per map,
			// this happens exactly five times.
			if (display[r][c] == FLOOR)
			{
				flood(floodChar, r,c, chamberNum);
				floodChar++;
				chamberNum++;
			}
		}
	}
}
void Display::unfloodGrid()
{
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{								 // If the tile used to be floor, make it floor
			if (display[i][j] >= 'a' && display[i][j] <= 'e')
				display[i][j] = FLOOR;
		}
	}
}
int Display::getChamberSize(int c)
{
	return chambers[c].tiles.size();
}
posn Display::getTile(int c, int ind)
{
	return chambers[c].tiles[ind];
}
void Display::occupyTile(int c, int ind)
{
	chambers[c].tiles.erase(chambers[c].tiles.begin() + ind);
}
void Display::emptyChambers()
{
	for (int i = 0; i < numChambers; i++)
		chambers[i].tiles.clear();
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
