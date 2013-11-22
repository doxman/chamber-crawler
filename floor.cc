#include "floor.h"
#include <fstream>
#include <cstdlib>
using namespace std;

// PRIVATE METHODS
void Floor::flood(char floodChar, char ***gridptr, int r, int c, int chamberNum)
{
	char **grid = *gridptr;
	posn temp = {r, c};
	chambers[chamberNum].tiles.push_back(temp);
	grid[r][c] = floodChar;
	if (r > 0 && grid[r-1][c]==FLOOR)
		flood(floodChar, &grid, r-1, c, chamberNum);
	if (r < (HEIGHT - 1) && grid[r+1][c]==FLOOR)
		flood(floodChar, &grid, r+1, c, chamberNum);
	if (c > 0 && grid[r][c-1]==FLOOR)
		flood(floodChar, &grid, r, c-1, chamberNum);
	if (c < (WIDTH - 1) && grid[r][c+1]==FLOOR)
		flood(floodChar, &grid, r, c+1, chamberNum);
}
void Floor::floodGrid (char **grid)
{
	char floodChar = 'a';
	int chamberNum = floodChar - 'a';
	for (int r = 0; r < HEIGHT; r++)
	{
		for (int c = 0; c < WIDTH; c++)
		{
			if (grid[r][c] == FLOOR)
			{
				flood(floodChar, &grid, r,c, chamberNum);
				floodChar++;
				chamberNum++;
			}
		}
	}
}
void Floor::unfloodGrid(char **grid)
{
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (grid[i][j] >= 'a' && grid[i][j] <= 'e') // If the tile was previously flooded
				grid[i][j] = FLOOR;
		}
	}
}
void Floor::spawnObject(char c)
{
	int chamberNumber = -1;
	// Makes sure player doesn't spawn in the same room as the stairs
	if (c == STAIRS)
	{
		chamberNumber = rand() % (numChambers - 1);
		if (chamberNumber >= playerChamber)
			chamberNumber++;
	}
	else
		chamberNumber = rand() % numChambers;
	// Stores player's chamber value for reference later
	if (c == PLAYER)
		playerChamber = chamberNumber;
	int numTiles = chambers[chamberNumber].tiles.size();
	int tileNumber = rand() % numTiles;
	posn temp = chambers[chamberNumber].tiles[tileNumber];
	if (c == PLAYER)
	{
		player.initLoc(temp);
		d->setChar(temp.row, temp.col, c);
	}
	else if (c == STAIRS)
	{
		stairs.initLoc(temp);
		d->setChar(temp.row, temp.col, c);
	}
	else if (c == POTION)
	{
		potions[numPotions].initLoc(temp);
		d->setChar(temp.row, temp.col, c);
		numPotions++;
	}
	else if (c == GOLD)
	{
		golds[numGolds].initLoc(temp);
		d->setChar(temp.row, temp.col, c);
		numGolds++;
	}
	else if (c == ENEMY)
	{
		enemies[numEnemies].initLoc(temp);
		d->setChar(temp.row, temp.col, c);
		numEnemies++;
	}
	chambers[chamberNumber].tiles.erase(chambers[chamberNumber].tiles.begin() + tileNumber);
}

void Floor::sortEnemies()
{
	Enemy sorted[numEnemies];
	int numSorted = 0;
	int currentMin; // Value of smallest remaining element: (79 * row) + col
	int minIndex; // Location of smallest remaining element
	int threshold = -1; // Lowest value we check; prevents counting the same element twice
	int temp; // Temporarily holds value of each element
	while (numSorted < numEnemies)
	{
		currentMin = 79 * 25;
		minIndex = -1;
		for (int i = 0; i < numEnemies; i++)
		{
			temp = 79 * enemies[i].getLoc().row + enemies[i].getLoc().col;
			if (temp < currentMin && temp > threshold)
			{
				currentMin = temp;
				minIndex = i;
			}
		}
		sorted[numSorted] = enemies[minIndex];
		threshold = currentMin;
		numSorted++;
	}
	// Sorted; now set established array to be sorted
	for (int i = 0; i < numEnemies; i++)
		enemies[i] = sorted[i];
}

void Floor::moveEnemy(Enemy *e)
{
	posn loc = e->getLoc();
	bool open[numDirections]; // Need to initialize these to false
	for (int i = 0; i < numDirections; i++)
		open[i] = false;
	int availableMoves = 0;
	if (*d->getChar(loc.row - 1, loc.col) == FLOOR)
	{
		open[NORTH] = true;
		availableMoves++;
	}
	if (*d->getChar(loc.row - 1, loc.col - 1) == FLOOR)
	{
		open[NORTHWEST] = true;
		availableMoves++;
	}
	if (*d->getChar(loc.row - 1, loc.col + 1) == FLOOR)
	{
		open[NORTHEAST] = true;
		availableMoves++;
	}
	if (*d->getChar(loc.row, loc.col - 1) == FLOOR)
	{
		open[WEST] = true;
		availableMoves++;
	}
	if (*d->getChar(loc.row, loc.col + 1) == FLOOR)
	{
		open[EAST] = true;
		availableMoves++;
	}
	if (*d->getChar(loc.row + 1, loc.col - 1) == FLOOR)
	{
		open[SOUTHWEST] = true;
		availableMoves++;
	}
	if (*d->getChar(loc.row + 1, loc.col + 1) == FLOOR)
	{
		open[SOUTHEAST] = true;
		availableMoves++;
	}
	if (*d->getChar(loc.row + 1, loc.col) == FLOOR)
	{
		open[SOUTH] = true;
		availableMoves++;
	}
	if (availableMoves == 0) // All adjacent tiles are blocked
		return;
	d->setChar(loc.row, loc.col, FLOOR); // Clears the tile the enemy is leaving
	int move = rand() % availableMoves;
	int counter = 0;
	for (int i = NORTH; i < numDirections; i++)
	{
		if (open[i] && counter == move)
		{
			e->move(i);
			break;
		}
		if (open[i])
			counter++;
	}
	loc = e->getLoc(); // New position
	d->setChar(loc.row, loc.col, e->getObjectChar()); // Draws the enemy's new position
}

// PUBLIC METHODS
Floor::Floor()
{
	d = new Display();
	playerChamber = -1;
	numPotions = 0;
	numGolds = 0;
	numEnemies = 0;
}
Floor::~Floor()
{
	delete d;
}
void Floor::init()
{
	ifstream f("test.ccmap");
	f >> *d;
	char **temp;
	temp = d->getDisplay();
	floodGrid(temp);
	unfloodGrid(temp);
	srand(time(NULL)); // Sets up rand()
	spawnObject(PLAYER);
	spawnObject(STAIRS);
	while(numPotions < potionsSpawned)
		spawnObject(POTION);
	while(numGolds < goldsSpawned)
		spawnObject(GOLD);
	while(numEnemies < enemiesSpawned)
		spawnObject(ENEMY);
	sortEnemies();
}
void Floor::round()
{
	sortEnemies();
	for (int i = 0; i < numEnemies; i++)
		moveEnemy(&enemies[i]);
}
void Floor::print()
{
	cout << *d << endl;
	// Print list of posns in each chamber
	/* BLOCK SAVED FOR TESTING
	for (int i = 0; i < numChambers; i++)
	{
		cout << "Chamber: " << i << endl;
		int numTiles = chambers[i].tiles.size();
		for (int j = 0; j < numTiles; j++)
			cout << "(" << chambers[i].tiles[j].row << ", " 
						<< chambers[i].tiles[j].col << ")" << endl;
	}
	// BLOCK SAVED FOR TESTING*/
	// Print coordinates of each enemy, in sorted order
	/* BLOCK SAVED FOR TESTING
	for (int i = 0; i < numEnemies; i++)
	{
		cout << "Enemy " << i << " position: " << enemies[i].getLoc().row
			 << ", " << enemies[i].getLoc().col << endl;
	}
	// BLOCK SAVED FOR TESTING*/
}
