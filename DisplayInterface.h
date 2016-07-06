// DisplayInterface.h

#ifndef _DISPLAYINTERFACE_h
#define _DISPLAYINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Button.h>
#include <genieArduino.h>
#include <Timer.h>
#include <Vector.h>

#include "ConsoleForm.h"
#include "DebugForm.h"
#include "LCDForm.h"
#include "HWConfig.h"
#include "MainForm.h"
#include "MapsForm.h"

#define LCD_TAG		F("LCD")	
#define REFRESH_RATEO	5

class DisplayInterfaceClass{

public:
	void init();
	void update();
	void setCurrentForm(LCDForm* currentForm);
	const LCDForm* getCurrentForm(){ return currentForm; }


	friend void onChangeFormButtonPress(void* data = NULL);
	friend void onEvent();

private:
	Genie genie;
	Timer refreshTimer;
	LCDForm* currentForm;

};

extern DisplayInterfaceClass displayInterface;

#endif

