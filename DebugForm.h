// DebugForm.h

#ifndef _DEBUGFORM_h
#define _DEBUGFORM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CanInterface.h"
#include "ChannelsBuffer.h"
#include "DisplayInterface.h"
#include "LCDForm.h"
#include "PhoneInterface.h"

//Widget index
#define DEBUG_ON_BLUETOOTH_BUTTON	0
#define DEBUG_ON_SERIAL_BUTTON		1
#define DEBUG_STOP_BUTTON			2
#define DEBUG_ACTIVE_LED			2

class DebugFormClass : public LCDForm{

public:
	void init(Genie &genie);
	void update(Genie &genie);
	void onEvent(Genie& genie, genieFrame& evt);

	int getFormIndex(){ return 2; }

private:

};

extern DebugFormClass debugForm;


#endif

