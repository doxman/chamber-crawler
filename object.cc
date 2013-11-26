#include "object.h"
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sstream>
using namespace std;

// Equality test for races
bool operator==(race arg1, race arg2)
{
	if (arg1.hp == arg2.hp && arg1.atk == arg2.atk && arg1.def == arg2.def
		&& arg1.name == arg2.name)
		return true;
	return false;
}

// Object class
void Object::setObjectChar(char c) // protected
{
	objectChar = c;
}
void Object::setLoc(posn p) // protected
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

Stairs::Stairs(): Object(STAIRS) {}
void Stairs::die(){}

Potion::Potion(): Object(POTION)
{
	hpMod = 0;
	atkMod = 0;
	defMod = 0;
	type = "WATER";
}
void Potion::initPotion(race p)
{
	if (type == "WATER")
	{
		hpMod = p.hp;
		atkMod = p.atk;
		defMod = p.def;
		type = p.name;
	}
}
int Potion::getHP() {return hpMod;}
int Potion::getAtk() {return atkMod;}
int Potion::getDef() {return defMod;}
string Potion::getType() {return type;}
void Potion::die(){}

Gold::Gold(): Object(GOLD), value(0), guarded(false){}
void Gold::initValue(int v)
{
	value = v;
}

void Gold::setGuarded(bool guard)
{
	guarded = guard;
}
bool Gold::isGuarded()
{
	return guarded;
}
int Gold::getValue()
{
	return value;
}
void Gold::die(){}

// Character superclass for Player and Enemy
void Character::setHP(int hp)
{
	currentHP = hp;
}
void Character::setAtk(int atk)
{
	currentAtk = atk;
}
void Character::setDef(int def)
{
	currentDef = def;
}
Character::Character(char oC): Object(oC) {}
void Character::move(int dir)
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
int Character::getHP()
{
	return currentHP;
}
int Character::getAtk()
{
	return currentAtk;
}
int Character::getDef()
{
	return currentDef;
}

// Subclasses
Player::Player(): Character(PLAYER), tileChar(FLOOR), pRace(HUMAN)
{
	gold = 0;
	message = "";
	setHP(HUMAN.hp);
	setAtk(HUMAN.atk);
	setDef(HUMAN.def);
}
char Player::getTileChar()
{
	return tileChar;
}
void Player::setTileChar(char c)
{
	tileChar = c;
}
double Player::getGold()
{
	return gold;
}
string Player::getRace()
{
	return pRace.name;
}
string Player::getMessage()
{
	return message;
}
void Player::setMessage(string m)
{
	message = m;
}
void Player::initGold(double g) // Make this more selective later
{
	if (gold == 0)
		gold = g;
}
void Player::initHP(int hp) // Make this more selective later
{
	if (getHP() == pRace.hp)
		setHP(hp);
}
void Player::initRace(char c)
{
	if (c == 'h')
		pRace = HUMAN;
	else if (c == 'd')
		pRace = DWARF;
	else if (c == 'e')
		pRace = ELF;
	else if (c == 'o')
		pRace = ORC;
	// Sets current stats to match race
	setHP(pRace.hp);
	setAtk(pRace.atk);
	setDef(pRace.def);
}
void Player::addGold(double value)
{
	int collected;
	if (pRace == DWARF)
		collected = 2 * value;
	else if (pRace == ORC)
		collected = value / 2;
	else
		collected = value;
	stringstream m;
	m << getMessage() << "PC collects " << collected << " gold. ";
	setMessage(m.str());
	gold += collected;
}
void Player::usePotion(Potion &p)
{
	if(pRace == ELF)
	{
		setHP(getHP() + abs(p.getHP()));
		setAtk(getAtk() + abs(p.getAtk()));
		setDef(getDef() + abs(p.getDef()));
	}
	else
	{
		setHP(getHP() + p.getHP());
		setAtk(getAtk() + p.getAtk());
		setDef(getDef() + p.getDef());
	}
	if (getHP() > pRace.hp)
		setHP(pRace.hp);
	if (getHP() < 0)
		setHP(0);
	if (getAtk() < 0)
		setAtk(0);
	if (getDef() < 0)
		setDef(0);
	stringstream m;
	m << getMessage() << "PC uses " << p.getType() << ". ";
	setMessage(m.str());
	
}
bool Player::attack(Enemy * e){
	double damage = ceil((100.0/(100.0 + (double)e->getDef()))*((double)getAtk()));
	e->setHP((e->getHP()-damage));
	bool lethal = (e->getHP() <= 0);
	stringstream m;
	m << getMessage() << "PC deals " << damage << " damage to " << e->getRace();
	if (lethal) {
		m << ", slaying it. ";
	}
	else
	{
		m << ", it has " << e->getHP() << " health remaining. ";
	}
	setMessage(m.str());
	return lethal;
	
}
bool Player::getAttacked(Enemy e){
	int hit = rand() % 2;
	stringstream m;
	if (hit == 1) {
		double damage = ceil((100.0/(100.0 + (double)getDef()))*((double)e.getAtk()));
		setHP((getHP()-damage));
		m << getMessage() << e.getRace() << " deals " << damage << " damage to PC. ";
	}
	else
	{
		m << getMessage() << e.getRace() << " attacks and misses PC. ";
	}
	setMessage(m.str());
	return (getHP() <= 0);
}

void Player::die(){}

// Since dragons are generated before other enemies, default race should be dragon! (?)
Enemy::Enemy(): Character(ENEMY), eRace(DRAGON), hoard(NULL){}
void Enemy::initRace(race r)
{
	if (eRace == DRAGON)
	{
		eRace = r;
		setHP(eRace.hp);
		setAtk(eRace.atk);
		setDef(eRace.def);
		if (eRace.name == "Goblin")
			setObjectChar('N');
		else if (eRace.name == "Phoenix")
			setObjectChar('X');
		else // except for goblin and phoenix, first character of name is also character on display
			setObjectChar(eRace.name[0]);
	}
}
string Enemy::getRace()
{
	return eRace.name;
}

void Enemy::initHoard(Gold * g)
{
	hoard = g;
}
void Enemy::freeHoard()
{
	hoard->setGuarded(false);
}
posn Enemy::hoardLoc()
{
	if (hoard != NULL)
	{
		return hoard->getLoc();
	}
	else
	{
		return nullPosn;
	}
}

void Enemy::die(){}
