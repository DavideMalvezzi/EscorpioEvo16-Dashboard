// MainForm.h

#ifndef _MAINFORM_h
#define _MAINFORM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CanInterface.h"
#include "ChannelsBuffer.h"
#include "DisplayInterface.h"
#include "Interprete.h"
#include "LCDForm.h"
#include "MapSelector.h"
#include "PhoneInterface.h"
#include "StrategySettings.h"
#include "WheelSensor.h"


#define DUTY_CICLE_BAR		0
#define IST_SPEED_DIGITS	0
#define AVG_SPEED_DIGITS	1
#define LEFT_TIME_DIGITS	2
#define LAP_DIGITS			3
#define LAST_TIME_DIGITS	4
#define CURR_TIME_DIGITS	5
#define RADIO_LED			0
#define GAS_LED				1
#define GAP_STRING			1
#define CONSUMPTION_STRING	2
#define SAFE_MODE_STRING	8
#define MAP_SELECTOR_IMG	0
#define MAP_SELECTOR_TTL	700

class MainFormClass : public LCDForm{

public:
	void init(Genie &genie);
	void update(Genie &genie);
	int getFormIndex(){ return 1; }
	void setNewCurrentMap(byte map);

private:
	byte prevMap;
	Timer changedMapTimer;

	void updateWidgetsValues(Genie& genie);
	unsigned short convertMillisToMinSec(unsigned long time);
};

extern MainFormClass mainForm;

#endif

