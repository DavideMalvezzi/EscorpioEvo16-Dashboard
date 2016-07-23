// ConsoleForm.h

#ifndef _CONSOLEFORM_h
#define _CONSOLEFORM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#define TEXT_BUFFER_SIZE	256
#define TEXT_COLS			58
#define TEXT_ROWS			22
#define CONSOLE_STR			0

#include "DisplayInterface.h"
#include "LCDForm.h"

class ConsoleFormClass : public LCDForm{

public:
	void init(Genie &genie);
	void update(Genie &genie);
	void print(String str);
	void println(String str);
	void clear();
	int getFormIndex(){ return 0; }

private:
	Genie* genie;
	String text;

	void addText(String str);

};

extern ConsoleFormClass consoleForm;


#endif

