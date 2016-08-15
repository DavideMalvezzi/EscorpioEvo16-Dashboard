// DisplayInterface.h

#ifndef _DISPLAYINTERFACE_h
#define _DISPLAYINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/**
* This class represent a generic interface to handle different form on the lcd screen
*/

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

//Logger tag
#define LCD_TAG			F("LCD")	

//Refresh per second
#define REFRESH_RATEO	3

//Max time allowed to refresh a form
#define MAX_UPDATE_TIME	1500

class DisplayInterfaceClass{

public:
	//Init function to call inside the setup
	void init();

	//Update function to call inside the loop
	void update();

	//Change the current form
	void setCurrentForm(LCDForm* currentForm);

	//Return the current form
	const LCDForm* getCurrentForm(){ return currentForm; }

	//Function to handle the physical button to change the form
	friend void onChangeFormButtonPress(void* data = NULL);
	
	//Function to handle the incoming event from the lcd
	friend void onEvent();

private:
	//Indicates if the form refresh is enabled
	boolean enabled;

	//Genie library instance
	Genie genie;

	//Refresh form timer
	Timer refreshTimer;

	//Current form instance pointer
	LCDForm* currentForm;

};

//Display interface instance
extern DisplayInterfaceClass displayInterface;

#endif

