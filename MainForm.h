// MainForm.h

#ifndef _MAINFORM_h
#define _MAINFORM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/**
* Class representing the main form the driver will see
*/

#include "CanInterface.h"
#include "ChannelsBuffer.h"
#include "DisplayInterface.h"
#include "Interprete.h"
#include "LCDForm.h"
#include "MapSelector.h"
#include "PhoneInterface.h"
#include "StrategySettings.h"
#include "WheelSensor.h"


//Form widgets indexs
#define DUTY_CICLE_BAR		0
#define IST_SPEED_DIGITS	0
#define AVG_SPEED_DIGITS	1
#define LEFT_TIME_LABEL		2
#define LEFT_TIME_DIGITS	2
#define LAP_DIGITS			3
#define LAST_TIME_DIGITS	4
#define CURR_TIME_DIGITS	5
#define RADIO_LED			0
#define GAS_LED				1
#define GAP_STRING			1
#define CONSUMPTION_STRING	2
#define LAP_SPACE_DIGITS	8
#define	PREF_MAP_DIGITS		9
#define SAFE_MODE_STRING	8
#define POP_UP_IMG			0
#define POP_UP_TTL			700

//Pop up state
#define POP_UP_HIDE			0
#define POP_UP_MAP			1
#define POP_UP_WARN			5


class MainFormClass : public LCDForm{

public:
	//LCDForm abstract method implementation
	void init(Genie &genie);

	//LCDForm abstract method implementation
	void update(Genie &genie);
	
	//LCDForm abstract method implementation
	int getFormIndex(){ return 1; }

	//Function to call when the driver map is changed to show the pop up
	void updateCurrentMap(byte map);

	//Function to call when the driver motor power is changed to show the alert pop up if necessary
	void updateCurrentMotorPower(byte power);

private:
	//Pop up index 
	byte popUpIndex;
	
	//Previous map
	byte prevMap;

	//Pop up show timer
	Timer popUpTimer;

	//Function to show the index-nth pop up for duration ms
	void showPopUp(byte index, unsigned long duration);

	//Update all the widgets values
	void updateWidgetsValues(Genie& genie);

	//Utils function to convert millis to min:sec.
	//Used to show the time on the lcd
	unsigned short convertMillisToMinSec(unsigned long time);

	//Convert the gap values into a string to show it on the lcd
	String getGapString();
};

//Main form instance
extern MainFormClass mainForm;

#endif

