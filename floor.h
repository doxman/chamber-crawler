#ifndef __FLOOR_H__
#define __FLOOR_H__
#include "display.h"
#include "object.h"
#include <vector>
#include <fstream>

// Integer constants for number of objects spawned by type
const int potionsSpawned = 10;
const int goldsSpawned = 10;
const int enemiesSpawned = 20;

// Number of different potions available in the game
const int numPotionTypes = 6;

// String constants for directions
const std::string STRNWEST = "nw";
const std::string STRNORTH = "no";
const std::string STRNEAST = "ne";
const std::string STRWEST = "we";
const std::string STREAST = "ea";
const std::string STRSWEST = "sw";
const std::string STRSOUTH = "so";
const std::string STRSEAST = "se";

// String arrays used to generate insulting messages for invalid commands:
// Invalid movement
const std::string blockedMove[3] = 
{"PC walks face first into an obstacle. ", 
 "PC attempts to become incorporeal and pass through an occupied space, and fails. ",
 "PC forgets that noclip is off. "};
// Invalid potion use
const std::string noPotion[3] = 
{"PC embarrasingly attempts to drink the ground. ",
 "PC licks the floor, and immediately regrets everything. ",
 "PC realizes that they are really thirsty. "};
// Invalid attack
const std::string noEnemy[3] = 
{"PC swings wildly at thin air, and generally makes a fool of themself. ",
"PC slays all the monsters in that space. There weren't any. ",
"PC shows off their fighting prowess. No one is impressed. "};



// Main control class of the game; generates floors and runs the game
class Floor
{
	Display d;
	Player player;
	Stairs stairs;
	Potion potions[potionsSpawned];				// Stores all potions on the floor
	Gold golds[goldsSpawned + enemiesSpawned];	// Stores all gold piles on the floor
												// Leaves extra room for gold that
												// is dropped by slain enemies
	Enemy enemies[enemiesSpawned];				// Stores all enemies on the floor
	int numPotions;				 // Number of potions currently on the floor
	int numGolds;				 // Number of gold piles currently on the floor
	int numEnemies;				 // Number of enemies currently on the floor
	bool shinyDLC;				 // Tracks whether DLC is being used
	bool angryMerchants;		 // Tracks whether player has attacked a merchant
	bool quit;
	bool restart;
	bool knownPotions[numPotionTypes];
	int playerChamber;           // Stores which chamber the player spawned in
	char playerRace;             // Stores the player's race, as a character
	int floorNum;				 // Tracks the current number of the floor
	int playerHP;                // Holds player HP when transitioning between floors
	double playerGold;           // Holds player gold when transitioning between floors
	char *fileName;	// Used to store name of the file that floors are being read from
					// Set to " " if floor is not set to read from a file
	std::ifstream *fileIn;	// Points to file stream, provided a file is given
	void spawnObject(char c); // Spawns a different object depending on the symbol given
	bool tryMove(int dir); // Deals with player movement ()
	bool playerTurn(); // Determines what the player does on their turn
	// Returns false if not followed by enemy turns (ie. moved up a floor or quit)
	void sortEnemies(); // Sorts array of enemies into order, top left to bottom right
	void moveEnemy(Enemy *e); // Determines what an enemy does on its turn
public:
	Floor(char pR, char *fN, bool dlc);
	int clockwise(int dir);	// Finds the direction directly clockwise from the one given
	int cClockwise(int dir);// ""	 ""	 ""			 counterclockwise from the one given
	~Floor();
	void print();			// Prints the display and the information text below it
	void init();			// Initializes a floor (used at start of game and when
							// player goes up a floor)
	void round();			// Deals with one game round (player actions and 
							// enemy actions)
	void nextFloor();		// Advances up a floor, by initializing it and saving stats
							// such as gold and HP
	bool shouldQuit();		// Returns true if player has decided to quit
	bool shouldRestart();	// Returns true if player has decided to restart
	int getFloorNum();		// Returns the number of the current floor
	void endGame();			// Ends the game (if player has decided to quit or restart,
							// or if player has died/won in-game)
};

#endif
