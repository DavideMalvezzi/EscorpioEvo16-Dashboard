// MainForm.h

#ifndef _MAINFORM_h
#define _MAINFORM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DisplayInterface.h"

#define CAN_PWR			0x0A2	//Power
#define CAN_DC			0x0A5
#define CAN_RADIO		0x209  // flag
#define CAN_RADIO_REQ	0x20C  // flag

class MainFormClass : public DisplayForm{

public:
	void init(Genie &genie);
	void update(Genie &genie);
	int getFormIndex(){ return 0; }

private:

};

extern MainFormClass mainForm;

#endif

