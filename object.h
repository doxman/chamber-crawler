#ifndef __OBJECT_H__
#define __OBJECT_H__
#include <string>
#include "display.h"

// Structure used to package a race (HP, attack, defense, and a unique name)
struct race
{
	int hp;
	int atk;
	int def;
	std::string name;
	bool operator==(race other); // Checks equality for races
};

// Player races
const race HUMAN = {140, 20, 20, "Human"};
const race DWARF = {100, 20, 30, "Dwarf"};
const race ELF = {140, 30, 10, "Elf"};
const race ORC = {180, 30, 25, "Orc"};
const race NINJA = {50, 50, 10, "Ninja"};

// Enemy races
const race VAMPIRE = {50, 25, 25, "Vampire"};
const race WEREWOLF = {120, 30, 5, "Werewolf"};
const race TROLL = {120, 25, 15, "Troll"};
const race GOBLIN = {70, 5, 10, "Goblin"};
const race PHOENIX = {50, 35, 25, "Phoenix"};
const race MERCHANT = {30, 70, 5, "Merchant"};
const race DRAGON = {150, 20, 20, "Dragon"};
const race EVILNINJA = {10, 100, 5, "Evil Ninja"};

// Potion types (uses race struct for convenience)

const race WATER = {0, 0, 0, "WATER"};	// Null (uninitialized) value for potions
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

// Object superclass: Methods and fields common to all types of objects in the game
class Object
{
	char objectChar;			// Character that represents the object on the map
	posn loc;					// Posn representing the object's coordinates on the grid
protected:
	void setObjectChar(char c); // Used by Enemy class to make different races look different
	void setLoc(posn p); // Sets object location;
						 // Protected to limit user control over object location
public:
	Object(char oC);		// Constructor: Initializes object at nullPosn with given char
	char getObjectChar();	// Returns character that represents the object
	posn getLoc();		 	// Returns posn representing object's location
	
	void initLoc(posn p);   // Sets the object's location to given position;
							// works ONLY if the object's position is currently null
							// (ie. can only be called once per instance)
};

// Subclass of Object used for the stairs
class Stairs:public Object
{
public:
	Stairs(); // Simply creates an object with the '\' character
};

// Subclass of Object used for potions
class Potion:public Object
{
	race pRace;				 // Race struct encapsulating potion modifiers and type
public:
	Potion(); 				 // Creates an uninitialized potion ("WATER")
	void initPotion(race p); // Provides values to an uninitialized potion
	int getHP();			 // Returns potion's HP modifier
	int getAtk();			 // Returns potion's Atk modifier
	int getDef();			 // Returns potion's Def modifier
	std::string getType();	 // Returns potion's type
	int getTypeNum();		 // Returns integer corresponding to potion type (0-5)
};

// Subclass of Object used for gold
class Gold:public Object
{
	int value;				 // Integer representing the value of the gold pile
	bool guarded;			 // Stores whether the gold is guarded
							 // (true for dragon hoards while dragon is still alive,
							 // false otherwise)
public:
	Gold();					 // Initializes gold to be unguarded with a value of 0
	void initValue(int v);	 // If current value of the gold is 0, sets it to v
	void setGuarded(bool guard); // Changes whether gold is guarded or not
								 // Used with dragon hoards to make them guarded
								 // when they are spawned and then make them
								 // unguarded when the dragon is defeated
	bool isGuarded();		 // Returns true if gold is guarded, false otherwise
	int getValue();			 // Returns value of gold
};

// Subclass of Object; used as a superclass for both the player and enemies
class Character:public Object
{
	int currentHP;	// Character's current HP
	race cRace;		// Character's race
public:
	Character (char oC);  // Initializes the Character with given char and the null race
	void move(int dir);   // Moves the character to an adjacent posn in a given direction
	void setHP(int hp);   // Sets the character's current HP
	int getHP();		  // Returns the character's current HP
	std::string getName();// Returns the character's name, based on its race
	virtual int getAtk(); // Returns the character's Atk, based on its race
	virtual int getDef(); // Returns the character's Def, based on its race
	race getRace();		  // Returns the character's race (max HP, Atk, Def, name)
	void setRace(race r); // Sets the character's race
};

// Subclass of Character used for enemies
class Enemy:public Character
{
	posn hoard;			  // Stores the location of the enemy's hoard (if it's a Dragon)
						  // If the enemy is not a Dragon, this will equal nullPosn
public:
	Enemy();			  // Initializes an enemy to be a Dragon
						  // This is done because Dragons are spawned by gold and thus
						  // will not be spawned in the same way as other enemies;
						  // therefore, the initRace() method will not be used for them
	void initHoard(posn p); // Sets location of the enemy's hoard (if uninitialized)
	posn hoardLoc();	    // Returns location of the enemy's hoard
};

// Subclass of Character used for the player
class Player:public Character
{
	int currentAtk; // Player's current Atk
	int currentDef; // Player's current Def
	char tileChar;	// Character of the tile player is currently standing on
					// this is necessary because players can move on more than just floor
	double gold;	// Total gold player has picked up
					// stored as a double because Orc players may pick up 0.5 gold
	std::string message; // Stores a message to be printed to the user
						 // message contents depend on the action the player took
						 // and its degree of success
	bool shinyDLC;	// Stores whether DLC is turned on or not;
					// if it is, player may use the Ninja race, heal from phoenixes, etc
protected:
	void setAtk(int atk);
	void setDef(int def);
public:
	Player(); // Makes a default human player; placeholder
	
	// Accessor/mutator methods
	int getAtk(); // Returns player's current Atk (may differ from race)
	int getDef(); // Returns player's current Def (may differ from race)
	char getTileChar();
	void setTileChar(char c);
	double getGold();
	void setMessage(std::string m);
	std::string getMessage();
	
	// Initializer methods; used to set up the player object when a new floor begins
	void initGold(double g);// Only works when gold is at 0; 
							// sets gold to last floor's value
	void initHP(int hp);    // Only works when hp is at race total;
							// sets last floor's value
	void initRace(char c); 	// Initializes race from char;
							// used when race is read in at start
	void initDLC(bool dlc); // Initializes DLC variable
	
	// Item-related methods
	void addGold(double value);	// Adds value of a gold pile to the player
	void usePotion (Potion &p); // Applies the modifiers of a potion the player
	
	// Cheats (yay!)
	void oxmanly();				// Multiplies Def by 9001
	void grosslyOverpowered();  // Multiplies Atk by 144
	
	// Attack/damage related methods
	bool attack(Enemy * e); // Modifies enemy HP;
							// Returns true if the attack kills the enemy
	bool getAttacked(Enemy * e); // Returns true if the attack kills the player
};

#endif
