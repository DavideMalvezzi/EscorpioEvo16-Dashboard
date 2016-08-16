// BMSInterface.h

#ifndef _BMSINTERFACE_h
#define _BMSINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Timer.h>

/**
* Class representing the external interface to the BMS board
*/

#include "CanInterface.h"
#include "HWConfig.h"
#include "Logger.h"

//Logger tag
#define BMS_TAG			F("BMS")

//BMS can bus command to start
#define BMS_START_CMD	0xAA

//BMS can bus command to set debug mode
#define BMS_DEBUG_CMD	0xBB

//BMS state string
#define ENABLED_STATE	"ENABLED"
#define RUN_STATE		"RUN"
#define DEBUG_STATE		"DEBUG"
#define ERROR_STATE		"ERROR"

//BMS button led blinking time
#define BMS_LED_DUR		100

//TTL time for the BMS state
#define BMS_STATE_TTL	1000


class BMSInterface{

public:
	//Enum containig the BMS state
	enum BMSState : byte{
		Unknown,
		Enabled,
		Run,
		Debug,
		Error
	};

	//Function to call in the setup
	void init();

	//Function to call in the loop
	void update();

	//Function to call when a new can bus packet containing the state is received from the BMS
	void onStateChanged(const char* state);

	//Return the current state
	BMSState getState(){ return state; }

	//Send the start command via can bus
	void start();

	//Send the debug mode command via can bus
	void setDebugMode();

	//Handles the BMS button pression 
	friend void onStartButtonPressed(void* data = NULL);

private:
	//BMS led button status
	byte ledStatus;

	//Current BMS status
	BMSState state;

	//BMS led button timer
	Timer ledTimer;
	
	//BMS state TTL timer
	Timer lastStateUpdate;
};

//BMS interface instance
extern BMSInterface BMS;



#endif

