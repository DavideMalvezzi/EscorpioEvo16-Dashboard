// Button.h

#ifndef _BUTTON_h
#define _BUTTON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Timer.h"
#include "Vector.h"

#define BOUNCE_TIME		50

class Button {

public:
	static void setMaxNumber(int max);
	static void add(byte pin, void(*pressFunction)(void*) = NULL, void(*releaseFunction)(void*) = NULL, void* data = NULL);
	static void update();
	static unsigned long int getPressTime();


protected:
	static unsigned long int pressTime;
	static Vector<Button*>* activeButtons;

	Button(byte pin, void(*pressFunction)(void*) = NULL, void(*releaseFunction)(void*) = NULL, void* data = NULL);
	
	byte buttonPin;
	byte newState, buttonState;
	Timer pushTimer, pressTimer;

	void(*pressFunction)(void*);
	void(*releaseFunction)(void*);
	void* data;

};

#endif

