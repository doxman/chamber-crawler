#include "floor.h"
#include <fstream>
#include <cstdlib>
#include <sstream>
using namespace std;

// PRIVATE METHODS
void Floor::flood(char floodChar, char grid[][WIDTH], int r, int c, int chamberNum)
{
	posn temp = {r, c};
	chambers[chamberNum].tiles.push_back(temp);
	grid[r][c] = floodChar;
	if (r > 0 && grid[r-1][c]==FLOOR)
		flood(floodChar, grid, r-1, c, chamberNum);
	if (r < (HEIGHT - 1) && grid[r+1][c]==FLOOR)
		flood(floodChar, grid, r+1, c, chamberNum);
	if (c > 0 && grid[r][c-1]==FLOOR)
		flood(floodChar, grid, r, c-1, chamberNum);
	if (c < (WIDTH - 1) && grid[r][c+1]==FLOOR)
		flood(floodChar, grid, r, c+1, chamberNum);
}
void Floor::floodGrid (char grid[][WIDTH])
{
	char floodChar = 'a';
	int chamberNum = floodChar - 'a';
	for (int r = 0; r < HEIGHT; r++)
	{
		for (int c = 0; c < WIDTH; c++)
		{
			if (grid[r][c] == FLOOR)
			{
				flood(floodChar, grid, r,c, chamberNum);
				floodChar++;
				chamberNum++;
			}
		}
	}
}
void Floor::unfloodGrid(char grid[][WIDTH])
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
	int numTiles = chambers[choice].tiles.size();
	int tileNumber = rand() % numTiles;
	posn temp = chambers[choice].tiles[tileNumber];
	chambers[choice].tiles.erase(chambers[choice].tiles.begin() + tileNumber);
	if (c == PLAYER)
	{
		player.initLoc(temp);
		player.initRace(playerRace);
		player.initGold(playerGold);
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
		choice = rand() % 14; //Increased from 8 to make more dragons
		if (choice < 5)
		{
			golds[numGolds].initValue(NORMAL);
		}
		else if (choice < 7)
		{
			golds[numGolds].initValue(SMALL_HOARD);
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
						posn p;
						if (i == NORTH)
							p.row = temp.row - 1, p.col = temp.col;
						else if (i == NORTHWEST)
							p.row = temp.row - 1, p.col = temp.col - 1;
						else if (i == NORTHEAST)
							p.row = temp.row - 1, p.col = temp.col + 1;
						else if (i == WEST)
							p.row = temp.row, p.col = temp.col - 1;
						else if (i == EAST)
							p.row = temp.row, p.col = temp.col + 1;
						else if (i == SOUTHWEST)
							p.row = temp.row + 1, p.col = temp.col - 1;
						else if (i == SOUTHEAST)
							p.row = temp.row + 1, p.col = temp.col + 1;
						else
							p.row = temp.row + 1, p.col = temp.col;
						enemies[numEnemies].initLoc(p);
						enemies[numEnemies].initRace(DRAGON);
						enemies[numEnemies].initHoard(temp);
						d.setChar(temp.row, temp.col, '$');
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
		choice = rand() % 18;
		if (choice < 4) // werewolf 2/9
			enemies[numEnemies].initRace(WEREWOLF);
		else if (choice < 7) // vampire 3/18
			enemies[numEnemies].initRace(VAMPIRE);
		else if (choice < 12) // goblin 5/18
			enemies[numEnemies].initRace(GOBLIN);
		else if (choice < 14) // troll 1/9
			enemies[numEnemies].initRace(TROLL);
		else if (choice < 16) // phoenix 1/9
			enemies[numEnemies].initRace(PHOENIX);
		else 				  // merchant 1/9
			enemies[numEnemies].initRace(MERCHANT);
		d.setChar(temp.row, temp.col, enemies[numEnemies].getObjectChar());
		numEnemies++;
	}
}

bool Floor::tryMove(int dir)
{
	posn current = player.getLoc();
	int row;
	int col;
	if (dir == NORTHWEST)
		row = current.row - 1, col = current.col - 1;
	else if (dir == NORTH)
		row = current.row - 1, col = current.col;
	else if (dir == NORTHEAST)
		row = current.row - 1, col = current.col + 1;
	else if (dir == WEST)
		row = current.row, col = current.col - 1;
	else if (dir == EAST)
		row = current.row, col = current.col + 1;
	else if (dir == SOUTHWEST)
		row = current.row + 1, col = current.col - 1;
	else if (dir == SOUTH)
		row = current.row + 1, col = current.col;
	else // southeast
		row = current.row + 1, col = current.col + 1;
	if (d.getChar(row, col) == STAIRS) // go to next floor immediately
	{
		nextFloor();
		return false;
	}
	if (d.getChar(row, col) == GOLD || d.getChar(row, col) == '$')
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
			//cout << "Guarded gold has value " << golds[goldNum].getValue() << endl;
			player.setMessage(player.getMessage() + "Dragon scares PC away from their hoard. ");
		}
		// Add code for dragon hoards (value 6) later
	}
	// Can move if tile is floor, passage, or door (after gold is collected)
	char target = d.getChar(row, col);
	if (target == FLOOR || target == '#' || target == '+')
	{
		d.setChar(player.getLoc().row, player.getLoc().col, player.getTileChar());
		player.setTileChar(target);
		d.setChar(row, col, PLAYER);
		player.move(dir);
	}
	else // insulting message?
	{
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
			d.setChar(checkRow, checkCol, FLOOR);
			// delete used potion from array
			numPotions--;
			for (potNum = potNum; potNum < numPotions; potNum++)
				potions[potNum] = potions[potNum + 1];
		}
		else // Insulting message?
		{
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
		char c = d.getChar(checkRow, checkCol);
		if (c == 'V' || c == 'W' || c == 'T' || c == 'N' || c == 'X' || c == 'M' || c == 'D')
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
					golds[numGolds].setLoc(p);
					golds[numGolds].initValue(MERCHANT_HOARD);
					d.setChar(checkRow, checkCol, GOLD);
					numGolds++;
				}
				else
				{
					golds[numGolds].setLoc(p);
					golds[numGolds].initValue(NORMAL);
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
		}
		
	}
	else if (temp == "r") // Restart code here
	{
		restart = true;
	}
	else if (temp == "q") // Quit code here
	{
		quit = true;
	}
	else if (temp == "oxmanly")
	{
		player.oxmanly();
	}
	else if (temp == "grosslyoverpowered")
	{
		player.grosslyOverpowered();
	}
	else // Catch invalid strings
	{
	}
	return true;
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
	int counter = 0; // Goes through the 8 directions controlled by the array
	for (int i = loc.row - 1; i <= loc.row + 1; i++)
	{
		for (int j = loc.col - 1; j <= loc.col + 1; j++)
		{
			if (i == loc.row && j == loc.col)
				continue;
			if (d.getChar(i, j) == FLOOR)
			{
				open[counter] = true;
				if (e->getRace()!= "Dragon") {
					availableMoves++; //Only can move if not a dragon
				}
			}
			if (d.getChar(i, j) == PLAYER)
			{
				if ((angryMerchants || e->getRace()!= "Merchant")&&
					(e->getRace()!= "Dragon" || ((abs(e->hoardLoc().row - i) <= 1)&&
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
			bool lethal = player.getAttacked(*e);
			if(lethal)
			{
				restart = true;
			}
		}
		return;
	}
	d.setChar(loc.row, loc.col, FLOOR); // Clears the tile the enemy is leaving
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
	loc = e->getLoc(); // New position
	d.setChar(loc.row, loc.col, e->getObjectChar()); // Draws the enemy's new position
}

// PUBLIC METHODS
Floor::Floor(char pR)
{
	for (int i = 0; i < numChambers; i++)
		chambers[i].tiles = vector<posn>();
	playerRace = pR;
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
}
void Floor::init()
{
	d = Display();
	playerChamber = -1;
	numPotions = 0;
	numGolds = 0;
	numEnemies = 0;
	char temp [HEIGHT] [WIDTH];
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
			temp[i][j] = d.getChar(i, j);
	}
	floodGrid(temp);
	unfloodGrid(temp);
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
	bool turn = playerTurn();
	if (turn) // If player did not go up a floor, move the enemies
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
	s << "Race: " << player.getRace() << " Gold: " << player.getGold();
	cout << s.str();
	int num = s.str().length();
	for (int i = num; i < 69; i++)
		cout << " ";
	cout << "Floor " << floorNum << endl;
	cout << "HP: " << player.getHP() << endl;
	cout << "Atk: " << player.getAtk() << endl;
	cout << "Def: " << player.getDef() << endl;
	cout << "Action: " << player.getMessage() << endl; // change this to actually display stuff!
	/*
	for (int i = 0; i < numEnemies; i++) {
		cout << enemies[i].getRace() << " at " << enemies[i].getLoc().row << "," << enemies[i].getLoc().col << " is guarding hoard at " << enemies[i].hoardLoc().row << "," << enemies[i].hoardLoc().col << endl;
	}
	for (int i = 0; i < numGolds; i++) {
		string g = "";
		if	(!golds[i].isGuarded())
		{
			g = "not ";
		}
		cout << "Gold at " << golds[i].getLoc().row << "," << golds[i].getLoc().col << " is " << g << "guarded." << endl;
	}//*/

	player.setMessage("");
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
void Floor::nextFloor()
{
	playerHP = player.getHP();
	playerGold = player.getGold();
	player = Player();
	stairs = Stairs();
	int i;
	// resets objects before spawn
	for (i = 0; i < potionsSpawned; i++)
		potions[i] = Potion();
	for (i = 0; i < goldsSpawned; i++)
		golds[i] = Gold();
	for (i = 0; i < enemiesSpawned; i++)
		enemies[i] = Enemy();
	floorNum++;
	// Empty chambers
	for (i = 0; i < numChambers; i++)
	{
		while (chambers[i].tiles.size() > 0)
			chambers[i].tiles.pop_back();
	}
	init();
}
void Floor::endGame()
{
	if(floorNum > 8)
	{
		int score = player.getGold();
		if (player.getRace() == "Human")
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