// LCDStringList.h

#ifndef _LCDSTRINGLIST_h
#define _LCDSTRINGLIST_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/**
* Class to use a string widget as a list of selectable item
*/


#include <genieArduino.h>

#include "Utils.h"

//Char to use as selector
#define SELECT_LINE_CHAR	'>'

class LCDStringList {

public:
	//Function to init the StringList
	//Index is the widget index in ViSi
	//MaxSize is the max number of chars
	void init(byte index, int maxSize, Genie* parent);

	//Move the selector up of one position
	void up();

	//Move the selector down of one position
	void down();

	//Remove all the items
	void clear();

	//Add new string item
	void addElement(String elem);

	//Remove an item by the index
	void removeElement(int index);

	//Get the current element index
	int getCurrentElement();

	//Get the string widget index
	int getWidgetIndex();

	//Get the items count
	int getElementCount();

	//Repaint the list
	void repaint();

private:
	//Current item index
	char currentElement;

	//Widget index in ViSi
	byte widgetIndex;
	
	//Items count
	byte elementCount;

	//String buffer
	String buffer;

	//Genie instance pointer
	Genie* parent;
};


#endif

