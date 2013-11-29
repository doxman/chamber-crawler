#ifndef __FLOOR_H__
#define __FLOOR_H__
#include "display.h"
#include "object.h"
#include <vector>
#include <fstream>

const int numChambers = 5;
const int potionsSpawned = 10;
const int goldsSpawned = 10;
const int enemiesSpawned = 20;
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
const std::string blockedMove[3] = {"PC walks face first into an obstacle. ", "PC attempts to become incorporeal and pass through an occupied space, and fails. ", "PC forgets that noclip is off. "};
const std::string noPotion[3] = {"PC embarrasingly attempts to drink the ground. ", "PC licks the floor, and immediately regrets everything. ", "PC realizes that they are really thirsty. "};
const std::string noEnemy[3] = {"PC swings wildly at thin air, and generally makes a fool of themself. ", "PC slays all the monsters in that space. There weren't any. ", "PC shows off their fighting prowess. No one is impressed. "};
struct Chamber
{
	std::vector<posn> tiles;
};

class Floor
{
	Display d;
	Chamber chambers[numChambers];
	Player player;
	Stairs stairs;
	Potion potions[potionsSpawned];
	Gold golds[goldsSpawned + enemiesSpawned];
	Enemy enemies[enemiesSpawned];
	int numPotions;
	int numGolds;
	int numEnemies;
	bool shinyDLC;
	bool angryMerchants;
	bool quit;
	bool restart;
	bool knownPotions[numPotionTypes];
	int playerChamber;                  // Stores which chamber the player spawned in
	char playerRace;                  // Stores the player's race, as a character
	int floorNum;
	int playerHP;                           // Holds player HP when transitioning between floors
	double playerGold;                  // Holds player gold when transitioning between floors
	char *fileName;	// Used to store name of the file that floors are being read from
					// Set to " " if floor is not set to read from a file
	std::ifstream *fileIn;	// Points to file stream, provided a file is given
	void flood(char floodChar, char grid[][WIDTH], int r, int c, int chamberNum);
	void floodGrid(char grid[][WIDTH]);
	void unfloodGrid(char grid[][WIDTH]);
	void spawnObject(char c); // Spawns a different object depending on the symbol given
	bool tryMove(int dir); // False if player moved onto stairs
	bool playerTurn(); // False if not followed by enemy turns (ie. moved up a floor or quit)
	void sortEnemies();
	void moveEnemy(Enemy *e);
public:
	Floor(char pR, char *fN, bool dlc);
	int clockwise(int dir);
	int cClockwise(int dir);
	~Floor();
	void print();
	void init();
	void round();
	void nextFloor();
	bool shouldQuit();
	bool shouldRestart();
	int getFloorNum();
	void endGame();
};

#endif
