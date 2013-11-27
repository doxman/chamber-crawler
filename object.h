#ifndef __OBJECT_H__
#define __OBJECT_H__
#include <string>

struct posn
{
	int row;
	int col;
	posn dirAdjacent(int dir);
	int findDir(posn other);
};

struct race
{
	int hp;
	int atk;
	int def;
	std::string name;
};

bool operator==(race arg1, race arg2);
bool operator==(posn arg1, posn arg2);
const posn nullPosn = {-1, -1};

const char FLOOR = '.';

// Player races
const race HUMAN = {140, 20, 20, "Human"};
const race DWARF = {100, 20, 30, "Dwarf"};
const race ELF = {140, 30, 10, "Elf"};
const race ORC = {180, 30, 25, "Orc"}; //Divide by 100, made orc op for testing

// Enemy races
const race VAMPIRE = {50, 25, 25, "Vampire"};
const race WEREWOLF = {120, 30, 5, "Werewolf"};
const race TROLL = {120, 25, 15, "Troll"};
const race GOBLIN = {70, 5, 10, "Goblin"};
const race PHOENIX = {50, 35, 25, "Phoenix"};
const race MERCHANT = {30, 70, 5, "Merchant"};
const race DRAGON = {150, 20, 20, "Dragon"};

// Potion types (uses race struct for convenience)
const race RH = {10, 0, 0, "RH"};
const race BA = {0, 5, 0, "BA"};
const race BD = {0, 0, 5, "BD"};
const race PH = {-10, 0, 0, "PH"};
const race WA = {0, -5, 0, "WA"};
const race WD = {0, 0, -5, "WD"};

// Gold types (differentiated by value)
const int NORMAL = 1;
const int SMALL_HOARD = 2;
const int MERCHANT_HOARD = 4;
const int DRAGON_HOARD = 6;

// Directions
const int numDirections = 8;

const int NORTHWEST = 0;
const int NORTH = 1;
const int NORTHEAST = 2;
const int WEST = 3;
const int EAST = 4;
const int SOUTHWEST = 5;
const int SOUTH = 6;
const int SOUTHEAST = 7;

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
	void setObjectChar(char c); // Used by Enemy class to make different races look different
public:
	Object(char oC);
	char getObjectChar();
	posn getLoc();
	void setLoc(posn p);
	void initLoc(posn p); // Only works when loc is the null position; used to initialize posn;
	virtual void die() = 0; // Might get rid of this? Hasn't actually been used yet...
};

class Stairs:public Object
{
public:
	Stairs();
	void die();
};

class Potion:public Object
{
	int hpMod;
	int atkMod;
	int defMod;
	std::string type;
public:
	Potion(); // Makes a potion that does nothing; placeholder
	void initPotion(race p); // Initializes empty potion; race structure used for potion mods
	int getHP();
	int getAtk();
	int getDef();
	std::string getType();
	int getTypeNum();
	void die();
};

class Gold:public Object
{
	int value;
	bool guarded;
public:
	Gold();
	void initValue(int v);
	void setGuarded(bool guard);
	bool isGuarded();
	int getValue();
	void die();
};

class Character:public Object
{
	int currentHP;
	int currentAtk;
	int currentDef;
protected:
	void setAtk(int atk);
	void setDef(int def);
public:
	Character (char oC);
	void move(int dir);
	void setHP(int hp);
	int getHP();
	int getAtk();
	int getDef();
};

class Enemy:public Character
{
	race eRace;
	posn hoard;
public:
	Enemy();
	void initRace(race r);
	std::string getRace();
	void initHoard(posn p);
	//void freeHoard();
	posn hoardLoc();
	void die();
};

class Player:public Character
{
	char tileChar;
	race pRace;
	double gold;
	std::string message;
public:
	Player(); // Makes a default human player; placeholder
	char getTileChar();
	void setTileChar(char c);
	double getGold();
	void setMessage(std::string m);
	std::string getMessage();
	std::string getRace();
	void initGold(double g); // Only works when gold is at 0; sets last floor's value
	void initHP(int hp);   // Only works when hp is at race total; sets last floor's value
	void initRace(char c); // Initializes race from char; used when race is read in at start
	void addGold(double value);
	void usePotion (Potion &p);
	void oxmanly(); //Cheats are fun!
	void grosslyOverpowered(); //Hooray!
	void die();
	bool attack(Enemy * e); //Returns true if the attack kills the enemy
	bool getAttacked(Enemy e); //Returns true if the attack kills the player
};

#endif
