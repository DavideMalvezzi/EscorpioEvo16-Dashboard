
#include "CanInterface.h"


void CanInterfaceClass::init(int canSpeed, unsigned short minID, unsigned short maxID){
	Can0.begin(canSpeed);
	Can0.watchForRange(minID, maxID);
	
	debugSerial = NULL;
	canEvent = NULL;
	rxBuffer.resize(DEBUG_RX_BUFFER_SIZE);
}

void CanInterfaceClass::update(){
	if (Can0.available()){
		Can0.read(frame);

		if (canEvent != NULL){
			canEvent(frame);
		}

		writePacketOnDebugSerial(frame);
	}

	if (debugSerial != NULL){
		readFromDebugSerial();
		parseSerialDebugCmd();
	}
}


//R/W
int CanInterfaceClass::available(){
	return Can0.available();
}

CAN_FRAME& CanInterfaceClass::read(){
	//Return next available can frame
	if (Can0.available()){
		Can0.read(frame);

		//Echo
		writePacketOnDebugSerial(frame);	
	}
	else{
		//Invalid frame if nothing available
		frame.id = 0;
		frame.length = 0;
	}
	return frame;
}

void CanInterfaceClass::send(CanID::IDs id, byte* data, byte size){
	//Send frame over can
	frame.id = id;
	frame.length = size;
	memcpy(frame.data.bytes, data, size);
	Can0.sendFrame(frame);

	//Echo
	writePacketOnDebugSerial(frame);	
}


//Event
void CanInterfaceClass::setCanEventCallBack(CanEventHandler cb){
	this->canEvent = cb;
}

void CanInterfaceClass::removeCanEventCallBack(){
	this->canEvent = NULL;
}


//Debug
void CanInterfaceClass::setCanDebugSerial(Stream* debugSerial){
	//Attach interrupt for Can Debug
	this->debugSerial = debugSerial;
}

void CanInterfaceClass::writePacketOnDebugSerial(CAN_FRAME& packet){
	if (debugSerial != NULL){
		debugSerial->print(CAD_MSG_HEADER);
		debugSerial->write(packet.id >> 8);
		debugSerial->write(packet.id);
		debugSerial->write(packet.length);
		debugSerial->write((byte*)packet.data.bytes, packet.length);
	}
}

void CanInterfaceClass::readFromDebugSerial(){
	//Read from debug Serial
	if (debugSerial->available()){
		while (debugSerial->available() && rxBuffer.getSize() < rxBuffer.getCapacity()){
			rxBuffer.append(debugSerial->read());
		}
	}
}

void CanInterfaceClass::parseSerialDebugCmd(){
	//Parse debug command
	int index = rxBuffer.indexOf(CAN_MSG_HEADER);
	unsigned short id, size;

	//CMD Packet ->  CMD + Size + Command + Data	Size = sizeof(Command) + sizeof(data)
	if (index != -1){
		index += sizeof(CAN_MSG_HEADER) - 1;

		if (rxBuffer.getSize() > index + 3){
			id = ((rxBuffer[index] & 0xFF) << 8) | (rxBuffer[index + 1] & 0xFF);
			size = rxBuffer[index + 2] & 0xFF;
			index += 3;

			if (rxBuffer.getSize() >= index + size){
				send((CanID::IDs)id, &rxBuffer[index], size);
				
				//test
				frame.id = id;
				frame.length = size;
				memcpy(frame.data.bytes, &rxBuffer[index], size);
				canEvent(frame);
				//////////////

				rxBuffer.remove(0, index + size);
			}
		}
	}
}


//Stream
CanStreamResult CanInterfaceClass::streamOverCan(CanID::IDs canID, const char* openStreamCmd, byte* buffer, int size){
	byte ack;
	int r, q;
	Timer timeOut;
	CAN_FRAME frame;

	//Open the stream over can
	canInterface.send(canID, (byte*)openStreamCmd, strlen(openStreamCmd));

	//Needed packets to send
	q = size / 8;
	r = size % 8;
	ack = getAck(buffer, size);

	//LOGLN(size);
	//LOG_ARR(buffer, size, HEX);

	//Send packets
	for (int i = 0; i < q; i++){
		canInterface.send(canID, buffer + i * 8, 8);
		delay(PACKET_DELAY);
	}

	//Send remaining bytes
	if (r != 0){
		canInterface.send(canID, buffer + q * 8, r);
		delay(PACKET_DELAY);
	}

	//Send ack
	canInterface.send(canID, &ack, 1);

	//Init timeout timer
	timeOut.setDuration(CMD_TIMEOUT).start();

	while (!timeOut.hasFinished()){
		//Read all response frame
		frame = canInterface.read();
		//If the packet has the stream id
		if (frame.id == canID){
			//if response is OK
			if (strcmp((const char*)frame.data.bytes, OK_CMD) == 0){
				return SUCCES;
			}
			//if response is ERROR
			else if (strcmp((const char*)frame.data.bytes, ERROR_CMD) == 0){
				return ERROR;
			}
		}
	}

	return TIMEOUT;
}

CanStreamResult CanInterfaceClass::waitForStreamOverCan(CanID::IDs canID, const char* openStreamCmd, byte* buffer, int expectedBytes){
	byte ack;
	int memIndex;
	Timer timeOut;
	CAN_FRAME frame;

	//Open the stream over can
	canInterface.send(canID, (byte*)openStreamCmd, strlen(openStreamCmd));

	//Set timeout timer
	timeOut.setDuration(CMD_TIMEOUT).start();

	memIndex = 0;
	//Wait for can packet
	while (!timeOut.hasFinished() && memIndex < expectedBytes){
		frame = canInterface.read();
		//Read responses
		if (frame.id == canID){
			//On error
			if (strcmp((const char*)frame.data.bytes, ERROR_CMD) == 0){
				return ERROR;
			}
			//Save in buffer the data
			memcpy(
				buffer + memIndex,
				frame.data.bytes,
				memIndex + frame.length > expectedBytes ? expectedBytes - memIndex : frame.length	//Buffer over-run check
			);
			memIndex += frame.length;
		}
	}

	//Timeout
	if (memIndex < expectedBytes){
		return TIMEOUT;
	}
	//Wait for ack
	while (!timeOut.hasFinished()){
		frame = canInterface.read();
		if (frame.id == canID && frame.length == 1){
			ack = frame.data.bytes[0];
			if (ack != getAck(buffer, expectedBytes)){
				//LOGLN((int)ack);
				//LOGLN((int)getAck(buffer, expectedBytes))
				return WRONG_ACK;
			}
			return SUCCES;
		}
	}

	return TIMEOUT;
}



CanInterfaceClass canInterface;

