// DisplayInterface.h

#ifndef _DISPLAYINTERFACE_h
#define _DISPLAYINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <genieArduino.h>
#include <Timer.h>
#include <Vector.h>

#define LCDSerial		Serial1
#define LCD_SERIAL_BAUD	9600
#define REFRESH_RATEO	10

#define RESET_PIN				6
#define MAX_FORM_NUMBER			3
#define CHANGE_FORM_BUTTON_PIN	3


class DisplayForm{

public:
	virtual void init(Genie& genie) = 0;
	virtual void update(Genie& genie) = 0;
	virtual void onEvent(Genie& genie, genieFrame& evt){}
	virtual void onEnter(Genie& genie){}
	virtual void onExit(Genie& genie){}
	virtual int getFormIndex();
};

class DisplayInterfaceClass{

public:
	void init();
	void update();
	void nextForm();

	friend void onChangeFormButtonPress(void* data = NULL);
	friend void onEvent();


private:
	short currentForm;
	Genie genie;
	Timer refreshTimer;
	Vector<DisplayForm*> forms;

};

extern DisplayInterfaceClass displayInterface;

#endif

