#ifndef __OBJECT_H__
#define __OBJECT_H__

struct posn
{
	int row;
	int col;
};

const posn nullPosn = {-1, -1};

const char FLOOR = '.';

// Directions
const int numDirections = 8;

const int NORTH = 0;
const int NORTHWEST = 1;
const int NORTHEAST = 2;
const int WEST = 3;
const int EAST = 4;
const int SOUTHWEST = 5;
const int SOUTHEAST = 6;
const int SOUTH = 7;

// Object characters
const char PLAYER = '@';
const char STAIRS = '\\';
const char POTION = 'P';
const char GOLD = 'G';
const char ENEMY = 'E';

class Object
{
	char objectChar;
	posn loc;
protected:
	void setLoc(posn p);
public:
	Object(char oC);
	char getObjectChar();
	posn getLoc();
	void initLoc(posn p); // Only works when loc is the null position; used to initialize posn;
};

class Player:public Object
{
	char tileChar;
public:
	Player();
	char getTileChar();
};

class Stairs:public Object
{
public:
	Stairs();
};

class Potion:public Object
{
public:
	Potion();
};

class Gold:public Object
{
public:
	Gold();
};

class Enemy:public Object
{
public:
	Enemy();
	void move (int dir);
};

#endif
