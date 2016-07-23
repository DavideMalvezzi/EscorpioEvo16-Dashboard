
#include "MotorDriverLoader.h"

boolean MotorDriverLoaderClass::initFromEEPROM(){
	
	//Load motor from EEPROM
	EEPROM.get<Motor>(EEPROM_MOTOR_CFG, motor);
	motorValid = EEPROM.read(EEPROM_MOTOR_ACK) == getAck((byte*)&motor, sizeof(Motor));
	
	//Load mapset from EEPROM
	EEPROM.get<MotorMap[MAPS_PER_SET]>(EEPROM_MAP_CFG, map);
	mapSetValid = EEPROM.read(EEPROM_MAP_ACK) == getAck((byte*)map, sizeof(MotorMap)* MAPS_PER_SET);

	return motorValid && mapSetValid;
}

void MotorDriverLoaderClass::onCanRequest(CanFrame* frame){
	if (strstr_P((const char*)frame->data, GET_MOTOR_DATA_CMD) != 0){
		LOGLN("GETMOT");
		onGetMotorRequest();
	}
	else if (strstr_P((const char*)frame->data, GET_MAP_DATA_CMD) != 0){
		LOGLN("GETMAP");
		onGetMapRequest();
	}
	else if (strstr_P((const char*)frame->data, SET_MOT_DATA_CMD) != 0){
		LOGLN("SETMOT");
		onSetMotorRequest();
	}
	else if (strstr_P((const char*)frame->data, SET_MAP_DATA_CMD) != 0){
		LOGLN("SETMAP");
		onSetMapRequest();
	}

}

void MotorDriverLoaderClass::onGetMotorRequest(){
	char response[4];

	if (motorValid){
		LOGLN("Motor valid");
		streamOverCan(CAN_DRIVER_CMD, (byte*)&motor, sizeof(Motor));
	}
	else{
		LOGLN("Motor invalid");
		strcpy_P(response, ERROR_CMD);
		canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)response, sizeof(response));
	}
}

void MotorDriverLoaderClass::onGetMapRequest(){
	char response[4];

	if (mapSetValid){
		LOGLN("Map valid");
		streamOverCan(CAN_DRIVER_CMD, (byte*)&map, sizeof(MotorMap) * MAPS_PER_SET);
	}
	else{
		LOGLN("Map invalid");
		strcpy_P(response, ERROR_CMD);
		canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)response, sizeof(response));
	}
}

void MotorDriverLoaderClass::onSetMotorRequest(){
	char response[4];
	Motor temp;
	StreamResult sResult = waitForStreamOverCan(CAN_DRIVER_CMD, (byte*)&temp, sizeof(Motor));

	switch (sResult){
		case SUCCES:
			memcpy(&motor, &temp, sizeof(Motor));
			strcpy_P(response, OK_CMD);

			EEPROM.put<Motor>(EEPROM_MOTOR_CFG, motor);
			EEPROM.write(EEPROM_MOTOR_ACK, getAck((byte*)&motor, sizeof(Motor)));

			LOGLN("Set mot");
			LOGLN(motor.name);
			break;

		case ERROR:
			LOGLN("Error");
		case TIMEOUT:
			LOGLN("Timeout");
		case WRONG_ACK:
			LOGLN("ACK");
			strcpy_P(response, ERROR_CMD);
			break;
	}

	canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)response, sizeof(response));
}

void MotorDriverLoaderClass::onSetMapRequest(){
	char response[4];
	MotorMap temp[MAPS_PER_SET];
	StreamResult sResult = waitForStreamOverCan(CAN_DRIVER_CMD, (byte*)&temp, sizeof(MotorMap) * MAPS_PER_SET);

	switch (sResult){
	case SUCCES:
		memcpy(&map, &temp, sizeof(MotorMap)* MAPS_PER_SET);
		strcpy_P(response, OK_CMD);

		EEPROM.put<MotorMap[MAPS_PER_SET]>(EEPROM_MAP_CFG, map);
		EEPROM.write(EEPROM_MAP_ACK, getAck((byte*)map, sizeof(MotorMap)* MAPS_PER_SET));
		//LOGLN("Set map");
		//LOGLN(map[0].name);
		//LOGLN(map[1].name);
		//LOGLN(map[2].name);
		//LOGLN(map[3].name);

		break;

	case ERROR:
	case TIMEOUT:
	case WRONG_ACK:
		strcpy_P(response, ERROR_CMD);
		break;
	}

	canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)response, sizeof(response));


}



//Rx/Tx
StreamResult MotorDriverLoaderClass::streamOverCan(unsigned short canID, byte* buffer, int size){
	byte ack;
	int r, q;
	Timer timeOut;
	CanFrame frame;

	//Needed packets to send
	q = size / 8;
	r = size % 8;
	ack = getAck(buffer, size);

	//Send packets
	for (int i = 0; i < q; i++){
		canInterface.sendFrame(canID, buffer + 8 * i, 8);
		delay(PACKET_DELAY);
	}

	//Send remaining bytes
	if (r != 0){
		canInterface.sendFrame(canID, buffer + 8 * q, r);
		delay(PACKET_DELAY);
	}

	//Send ack
	canInterface.sendFrame(canID, &ack, 1);

	return SUCCES;
}

StreamResult MotorDriverLoaderClass::waitForStreamOverCan(unsigned short canID, byte* buffer, int expectedBytes){
	byte ack;
	int memIndex;
	Timer timeOut;
	CanFrame frame;

	//Set timeout timer
	timeOut.setDuration(CMD_TIMEOUT).start();

	memIndex = 0;
	//Wait for can packet
	while (!timeOut.hasFinished() && memIndex < expectedBytes){
		if (canInterface.available()){
			canInterface.readFrame(&frame);
			//Read responses
			if (frame.ID == CAN_DRIVER_CMD){
				//On error
				//LOG_ARR(frame.data, frame.size, HEX);
				if (strcmp_P((const char*)frame.data, ERROR_CMD) == 0){
					return ERROR;
				}
				//Save in buffer the data
				memcpy(
					buffer + memIndex,
					frame.data,
					memIndex + frame.size > expectedBytes ? expectedBytes - memIndex : frame.size	//Buffer over-run check
					);
				memIndex += frame.size;
			}
		}
	}

	//Timeout
	if (memIndex < expectedBytes){
		//LOGLN("TIMEOUT1");
		return TIMEOUT;
	}
	//Wait for ack
	while (!timeOut.hasFinished()){
		canInterface.readFrame(&frame);
		if (frame.ID == CAN_DRIVER_CMD && frame.size == 1){
			ack = frame.data[0];
			if (ack != getAck(buffer, expectedBytes)){
				//LOGLN("ACK");
				//LOGLN((int)ack);
				//LOGLN((int)getAck(buffer, expectedBytes));
				return WRONG_ACK;
			}
			//LOGLN("SUCCES");
			return SUCCES;
		}
	}
	//LOGLN("TIMEOUT2");
	return TIMEOUT;
}

byte MotorDriverLoaderClass::getAck(byte* data, int size){
	byte ack = 0;
	for (int i = 0; i < size; i++){
		ack ^= data[i];
	}
	return ack;
}

MotorDriverLoaderClass driverLoader;

