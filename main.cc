#include "floor.h"
using namespace std;
bool shinyDLC;
int main (int argc, char *argv[])
{
	char temp = ' ';
	string tempstr = "/"; // This character can't appear in actual filenames
						  // As such, it's used in the case where no filename is given
						  // Essentially, it represents the default floor layout
	char *fileName = const_cast<char*>(tempstr.c_str());
	if (argc > 1)
		fileName = argv[1];	
	Floor *f;
	while (temp != 'n' && temp != 'y')
	{
		cout << "Do you want shiny DLC? y/n" << endl;
		cout << "Enter your choice: ";
		cin >> temp;
	}
	shinyDLC = (temp == 'y');
	temp = ' ';
	while (true) {
		while (temp != 'h' && temp != 'd' && temp != 'e' && temp != 'o' && temp != 'q' && (temp != 'n' || !shinyDLC))
		{
			cout << "Please choose one of the following races" << endl;
			cout << "Human (h), dwarf (d), elf (e), orc (o)" << endl;
			cout << "Or enter 'q' to quit" << endl;
			cout << "Enter race: ";
			cin >> temp;
		}
		if (temp == 'q') // Exit option
			return 0;
		
		f = new Floor(temp, fileName, shinyDLC);
		f->init();
		f->print();
		while (f->getFloorNum() <= 8 && !f->shouldQuit() && !f->shouldRestart())
			f->round();
		if (f->shouldQuit()){
			return 0;
		}
		f->endGame();
		delete f;
		temp = ' ';
	}
}
