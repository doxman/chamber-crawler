#ifndef __DISPLAY_H__
#define __DISPLAY_H__
#include <iostream>

const int WIDTH = 79;
const int HEIGHT = 25;

class Display{
	char **display;
public:
	Display();
	~Display();
	char **getDisplay();
	void setChar(int row, int col, char c);
	char *getChar(int row, int col);
	friend std::istream &operator>>(std::istream &in, Display &d);
	friend std::ostream &operator<<(std::ostream &out, const Display &d);
};


#endif
