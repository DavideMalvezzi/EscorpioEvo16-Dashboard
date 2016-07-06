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
#include "WayPoint.h"


class DebugFormClass : public LCDForm{

public:
	void init(Genie &genie);
	void update(Genie &genie);
	int getFormIndex(){ return 2; }

private:

};

extern DebugFormClass debugForm;


#endif

