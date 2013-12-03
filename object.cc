#include "object.h"
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sstream>
using namespace std;

// Equality test for races
bool race::operator==(race other)
{
	if (hp == other.hp && atk == other.atk && def == other.def
		&& name == other.name)
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

Potion::Potion(): Object(POTION)
{
	pRace = WATER;
}
void Potion::initPotion(race p)
{
	if (pRace.name == "WATER")
	{
		pRace.hp = p.hp;
		pRace.atk = p.atk;
		pRace.def = p.def;
		pRace.name = p.name;
	}
}
int Potion::getHP() {return pRace.hp;}
int Potion::getAtk() {return pRace.atk;}
int Potion::getDef() {return pRace.def;}
string Potion::getType() {return pRace.name;}
int Potion::getTypeNum()
{
	if (pRace.name == "RH")
		return 0;
	else if (pRace.name == "BA")
		return 1;
	else if (pRace.name == "BD")
		return 2;
	else if (pRace.name == "PH")
		return 3;
	else if (pRace.name == "WA")
		return 4;
	else
		return 5;
}

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

// Character superclass for Player and Enemy
void Character::setHP(int hp)
{
	currentHP = hp;
}
Character::Character(char oC): Object(oC), cRace(DRAGON) {}
void Character::move(int dir)
{
	posn current = getLoc();
	setLoc(current.dirAdjacent(dir));
}
int Character::getHP()
{
	return currentHP;
}
int Character::getAtk()
{
	return cRace.atk;
}
int Character::getDef()
{
	return cRace.def;
}
string Character::getName()
{
	return cRace.name;
}
race Character::getRace()
{
	return cRace;
}
void Character::setRace(race r)
{
	cRace = r;
	setHP(cRace.hp);
	// Set up character accordingly
	if (cRace.name == "Human" ||
		cRace.name == "Dwarf" || cRace.name == "Elf" ||
	    cRace.name == "Orc" || cRace.name == "Ninja") // Possible player races
	    setObjectChar('@');
	else if (cRace.name == "Goblin")
		setObjectChar('N');
	else if (cRace.name == "Phoenix")
		setObjectChar('X');
	else if (cRace.name == "Evil Ninja")
		setObjectChar(',');
	else // except for these, first character of name is also the character displayed
		setObjectChar(cRace.name[0]);
}

// Subclasses
Player::Player(): Character(PLAYER), tileChar(FLOOR)
{
	gold = 0;
	shinyDLC = false;
	message = "";
	setHP(HUMAN.hp);
	setAtk(HUMAN.atk);
	setDef(HUMAN.def);
}
void Player::setAtk(int atk)
{
	currentAtk = atk;
}
void Player::setDef(int def)
{
	currentDef = def;
}
int Player::getAtk()
{
	return currentAtk;
}
int Player::getDef()
{
	return currentDef;
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
	if (getHP() == getRace().hp)
		setHP(hp);
}
void Player::initRace(char c)
{
	if (c == 'h')
		setRace(HUMAN);
	else if (c == 'd')
		setRace(DWARF);
	else if (c == 'e')
		setRace(ELF);
	else if (c == 'o')
		setRace(ORC);
	else if (c == 'n')
		setRace(NINJA);
		
	// Sets current stats to match race
	setHP(getRace().hp);
	setAtk(getRace().atk);
	setDef(getRace().def);
}
void Player::initDLC(bool dlc)
{
	shinyDLC = dlc;
}
void Player::addGold(double value)
{
	double collected;
	if (getRace() == DWARF)
		collected = 2 * value;
	else if (getRace() == ORC)
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
	if(getRace() == ELF)
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
	if (getHP() > getRace().hp)
		setHP(getRace().hp);
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
bool Player::attack(Enemy * e)
{
	double damage = ceil((100.0/(100.0 + (double)e->getDef()))*((double)getAtk()));
	e->setHP((e->getHP()-damage));
	bool lethal = (e->getHP() <= 0);
	stringstream m;
	m << getMessage() << "PC deals " << damage << " damage to " << e->getName();
	if (lethal) {
		m << ", slaying it. ";
		if (e->getName() == "Phoenix" && shinyDLC)
		{
			int heal = min(getDef(), (getRace().hp - getHP()));
			m << "PC heals themself for " << heal << " health using the phoenix down. ";
			setHP(getHP() + heal);
		}
	}
	else
	{
		m << ", it has " << e->getHP() << " health remaining. ";
	}
	setMessage(m.str());
	return lethal;
}
bool Player::getAttacked(Enemy * e)
{
	int hit = rand() % 2;
	stringstream m;
	if (hit == 1) {
		int damage = ceil((100.0/(100.0 + (double)getDef()))*((double)e->getAtk()));
		setHP((getHP()-damage));
		m << getMessage() << e->getName() << " deals " << damage << " damage to PC. ";
		if (shinyDLC && e->getName() == "Vampire")
		{
			int heal = min(damage / 5, (50 - e->getHP()));
			e->setHP(e->getHP() + heal);
			m << "Vampire heals " << heal << " HP, and now has " << e->getHP() << " health. ";
		}
	}
	else
		m << getMessage() << e->getName() << " attacks and misses PC. ";
	setMessage(m.str());
	return (getHP() <= 0);
}
void Player::oxmanly()
{
	if (getDef() < 9001)
		setDef(9001 * getDef());
	setMessage(getMessage() + "PC becomes Oxmanly! ");
}
void Player::grosslyOverpowered()
{
	if (getAtk() < 20736)
		setAtk(144 * getAtk());
	setMessage(getMessage() + "PC becomes Grossly Overpowered! ");
}

Enemy::Enemy(): Character(ENEMY), hoard(nullPosn){}
void Enemy::initHoard(posn p)
{
	hoard = p;
}
posn Enemy::hoardLoc()
{
	return hoard;
}
