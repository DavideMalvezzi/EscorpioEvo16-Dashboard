#include "CanInterface.h"
#include "ChannelsBuffer.h"
#include "ConsoleForm.h" 

//#define DEBUG_CAN	1

void CanInterfaceClass::init(int canSpeed, unsigned short minID, unsigned short maxID){
	Can0.begin(canSpeed);
	Can0.watchForRange(minID, maxID);

	LOGLN("CAN_INTERFACE_INIT");
	consoleForm.println("CAN_INTERFACE_INIT");
}

void CanInterfaceClass::readAll(){
	while (Can0.available()){
		Can0.read(frame);
		channelsBuffer.setValue(frame.id, frame.data.bytes, frame.length);

#ifdef DEBUG_CAN
		LOG("Read "); LOG(frame.id); 
		LOG(" size "); LOG(frame.length); 
		LOG(" data "); LOG_ARR(frame.data.bytes, frame.length, HEX);
#endif
	}
}

CAN_FRAME& CanInterfaceClass::read(){
	if (Can0.available()){
		Can0.read(frame);
	}
	else{
		frame.id = 0;
		frame.length = 0;
	}
	return frame;
}

void CanInterfaceClass::send(unsigned short id, byte* data, byte size){
	frame.id = id;
	frame.length = size;
	memcpy(frame.data.bytes, data, size);
	Can0.sendFrame(frame);
}

CanInterfaceClass canInterface;

