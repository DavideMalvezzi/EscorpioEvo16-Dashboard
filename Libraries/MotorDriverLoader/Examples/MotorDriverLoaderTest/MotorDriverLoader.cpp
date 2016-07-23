
#include "MotorDriverLoader.h"


boolean MotorDriverLoaderClass::initFromEEPROM(){
	EEPROM.get<Motor>(EEPROM_MOTOR_CFG, motor);
	EEPROM.get<MotorMap>(EEPROM_MAP_CFG, map);

	return motor.ack == getAck((byte*)&motor, sizeof(motor) - 1) &&
		map.ack == getAck((byte*)&map, sizeof(map) - 1);
}

void MotorDriverLoaderClass::onCanRequest(CanFrame* frame){
	if (strcmp_P((char*)frame->data, GET_DATA_CMD)){
		onGetRequest();
	}
	else if (strcmp_P((char*)frame->data, SET_MOT_DATA_CMD)){

	}
	else if (strcmp_P((char*)frame->data, SET_MAP_DATA_CMD)){

	}

}

void MotorDriverLoaderClass::onGetRequest(){
	int q, r;

	//Send motor
	q = sizeof(Motor) / 8;
	r = sizeof(Motor) % 8;

	for (int i = 0; i < q; i++){
		canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)(&motor + i * 8), 8);
	}

	if (r){
		canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)(&motor + q * 8), r);
	}

	//Send map
	q = sizeof(MotorMap) / 8;
	r = sizeof(MotorMap) % 8;

	for (int i = 0; i < q; i++){
		canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)(&map + i * 8), 8);
	}

	if (r != 0){
		canInterface.sendFrame(CAN_DRIVER_CMD, (byte*)(&map + q * 8), r);
	}

}


byte MotorDriverLoaderClass::getAck(byte* data, int size){
	byte ack = 0;
	for (int i = 0; i < size; i++){
		ack ^= data[i];
	}
	return ack;
}

MotorDriverLoaderClass driverLoader;

