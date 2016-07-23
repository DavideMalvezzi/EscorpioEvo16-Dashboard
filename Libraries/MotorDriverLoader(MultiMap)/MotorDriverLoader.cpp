
#include "MotorDriverLoader.h"

boolean MotorDriverLoaderClass::initFromEEPROM(){
	bool valid = false;
	
	EEPROM.get<Motor>(EEPROM_MOTOR_CFG, motor);
	valid &= (motor.ack == getAck((byte*)&motor, sizeof(Motor) - 1) );
	
	for(int i = 0; i < MAX_MAP_NUM; i++){
		EEPROM.get<MotorMap>(EEPROM_MAP_CFG, map[i]);
		valid &= (map.ack == getAck((byte*)&map, sizeof(MotorMap) - 1));
	}

	return valid;
}

void MotorDriverLoaderClass::onCanRequest(CanFrame* frame){
	if (strstr_P((char*)frame->data, GET_DATA_CMD) != 0){
		onGetRequest();
	}
	else if (strstr_P((char*)frame->data, SET_MOT_DATA_CMD) != 0){
		onSetMotorRequest();
	}
	else if (strstr_P((char*)frame->data, SET_MAP_DATA_CMD) != 0){
		//Get the num byte. Don't need to add +1 to sizeof 'cause \0 at the end of the string 
		onSetMapRequest(frame->data[sizeof(SET_MAP_DATA_CMD)]);
	}

}

void MotorDriverLoaderClass::onGetRequest(){
	int q, r;

	if (initFromEEPROM()){

		//Send motor
		q = sizeof(Motor) / 8;
		r = sizeof(Motor) % 8;

		for (int i = 0; i < q; i++){
			canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)(&motor) + i * 8, 8);
			delay(100);
		}

		if (r != 0){
			canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)(&motor) + q * 8, r);
		}

		//Send map
		q = sizeof(MotorMap) / 8;
		r = sizeof(MotorMap) % 8;

		for (int i = 0; i < q; i++){
			canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)(&map) + i * 8, 8);
			delay(100);
		}

		if (r != 0){
			canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)(&map) + q * 8, r);
		}
	}
	else{
		char response[4];
		int size = strlen_P(INVALID_RX_CMD) + 1;
		memcpy_P(response, INVALID_RX_CMD, size);
		canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)response, size);
	}
}

void MotorDriverLoaderClass::onSetMotorRequest(){
	int memIndex = 0, size;
	char response[4];
	CanFrame frame;
	Timer timeOut;
	Motor tempMotor;
	
	timeOut.setDuration(CMD_TIMEOUT).start();

	while (!timeOut.hasFinished() && memIndex < sizeof(Motor)){
		if (canInterface.available()){
			canInterface.readFrame(&frame);
			if (frame.ID == CAN_DRIVER_CMD){
				memcpy((byte*)(&tempMotor) + memIndex, frame.data, frame.size);
				memIndex += frame.size;
			}
		}
	}

	if (memIndex < sizeof(Motor) || tempMotor.ack != getAck((byte*)&tempMotor, sizeof(Motor) - 1)){
		size = strlen_P(ERROR_RX_CMD) + 1;
		memcpy_P(response, ERROR_RX_CMD, size);
	}
	else{
		size = strlen_P(OK_RX_CMD) + 1;
		memcpy_P(response, OK_RX_CMD, size);

		memcpy(&motor, &tempMotor, sizeof(Motor));
		EEPROM.put<Motor>(EEPROM_MOTOR_CFG, motor);

	}

	canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)response, size);
}

void MotorDriverLoaderClass::onSetMapRequest(char mapNum){
	int memIndex = 0, size;
	char response[4];
	CanFrame frame;
	Timer timeOut;
	MotorMap tempMap;

	timeOut.setDuration(CMD_TIMEOUT).start();

	while (!timeOut.hasFinished() && memIndex < sizeof(MotorMap)){
		if (canInterface.available()){
			canInterface.readFrame(&frame);
			if (frame.ID == CAN_DRIVER_CMD){
				memcpy((byte*)(&tempMap) + memIndex, frame.data, frame.size);
				memIndex += frame.size;
			}
		}
	}

	if (memIndex < sizeof(MotorMap) || tempMap.ack != getAck((byte*)&tempMap, sizeof(MotorMap)-1)){
		size = strlen_P(ERROR_RX_CMD) + 1;
		memcpy_P(response, ERROR_RX_CMD, size);
	}
	else{
		size = strlen_P(OK_RX_CMD) + 1;
		memcpy_P(response, OK_RX_CMD, size);

		memcpy(&map[mapNum], &tempMap, sizeof(MotorMap));
		EEPROM.put<MotorMap>(EEPROM_MAP_CFG, map[mapNum]);
	}

	canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)response, size);

}


byte MotorDriverLoaderClass::getAck(byte* data, int size){
	byte ack = 0;
	for (int i = 0; i < size; i++){
		ack ^= data[i];
	}
	return ack;
}

MotorDriverLoaderClass driverLoader;

