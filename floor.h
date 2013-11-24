#ifndef __FLOOR_H__
#define __FLOOR_H__
#include "display.h"
#include "object.h"
#include <vector>

const int numChambers = 5;
const int potionsSpawned = 10;
const int goldsSpawned = 10;
const int enemiesSpawned = 20;

// String constants for directions
const std::string STRNWEST = "nw";
const std::string STRNORTH = "no";
const std::string STRNEAST = "ne";
const std::string STRWEST = "we";
const std::string STREAST = "ea";
const std::string STRSWEST = "sw";
const std::string STRSOUTH = "so";
const std::string STRSEAST = "se";

struct Chamber
{
	std::vector<posn> tiles;
};

class Floor
{
	Display *d;
	Chamber chambers[numChambers];
	Player player;
	Stairs stairs;
	Potion potions[potionsSpawned];
	Gold golds[goldsSpawned];
	Enemy enemies[enemiesSpawned];
	int numPotions;
	int numGolds;
	int numEnemies;
	int playerChamber;		  // Stores which chamber the player spawned in
	char playerRace;		  // Stores the player's race, as a character
	int floorNum;
	int playerHP; 			  // Holds player HP when transitioning between floors
	double playerGold;		  // Holds player gold when transitioning between floors
	void flood(char floodChar, char ***gridptr, int r, int c, int chamberNum);
	void floodGrid(char **grid);
	void unfloodGrid(char **grid);
	void spawnObject(char c); // Spawns a different object depending on the symbol given
	bool tryMove(int dir); // False if player moved onto stairs
	bool playerTurn(); // False if not followed by enemy turns (ie. moved up a floor or quit)
	void sortEnemies();
	void moveEnemy(Enemy *e);
public:
	Floor(char pR);
	~Floor();
	void print();
	void init();
	void round();
	void nextFloor();
	int getFloorNum();
};

#endif
