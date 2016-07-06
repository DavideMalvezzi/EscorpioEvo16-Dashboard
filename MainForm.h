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
#include "PhoneInterface.h"
#include "StrategySettings.h"
#include "WheelSensor.h"


class MainFormClass : public LCDForm{

public:
	void init(Genie &genie);
	void update(Genie &genie);
	int getFormIndex(){ return 1; }

private:

};

extern MainFormClass mainForm;

#endif

