// LCDStringMsg.h

#ifndef _LCDSTRINGMSG_h
#define _LCDSTRINGMSG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <genieArduino.h>

#include "Utils.h"

class LCDStringMsg {

public:
	void init(byte index, Genie* parent);
	void clear();
	const String& getMessage();
	void repaint();
	void setMessage(String str);
	void setMessage(const char* str);

private:
	byte widgetIndex;
	String buffer;
	Genie* parent;
};

#endif

