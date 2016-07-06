// DebugForm.h

#ifndef _DEBUGFORM_h
#define _DEBUGFORM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DisplayInterface.h"

#define CAN_BAT		0x410	//Battery voltage
#define CAN_MAP		0x091	//Engine map selected
#define CAN_BMS		0x400	//BMS status
#define CAN_CELL0	0x401	//Cell 0 voltage (le successive di conseguenza)

class DebugFormClass : public DisplayForm{

public:
	void init(Genie &genie);
	void update(Genie &genie);
	int getFormIndex(){ return 1; }

private:

};

extern DebugFormClass debugForm;


#endif

