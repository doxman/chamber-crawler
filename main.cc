#include "floor.h"
using namespace std;

int main ()
{
	char temp = ' ';
	while (temp != 'h' && temp != 'd' && temp != 'e' && temp != 'o' && temp != 'q')
	{
		cout << "Please choose one of the following races" << endl;
		cout << "Human (h), dwarf (d), elf (e), orc (o)" << endl;
		cout << "Or enter 'q' to quit" << endl;
		cout << "Enter race: ";
		cin >> temp;
	}
	if (temp == 'q') // Exit option
		return 0;
	Floor *f = new Floor(temp);
	f->init();
	f->print();
	while (f->getFloorNum() <= 8)
		f->round();
}
