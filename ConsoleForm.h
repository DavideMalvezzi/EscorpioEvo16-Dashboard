// ConsoleForm.h

#ifndef _CONSOLEFORM_h
#define _CONSOLEFORM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/**
* Class representing the first form shown when the lcd is boot
*/


#include "DisplayInterface.h"
#include "LCDForm.h"

//Console text max size
#define TEXT_BUFFER_SIZE	256

//Console text max cols
#define TEXT_COLS			58

//Console text max rows
#define TEXT_ROWS			22

//Console text string widget index
#define CONSOLE_STR			0

class ConsoleFormClass : public LCDForm{

public:
	//LCDForm abstract method implementation
	void init(Genie &genie);

	//LCDForm abstract method implementation
	void update(Genie &genie);
	
	//Function to print on the console text widget
	void print(String str);
	
	//Function to print on the console text widget
	void println(String str);
	
	//Function to clear the console text widget
	void clear();

	//LCDForm abstract method implementation
	int getFormIndex(){ return 0; }

private:
	//Genie instance pointer
	Genie* genie;

	//Text buffer
	String text;

	//Function to add text to the buffer
	void addText(String str);

};

//Console form instance
extern ConsoleFormClass consoleForm;


#endif

