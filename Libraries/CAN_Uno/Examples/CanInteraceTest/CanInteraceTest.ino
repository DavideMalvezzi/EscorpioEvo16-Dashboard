#include "CanInterfaceUno.h"
#include <ByteBuffer.h>
#include <mcp_can.h>
#include <SPI.h>
#include <Utils.h>

#define SERIAL_BAUD		9600
#define CAN_SPEED		CAN_250KBPS

#define CAD_MSG_HEADER	"CAD"	//CAN Analyzer Data
#define CAN_MSG_HEADER	"CAN"	//SEND + CHANNEL + SIZE + DATA

ByteBuffer rxBuffer(32);

void setup() {
	INIT_SERIAL(Serial, SERIAL_BAUD);
	canInterface.init(CAN_SPEED);
}

void loop(){
	CanFrame frame;

	if (canInterface.available()){
		canInterface.readFrame(&frame);
		Serial.print(CAD_MSG_HEADER);
		Serial.write(frame.ID >> 8);
		Serial.write(frame.ID);
		Serial.write(frame.size);
		Serial.write((uint8_t*)frame.data, frame.size);
   
	}

	readFromSerial();
	parseCmd();
	
}

void readFromSerial(){
	if (Serial.available()){
		while (Serial.available()){
			rxBuffer.append(Serial.read());
		}
	}
}

void parseCmd(){
	int i = rxBuffer.indexOf(CAN_MSG_HEADER);
	unsigned short id, size;
	

	//CMD Packet ->  CMD + Size + Command + Data	Size = sizeof(Command) + sizeof(data)
	if (i != -1){
		i += sizeof(CAN_MSG_HEADER) - 1;
		LOGLN("find CAN");

		if (rxBuffer.getSize() > i + 3){
			id = ((rxBuffer[i] & 0xFF) << 8) | (rxBuffer[i + 1] & 0xFF);
			size = rxBuffer[i + 2] & 0xFF;
			i += 3;

			if (rxBuffer.getSize() >= i + size){
				canInterface.sendFrame(id, &rxBuffer[i] ,size);
				rxBuffer.remove(0, i + size);
			}
		}
	}

	
}