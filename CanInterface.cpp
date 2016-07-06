
#include "CanInterface.h"


void CanInterfaceClass::init(int canSpeed, unsigned short minID, unsigned short maxID){
	Can0.begin(canSpeed);
	Can0.watchForRange(minID, maxID);
}

CAN_FRAME& CanInterfaceClass::read(){
	//Return next available can frame
	if (Can0.available()){
		Can0.read(frame);
	}
	else{
		//Invalid frame if nothing available
		frame.id = 0;
		frame.length = 0;
	}
	return frame;
}

void CanInterfaceClass::send(unsigned short id, byte* data, byte size){
	//Send frame over can
	frame.id = id;
	frame.length = size;
	memcpy(frame.data.bytes, data, size);
	Can0.sendFrame(frame);
}

CanInterfaceClass canInterface;

