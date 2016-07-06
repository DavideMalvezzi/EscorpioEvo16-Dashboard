// CanInterface.h

#ifndef _CANINTERFACE_h
#define _CANINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <due_can.h>
	
class CanInterfaceClass {

public:
	void init(int canSpeed, unsigned short minID = 0, unsigned short maxID = 2048);
	void readAll();
	CAN_FRAME& read();
	void send(unsigned short id, byte* data, byte size);

private:
	CAN_FRAME frame;

};

extern CanInterfaceClass canInterface;


#endif

