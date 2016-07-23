// LCDForm.h

#ifndef _LCDFORM_h
#define _LCDFORM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <genieArduino.h>

class LCDForm {

protected:
	void updateWidget(Genie& genie, uint16_t wType, uint16_t wIndex, uint16_t wValue){
		genie.WriteObject(wType, wIndex, wValue);
	}

	void updateString(Genie& genie, uint16_t wIndex, String wValue){
		genie.WriteStr(wIndex, (char*)wValue.c_str());
	}

public:
	virtual void init(Genie& genie) = 0;	//Call it to init the form
	virtual void update(Genie& genie) = 0;	//Call it on loop
	virtual void onEvent(Genie& genie, genieFrame& evt){}	//Override for control events
	virtual void onEnter(Genie& genie){}	//Override for exec function on form activation
	virtual void onExit(Genie& genie){}		//Override for exec function on form deactivation
	virtual int getFormIndex() = 0;		//Get form index in ViSi

	
};


#endif

