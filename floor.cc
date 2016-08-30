#include "floor.h"
#include <cstdlib>
#include <sstream>
using namespace std;

// PRIVATE METHODS
void Floor::spawnObject(char c)
{
	int choice = -1; // Used to randomly select a chamber and pick object subtypes
	// Makes sure player doesn't spawn in the same room as the stairs
	if (c == STAIRS)
	{
		choice = rand() % (numChambers - 1);
		if (choice >= playerChamber)
			choice++;
	}
	else
		choice = rand() % numChambers;
	// Stores player's chamber value for reference later
	if (c == PLAYER)
		playerChamber = choice;
	int numTiles = d.getChamberSize(choice);
	int tileNumber = rand() % numTiles;
	posn temp = d.getTile(choice, tileNumber);
	d.occupyTile(choice, tileNumber);
	if (c == PLAYER)
	{
		player.initLoc(temp);
		player.initRace(playerRace);
		player.initGold(playerGold);
		player.initDLC(shinyDLC); 
		if (playerHP != 0)
			player.initHP(playerHP);
		d.setChar(temp.row, temp.col, c);
	}
	else if (c == STAIRS)
	{
		stairs.initLoc(temp);
		d.setChar(temp.row, temp.col, c);
	}
	else if (c == POTION)
	{
		potions[numPotions].initLoc(temp);
		choice = rand() % 6;
		if (choice == 0)
			potions[numPotions].initPotion(RH);
		else if (choice == 1)
			potions[numPotions].initPotion(BA);
		else if (choice == 2)
			potions[numPotions].initPotion(BD);
		else if (choice == 3)
			potions[numPotions].initPotion(PH);
		else if (choice == 4)
			potions[numPotions].initPotion(WA);
		else
			potions[numPotions].initPotion(WD);
		d.setChar(temp.row, temp.col, c);
		numPotions++;
	}
	else if (c == GOLD)
	{
		golds[numGolds].initLoc(temp);
		d.setChar(temp.row, temp.col, c);
		choice = rand() % 8; //Increased from 8 to make more dragons
		if (choice < 5)
		{
			golds[numGolds].initValue(NORMAL);
			golds[numGolds].setGuarded(false);
		}
		else if (choice < 7)
		{
			golds[numGolds].initValue(SMALL_HOARD);
			golds[numGolds].setGuarded(false);
		}
		else
		{
			bool open[numDirections]; // Need to initialize these to false
			for (int i = 0; i < numDirections; i++)
				open[i] = false;
			int availableSpaces = 0;
			int counter = 0; // Goes through the 8 directions controlled by the array
			for (int i = temp.row - 1; i <= temp.row + 1; i++)
			{
				for (int j = temp.col - 1; j <= temp.col + 1; j++)
				{
					if (i == temp.row && j == temp.col)
						continue;
					if (d.getChar(i, j) == FLOOR)
					{
						open[counter] = true;
						availableSpaces++;
					}
					counter++;
				}
			}
			if (availableSpaces < 1) // All adjacent tiles are blocked
			{
				golds[numGolds].initValue(SMALL_HOARD);//No room for a dragon, make this a small hoard instead
				golds[numGolds].setGuarded(false);
			}
			else
			{
				int dspace = rand() % availableSpaces;
				counter = 0;
				for (int i = NORTHWEST; i < numDirections; i++)
				{
					if (open[i] && counter == dspace)
					{
						//Spawn a dragon
						golds[numGolds].initValue(DRAGON_HOARD);
						golds[numGolds].setGuarded(true);
						posn p = temp.dirAdjacent(i);
						enemies[numEnemies].initLoc(p);
						enemies[numEnemies].setRace(DRAGON);
						enemies[numEnemies].initHoard(temp);
						d.setChar(temp.row, temp.col, GOLD);
						d.setChar(p.row, p.col, enemies[numEnemies].getObjectChar());
						numEnemies++;
					}
					if (open[i])
						counter++;
				}
			}
			
		}
		
		numGolds++;
	}
	else if (c == ENEMY)
	{
		enemies[numEnemies].initLoc(temp);
		int spawnDivisor = shinyDLC ? 19 : 18;
		choice = rand() % spawnDivisor;
		if (choice < 4) // werewolf 2/9
			enemies[numEnemies].setRace(WEREWOLF);
		else if (choice < 7) // vampire 3/18
			enemies[numEnemies].setRace(VAMPIRE);
		else if (choice < 12) // goblin 5/18
			enemies[numEnemies].setRace(GOBLIN);
		else if (choice < 14) // troll 1/9
			enemies[numEnemies].setRace(TROLL);
		else if (choice < 16) // phoenix 1/9
			enemies[numEnemies].setRace(PHOENIX);
		else if (choice < 18) // merchant 1/9
			enemies[numEnemies].setRace(MERCHANT);
		else
			enemies[numEnemies].setRace(EVILNINJA);
		d.setChar(temp.row, temp.col, enemies[numEnemies].getObjectChar());
		numEnemies++;
	}
}

bool Floor::tryMove(int dir)
{
	posn current = player.getLoc();
	int row;
	int col;
	posn p = current.dirAdjacent(dir);
	row = p.row;
	col = p.col;
	if (d.getChar(row, col) == STAIRS) // go to next floor immediately
	{
		nextFloor();
		return false;
	}
	if (d.getChar(row, col) == GOLD)
	{
		int goldNum;
		for (goldNum = 0; goldNum < numGolds; goldNum++)
		{
			posn loc = golds[goldNum].getLoc();
			if (loc.row == row && loc.col == col)
				break;
		}
		if (!golds[goldNum].isGuarded()) // If not guarded dragon hoard
		{
			player.addGold(golds[goldNum].getValue());
			d.setChar(row, col, FLOOR);
			// Delete picked-up gold from gold array
			numGolds--;
			for (goldNum = goldNum; goldNum < numGolds; goldNum++)
				golds[goldNum] = golds[goldNum + 1];
		}
		else
		{
			player.setMessage(player.getMessage() + "Dragon scares PC away from their hoard. ");
		}
	}
	// Can move if tile is floor, passage, or door (after gold is collected)
	char target = d.getChar(row, col);
	if (target == FLOOR || target == '#' || target == '+')
	{
		d.setChar(player.getLoc().row, player.getLoc().col, player.getTileChar());
		player.setTileChar(target);
		d.setChar(row, col, PLAYER);
		player.move(dir);
		//Show potions
		posn adj;
		int potNum;
		for (int i = 0; i < numDirections; i++) {
			adj = p.dirAdjacent(i);
			if (d.getChar(adj.row, adj.col) == POTION) {
				stringstream m;
				m << player.getMessage() << "There is a";
				
				for (potNum = 0; potNum < numPotions; potNum++)
				{
					posn potLoc = potions[potNum].getLoc();
					if (potLoc == adj)
					{
						if (knownPotions[potions[potNum].getTypeNum()]) {
							m << " " << potions[potNum].getType() << " potion to the ";
							
						}
						else
						{
							m << "n unknown potion to the ";
						}
					}
				}
				if (i == NORTHWEST)
				{
					m << "northwest. ";
				}
				else if (i == NORTHEAST)
				{
					m << "northeast. ";
				}
				else if (i == NORTH)
				{
					m << "north. ";
				}
				else if (i == WEST)
				{
					m << "west. ";
				}
				else if (i == EAST)
				{
					m << "east. ";
				}
				else if (i == SOUTHWEST)
				{
					m << "southwest. ";
				}
				else if (i == SOUTHEAST)
				{
					m << "southeast. ";
				}
				else
				{
					m << "south. ";
				}
				player.setMessage(m.str());
			}
		}
	}
	else
	{
		int randNum = rand() % 3;
		if (target != GOLD)
			player.setMessage(player.getMessage() + blockedMove[randNum]);
	}
	return true;
}
bool Floor::playerTurn()
{
	int pRow = player.getLoc().row;
	int pCol = player.getLoc().col;
	cout << "Enter command: ";
	string temp;
	cin >> temp;
	if (temp == STRNWEST)
		return tryMove(NORTHWEST);
	else if (temp == STRNORTH)
		return tryMove(NORTH);
	else if (temp == STRNEAST)
		return tryMove(NORTHEAST);
	else if (temp == STRWEST)
		return tryMove(WEST);
	else if (temp == STREAST)
		return tryMove(EAST);
	else if (temp == STRSWEST)
		return tryMove(SOUTHWEST);
	else if (temp == STRSOUTH)
		return tryMove(SOUTH);
	else if (temp == STRSEAST)
		return tryMove(SOUTHEAST);
	else if (temp == "u")
	{
		int checkRow;
		int checkCol;
		cin >> temp;
		if (temp == STRNWEST)
			checkRow = pRow - 1, checkCol = pCol - 1;
		else if (temp == STRNORTH)
			checkRow = pRow - 1, checkCol = pCol;
		else if (temp == STRNEAST)
			checkRow = pRow - 1, checkCol = pCol + 1;
		else if (temp == STRWEST)
			checkRow = pRow, checkCol = pCol - 1;
		else if (temp == STREAST)
			checkRow = pRow, checkCol = pCol + 1;
		else if (temp == STRSWEST)
			checkRow = pRow + 1, checkCol = pCol - 1;
		else if (temp == STRSOUTH)
			checkRow = pRow + 1, checkCol = pCol;
		else if (temp == STRSEAST)
			checkRow = pRow + 1, checkCol = pCol + 1;
		else
			checkRow = pRow, checkCol = pCol;
		if (d.getChar(checkRow, checkCol) == 'P')
		{
			int potNum;
			for (potNum = 0; potNum < numPotions; potNum++)
			{
				posn p = potions[potNum].getLoc();
				if (checkRow == p.row && checkCol == p.col)
					break;
			}
			player.usePotion(potions[potNum]);
			knownPotions[potions[potNum].getTypeNum()] = true;
			d.setChar(checkRow, checkCol, FLOOR);
			// delete used potion from array
			numPotions--;
			for (potNum = potNum; potNum < numPotions; potNum++)
				potions[potNum] = potions[potNum + 1];
		}
		else // Insulting message?
		{
			int randNum = rand() % 3;
			player.setMessage(player.getMessage() + noPotion[randNum]);
		}
	}
	else if (temp == "a") // Attack code here
	{
		int checkRow;
		int checkCol;
		cin >> temp;
		if (temp == STRNWEST)
			checkRow = pRow - 1, checkCol = pCol - 1;
		else if (temp == STRNORTH)
			checkRow = pRow - 1, checkCol = pCol;
		else if (temp == STRNEAST)
			checkRow = pRow - 1, checkCol = pCol + 1;
		else if (temp == STRWEST)
			checkRow = pRow, checkCol = pCol - 1;
		else if (temp == STREAST)
			checkRow = pRow, checkCol = pCol + 1;
		else if (temp == STRSWEST)
			checkRow = pRow + 1, checkCol = pCol - 1;
		else if (temp == STRSOUTH)
			checkRow = pRow + 1, checkCol = pCol;
		else if (temp == STRSEAST)
			checkRow = pRow + 1, checkCol = pCol + 1;
		else
			checkRow = pRow, checkCol = pCol;
		char c = d.getChar(checkRow, checkCol);
		if (c == 'V' || c == 'W' || c == 'T' || c == 'N' || c == 'X' || c == 'M' || c == 'D' || c == ',')
		{
			int enemyNum;
			posn p;
			for (enemyNum = 0; enemyNum < numEnemies; enemyNum++)
			{
				p = enemies[enemyNum].getLoc();
				if (checkRow == p.row && checkCol == p.col)
					break;
			}
			if (c == 'M') {
				angryMerchants = true;
			}
			if(player.attack(&enemies[enemyNum]))
			{
				if (c == 'D')
				{
					d.setChar(checkRow, checkCol, FLOOR);
					posn goldLoc = enemies[enemyNum].hoardLoc();
					for (int goldNum = 0; goldNum < numGolds; goldNum++)
					{
						p = golds[goldNum].getLoc();
						if (goldLoc == p)
						{
							golds[goldNum].setGuarded(false);
							break;
						}
					}
					
				}
				else if (c == 'M')
				{
					golds[numGolds].initLoc(p);
					golds[numGolds].initValue(MERCHANT_HOARD);
					golds[numGolds].setGuarded(false);
					d.setChar(checkRow, checkCol, GOLD);
					numGolds++;
				}
				else
				{
					golds[numGolds].initLoc(p);
					golds[numGolds].initValue(NORMAL);
					golds[numGolds].setGuarded(false);
					d.setChar(checkRow, checkCol, GOLD);
					numGolds++;
					
				}
				// delete dead enemy from array
				numEnemies--;
				for (enemyNum = enemyNum; enemyNum < numEnemies; enemyNum++)
					enemies[enemyNum] = enemies[enemyNum + 1];
				//cout << "Gold number "<< numGolds << " is in position " << golds[numGolds-1].getLoc().row << " " << golds[numGolds-1].getLoc().col << " with value " << golds[numGolds-1].getValue() << endl;
			}
		}
		else // Insulting message?
		{
			int randNum = rand() % 3;
			player.setMessage(player.getMessage() + noEnemy[randNum]);
		}
		
	}
	else if (temp == "r") // Restart code here
	{
		restart = true;
		return false;
	}
	else if (temp == "q") // Quit code here
	{
		quit = true;
		return false;
	}
	else if (temp == "oxmanly")
		player.oxmanly();
	else if (temp == "grosslyoverpowered")
		player.grosslyOverpowered();
	return true;
}

void Floor::sortEnemies()
{
	Enemy *sorted = new Enemy[numEnemies];
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
	delete[] sorted;
}

void Floor::moveEnemy(Enemy *e)
{
	posn loc = e->getLoc();
	bool open[numDirections]; // Need to initialize these to false
	for (int i = 0; i < numDirections; i++)
		open[i] = false;
	int availableMoves = 0;
	int counter = 0; // Goes through the 8 directions controlled by the array
	if (shinyDLC && e->getName() == "Troll")
	{
		//Troll regen
		e->setHP(min(e->getHP() + 5, 120));
	}
	for (int i = loc.row - 1; i <= loc.row + 1; i++)
	{
		for (int j = loc.col - 1; j <= loc.col + 1; j++)
		{
			if (i == loc.row && j == loc.col)
				continue;
			if (d.getChar(i, j) == FLOOR)
			{
				open[counter] = true;
				if (e->getName()!= "Dragon") {
					availableMoves++; //Only can move if not a dragon
				}
			}
			if (d.getChar(i, j) == PLAYER)
			{
				if ((angryMerchants || e->getName()!= "Merchant")&&
					(e->getName()!= "Dragon" || ((abs(e->hoardLoc().row - i) <= 1)&&
												 (abs(e->hoardLoc().col - j) <= 1)))) {
					availableMoves = -10;
				}
			}
			counter++;
		}
	}
	if (availableMoves < 1) // All adjacent tiles are blocked
	{
		//TBD: Check if a dragon would actually attack
		if (availableMoves < 0) {
			bool lethal = player.getAttacked(e);
			if(lethal)
			{
				restart = true;
			}
		}
		return;
	}
	if (shinyDLC && (e->getName() == "Werewolf" || e->getName() == "Vampire"))
	{
		//These enemies will attempt to chase the player
		int dir = e->getLoc().findDir(player.getLoc());
		if (open[dir])
		{
			e->move(dir);
			//cout << "An enemy moves right at PC" << endl;
		}
		else if (open[clockwise(dir)])
		{
			e->move(clockwise(dir));
			//cout << "An enemy goes clockwise" << endl;
		}
		else if (open[cClockwise(dir)])
		{
			e->move(cClockwise(dir));
			//cout << "An enemy goes counter clockwise" << endl;
		}
		else if (open[clockwise(clockwise(dir))])
		{
			e->move(clockwise(clockwise(dir)));
		}
		else if (open[cClockwise(cClockwise(dir))])
		{
			e->move(cClockwise(cClockwise(dir)));
		}
		else
		{
			//cout << "An enemy waits" << endl;
			return;
		}
			
	}
	else
	{
		int move = rand() % availableMoves;
		counter = 0;
		for (int i = NORTHWEST; i < numDirections; i++)
		{
			if (open[i] && counter == move)
			{
				e->move(i);
				break;
			}
			if (open[i])
				counter++;
		}
	}
	d.setChar(loc.row, loc.col, FLOOR); // Clears the tile the enemy is leaving
	loc = e->getLoc(); // New position
	d.setChar(loc.row, loc.col, e->getObjectChar()); // Draws the enemy's new position
}

// PUBLIC METHODS
Floor::Floor(char pR, char *fN, bool dlc)
{
	shinyDLC = dlc;
	playerRace = pR;
	fileName = fN;
	if (fN[0] != '/') // If name is not null, initialize filestream
		fileIn = new ifstream(fN);
	for (int i = 0; i < numPotionTypes; i++)
		knownPotions[i] = false;
	floorNum = 1;
	playerHP = 0;
	playerGold = 0;
	quit = false;
	restart = false;
	angryMerchants = false;
	srand(time(NULL)); // Sets up rand()
}
Floor::~Floor()
{
	delete fileIn;
}
int Floor::clockwise(int dir)
{
	if (dir == NORTHWEST)
		return NORTH;
	else if (dir == NORTH)
		return NORTHEAST;
	else if (dir == NORTHEAST)
		return EAST;
	else if (dir == EAST)
		return SOUTHEAST;
	else if (dir == SOUTHEAST)
		return SOUTH;
	else if (dir == SOUTH)
		return SOUTHWEST;
	else if (dir == SOUTHWEST)
		return WEST;
	else
		return NORTHWEST;
	
}
int Floor::cClockwise(int dir)
{
	if (dir == NORTHWEST)
		return WEST;
	else if (dir == WEST)
		return SOUTHWEST;
	else if (dir == SOUTHWEST)
		return SOUTH;
	else if (dir == SOUTH)
		return SOUTHEAST;
	else if (dir == SOUTHEAST)
		return EAST;
	else if (dir == EAST)
		return NORTHEAST;
	else if (dir == NORTHEAST)
		return NORTH;
	else
		return NORTHWEST;
}
void Floor::init()
{
	d = Display();
	if (fileName[0] == '/') // If not reading from file
	{
		playerChamber = -1;
		numPotions = 0;
		numGolds = 0;
		numEnemies = 0;
		d.floodGrid();
		d.unfloodGrid();
		spawnObject(PLAYER);
		spawnObject(STAIRS);
		while(numPotions < potionsSpawned)
			spawnObject(POTION);
		while(numGolds < goldsSpawned)
			spawnObject(GOLD);
		while(numEnemies < enemiesSpawned)
			spawnObject(ENEMY);
	}
	else
	{
		*fileIn >> d;
		numPotions = 0;
		numGolds = 0;
		numEnemies = 0;
		char temp;
		posn p;
		for (int i = 0; i < HEIGHT; i++)
		{
			for (int j = 0; j < WIDTH; j++)
			{
				temp = d.getChar(i, j);
				p.row = i, p.col = j;
				if (temp == '@') // Player
				{
					player.initLoc(p);
					player.initRace(playerRace);
					player.initGold(playerGold);
					player.initDLC(shinyDLC);
					if (playerHP != 0)
						player.initHP(playerHP);
				}
				else if (temp == '\\') // Stairs
				{
					stairs.initLoc(p);
				}
				else if (temp >= '0' && temp <= '5') // Potions
				{
					potions[numPotions].initLoc(p);
					if (temp == '0')
						potions[numPotions].initPotion(RH);
					else if (temp == '1')
						potions[numPotions].initPotion(BA);
					else if (temp == '2')
						potions[numPotions].initPotion(BD);
					else if (temp == '3')
						potions[numPotions].initPotion(PH);
					else if (temp == '4')
						potions[numPotions].initPotion(WA);
					else
						potions[numPotions].initPotion(WD);
					numPotions++;
				}
				else if (temp >= '6' && temp <= '9') // Gold
				{
					golds[numGolds].initLoc(p);
					golds[numGolds].setGuarded(false);
					if (temp == '6') // normal gold
						golds[numGolds].initValue(NORMAL);
					else if (temp == '7') // small hoard
						golds[numGolds].initValue(SMALL_HOARD);
					else if (temp == '8') // merchant hoard
						golds[numGolds].initValue(MERCHANT_HOARD);
					else // dragon hoard
					{
						golds[numGolds].initValue(DRAGON_HOARD);
						golds[numGolds].setGuarded(true);
						// We'll figure out which dragon is guarding it later in the method
					}
					numGolds++;
				}
				else if (temp >= 'A' && temp <= 'Z' && temp != 'D') // Any non-dragon Enemy
				{													// (any letter but 'D'
					enemies[numEnemies].initLoc(p);
					if (temp == 'W')      // werewolf 2/9
						enemies[numEnemies].setRace(WEREWOLF);
					else if (temp == 'V') // vampire 3/18
						enemies[numEnemies].setRace(VAMPIRE);
					else if (temp == 'N') // goblin 5/18
						enemies[numEnemies].setRace(GOBLIN);
					else if (temp == 'T') // troll 1/9
						enemies[numEnemies].setRace(TROLL);
					else if (temp == 'X') // phoenix 1/9
						enemies[numEnemies].setRace(PHOENIX);
					else                  // merchant 1/9
						enemies[numEnemies].setRace(MERCHANT);
					numEnemies++;
				}
			}
		}
		// At this point, all objects but dragons have been accounted for
		// Need to add dragons and associate them with hoards
		posn q; // Temporarily stores locations adjacent to p
		posn r; // Temporarily stores locations adjacent to q
		int numAdj;
		bool possible[numDirections];
		int threshold = 1; // Largest number of adjacent hoards per dragon that we look at
		int choice;
		int counter;
		bool dragonSpawned; // Used to break out of the loop whenever a new dragon is spawned
		int leastNeighbours;
		int numNeighbours;
		int matchingHoards; // Counts hoards that tie for least number of neighbours
		
		// Threshold is incremented whenever we don't find a dragon with than many neighbours
		// And decremented whenever we find and spawn such a dragon
		// (because finding a dragon changes the board and can affect other dragons' claims)
		// In this way, dragons with fewer available hoards are satisfied first.
		
		while (numEnemies < enemiesSpawned) // Loop while there are still unspawned dragons
		{
			dragonSpawned = false;
			for (int i = 0; i < HEIGHT; i++)
			{
				for (int j = 0; j < WIDTH; j++)
				{
					if (d.getChar(i, j) == 'D') // If we find a dragon
					{
						p.row = i, p.col = j;
						// Need to make sure it fits the threshold
						// Nothing LESS than threshold should exist!
						// Nothing MORE than threshold should be considered!
						numAdj = 0;
						for (int k = 0; k < numDirections; k++)
							possible[k] = false;
						for (int k = 0; k < numDirections; k++)
						{
							q = p.dirAdjacent(k); // Stores adjacent location
							if (d.getChar(q.row, q.col) == '9') // If location is a hoard
							{									// and is UNCLAIMED
								possible[k] = true;
								numAdj++;
							}
						}
						if (numAdj != threshold)
							continue; // If dragon doesn't match threshold, skip it for now
						// Now that we've made sure the dragon matches the threshold,
						// randomly pick an adjacent hoard from the hoards that have the LEAST
						// number of neighbours!
						leastNeighbours = 1;
						while (true) // Until it finds a hoard with that many neighbours
						{
							matchingHoards = 0;
							for (int k = 0; k < numDirections; k++)
							{
								if (!possible[k])
									continue;	// Skip anything but unclaimed hoards
								q = p.dirAdjacent(k);
								numNeighbours = 0;
								for (int l = 0; l < numDirections; l++)
								{
									r = q.dirAdjacent(l);
									if (d.getChar(r.row, r.col) == 'D')
										numNeighbours++;
								}
								if (numNeighbours == leastNeighbours)
								{
									matchingHoards++;
								}
							}
							if (matchingHoards > 0)
								break;
							leastNeighbours++;
						}
						// Now that we've determined leastNeighbours and matchingHoards,
						// determine which exact hoards have this number of neighbours
						for (int k = 0; k < numDirections; k++)
						{
							if (!possible[k])
								continue;	// Skip anything but unclaimed hoards
							q = p.dirAdjacent(k);
							numNeighbours = 0;
							for (int l = 0; l < numDirections; l++)
							{
								r = q.dirAdjacent(l);
								if (d.getChar(r.row, r.col) == 'D')
									numNeighbours++;
							}
							if (numNeighbours != leastNeighbours)
							{
								possible[k] = false; // Eliminates hoards that have more
								// than a minimum number of competing
								// dragons
							}
						}
						
						// We are guaranteed that 0 < matchingHoards <= numAdj
						choice = rand() % matchingHoards;
						counter = 0;
						for (int k = 0; k < numDirections; k++)
						{
							if (possible[k] && counter == choice)
							{									// Actually create the dragon
								q = p.dirAdjacent(k);
								enemies[numEnemies].initLoc(p);
								enemies[numEnemies].setRace(DRAGON);
								enemies[numEnemies].initHoard(q);
								d.setChar(q.row, q.col, 'G'); // Chosen hoard is now CLAIMED
								d.setChar(p.row, p.col, 'E'); // Dragon is now SATISFIED
								// (This second line hides a dragon from double assignment)
								numEnemies++;
								if (threshold > 1)
									threshold--; // ALWAYS decrement in case this dragon's claim
								// reduced number of options for other dragons
								// However, threshold > 0 is always true
								dragonSpawned = true;
								cout << "Dragon spawned at: " << p.row << ", " << p.col << endl;
								break;
							}
							if (possible[k])
								counter++;
						}
						break;
					}
				}
				if (dragonSpawned)
					break;
			}
			if (!dragonSpawned) // If no dragons were spawned, threshold was too low
				threshold++;
		}
		d.fixCharacters();
	}
	sortEnemies();
}
void Floor::round()
{
	bool turn = playerTurn();
	if(player.getName() == "Ninja")
		print();
	if (turn && (player.getName() != "Ninja" || playerTurn())) // If player did not go up a floor, move the enemies
	{
		
		sortEnemies();
		for (int i = 0; i < numEnemies; i++)
			moveEnemy(&enemies[i]);
	}
	print();
}
void Floor::print()
{
	cout << d;
	stringstream s;
	s << "Race: " << player.getName() << " Gold: " << player.getGold();
	cout << s.str();
	int num = s.str().length();
	for (int i = num; i < 69; i++)
		cout << " ";
	cout << "Floor " << floorNum << endl;
	cout << "HP: " << player.getHP() << endl;
	cout << "Atk: " << player.getAtk() << endl;
	cout << "Def: " << player.getDef() << endl;
	cout << "Action: " << player.getMessage() << endl;
	player.setMessage("");
}
void Floor::nextFloor()
{
	// stores player HP and gold for later
	playerHP = player.getHP();
	playerGold = player.getGold();
	// resets all objects before spawn
	player = Player();
	stairs = Stairs();
	int i;
	for (i = 0; i < potionsSpawned; i++)
		potions[i] = Potion();
	for (i = 0; i < goldsSpawned + enemiesSpawned; i++)
		golds[i] = Gold();
	for (i = 0; i < enemiesSpawned; i++)
		enemies[i] = Enemy();
	floorNum++;
	// Empty chambers ONLY IF CHAMBERS WERE USED (ie. when not reading from file)
	if (fileName[0] == '/')
	{
		d.emptyChambers();
	}
	if (floorNum <= 8)
		init();
}
void Floor::endGame()
{
	if(floorNum > 8)
	{
		int score = player.getGold();
		if (player.getName() == "Human")
		{
			score += score / 2;
		}
		cout << "You Win! Your final score is " << score << ". You can either play again, or quit" << endl;
	}
	else if (player.getHP() <= 0)
	{
		cout << "You are dead. You can either play again, or quit" << endl;
	}
}
int Floor::getFloorNum()
{
	return floorNum;
}
bool Floor::shouldQuit()
{
	return quit;
}
bool Floor::shouldRestart()
{
	return restart;
}
