#include "object.h"
using namespace std;

// Object class
void Object::setLoc(posn p) // private
{
	loc = p;
}
Object::Object(char oC): objectChar(oC), loc(nullPosn) {}
char Object::getObjectChar()
{
	return objectChar;
}
posn Object::getLoc()
{
	return loc;
}
void Object::initLoc(posn p)
{
	if (loc.row == nullPosn.row && loc.col == nullPosn.col)
		setLoc(p);
}

// Subclasses
Player::Player(): Object(PLAYER), tileChar(FLOOR) {}
char Player::getTileChar()
{
	return tileChar;
}

Stairs::Stairs(): Object(STAIRS) {}

Potion::Potion(): Object(POTION) {}

Gold::Gold(): Object(GOLD){}

Enemy::Enemy(): Object(ENEMY) {}
void Enemy::move(int dir)
{
	posn current = getLoc();
	posn p;
	if (dir == NORTH)
		p.row = current.row - 1, p.col = current.col;
	else if (dir == NORTHWEST)
		p.row = current.row - 1, p.col = current.col - 1;
	else if (dir == NORTHEAST)
		p.row = current.row - 1, p.col = current.col + 1;
	else if (dir == WEST)
		p.row = current.row, p.col = current.col - 1;
	else if (dir == EAST)
		p.row = current.row, p.col = current.col + 1;
	else if (dir == SOUTHWEST)
		p.row = current.row + 1, p.col = current.col - 1;
	else if (dir == SOUTHEAST)
		p.row = current.row + 1, p.col = current.col + 1;
	else
		p.row = current.row + 1, p.col = current.col;
	setLoc(p);
}
