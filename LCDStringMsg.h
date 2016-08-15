// LCDStringMsg.h

#ifndef _LCDSTRINGMSG_h
#define _LCDSTRINGMSG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/**
* Class to use a string widget to show simpre message
*/

#include <genieArduino.h>

#include "Utils.h"

class LCDStringMsg {

public:
	//Function to init. 
	//Index is the ViSi editor string widget index
	void init(byte index, Genie* parent);

	//Clear the message
	void clear();

	//Get the string message
	const String& getMessage();

	//Repaint the message
	void repaint();

	//Set the message
	void setMessage(String str);
	void setMessage(const char* str);

private:
	//Widget ViSi index
	byte widgetIndex;

	//String message
	String buffer;

	//Genie instance pointer
	Genie* parent;
};

#endif

