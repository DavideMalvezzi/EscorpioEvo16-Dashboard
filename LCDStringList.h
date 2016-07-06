// LCDStringList.h

#ifndef _LCDSTRINGLIST_h
#define _LCDSTRINGLIST_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <genieArduino.h>

#include "Utils.h"

#define SELECT_LINE_CHAR	'>'

class LCDStringList {

public:
	void init(byte index, int maxSize, Genie* parent);
	void up();
	void down();
	void clear();
	void addElement(String elem);
	void removeElement(int index);
	int getCurrentElement();
	int getWidgetIndex();
	int getElementCount();

	void repaint();

private:
	char currentElement;
	byte widgetIndex, elementCount;
	String buffer;
	Genie* parent;
};


#endif

