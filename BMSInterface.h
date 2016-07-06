// BMSInterface.h

#ifndef _BMSINTERFACE_h
#define _BMSINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Timer.h>

#include "CanInterface.h"
#include "HWConfig.h"
#include "Logger.h"

#define BMS_TAG			F("BMS")
#define BMS_START_CMD	0xAA
#define BMS_DEBUG_CMD	0xBB

#define ENABLED_STATE	"ENABLED"
#define RUN_STATE		"RUN"
#define DEBUG_STATE		"DEBUG"
#define ERROR_STATE		"ERROR"

#define BMS_LED_DUR		100


class BMSInterface{

public:
	enum BMSState : byte{
		Unknown,
		Enabled,
		Run,
		Debug,
		Error
	};

	void init();
	void update();
	void onStateChanged(const char* state);
	BMSState getState(){ return state; }

	void start();
	void setDebugMode();

	friend void onStartButtonPressed(void* data = NULL);

private:
	byte ledStatus;
	BMSState state;
	Timer ledTimer;
};

extern BMSInterface BMS;



#endif

