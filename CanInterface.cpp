
#include "CanInterface.h"

void onCanPacketCallBack(CAN_FRAME* frame){
	Stream* dSerial = canInterface.debugSerial;

	if (dSerial != NULL){
		dSerial->print(CAD_MSG_HEADER);
		dSerial->write(frame->id >> 8);
		dSerial->write(frame->id);
		dSerial->write(frame->length);
		dSerial->write((byte*)frame->data.bytes, frame->length);
	}
}

void CanInterfaceClass::init(int canSpeed, unsigned short minID, unsigned short maxID){
	Can0.begin(canSpeed);
	Can0.watchForRange(minID, maxID);

	debugSerial = NULL;
	rxBuffer.resize(DEBUG_RX_BUFFER_SIZE);
}

int CanInterfaceClass::available(){
	return Can0.available();
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

void CanInterfaceClass::send(CanID::IDs id, byte* data, byte size){
	//Send frame over can
	frame.id = id;
	frame.length = size;
	memcpy(frame.data.bytes, data, size);
	Can0.sendFrame(frame);
}

void CanInterfaceClass::update(){
	if (debugSerial != NULL){
		readFromDebugSerial();
		parseDebugCmd();
	}
}


//Debug
void CanInterfaceClass::setCanDebugSerialPort(Stream* debugSerial){
	//Attach interrupt for Can Debug
	this->debugSerial = debugSerial;
	if (debugSerial != NULL){
		Can0.attachCANInterrupt(&onCanPacketCallBack);
	}
	else{
		Can0.attachCANInterrupt(NULL);
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

void CanInterfaceClass::parseDebugCmd(){
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
		canInterface.send(canID, buffer + 8 * i, 8);
		delay(PACKET_DELAY);
	}

	//Send remaining bytes
	if (r != 0){
		canInterface.send(canID, buffer + 8 * q, r);
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

byte CanInterfaceClass::getAck(byte* data, int size){
	byte ack = 0;
	for (int i = 0; i < size; i++){
		ack ^= data[i];
	}
	return ack;
}



CanInterfaceClass canInterface;

