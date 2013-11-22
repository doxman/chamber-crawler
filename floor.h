#ifndef __FLOOR_H__
#define __FLOOR_H__
#include "display.h"
#include "object.h"
#include <vector>

const int numChambers = 5;
const int potionsSpawned = 10;
const int goldsSpawned = 10;
const int enemiesSpawned = 20;

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
	int playerChamber;		  // Stores which chamber the player spawned in
	int numPotions;
	int numGolds;
	int numEnemies;
	void flood(char floodChar, char ***gridptr, int r, int c, int chamberNum);
	void floodGrid(char **grid);
	void unfloodGrid(char **grid);
	void spawnObject(char c); // Spawns a different object depending on the symbol given
	void sortEnemies();
	void moveEnemy(Enemy *e);
public:
	Floor();
	~Floor();
	void init();
	void round();
	void print();
};

#endif
