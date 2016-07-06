// MotorDriverLoader.h

#ifndef _MOTORDRIVERLOADER_h
#define _MOTORDRIVERLOADER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <CanInterfaceUno.h>
#include <EEPROM.h>
#include <Timer.h>
#include <Utils.h>


#define EEPROM_MOTOR_CFG	0x0
#define EEPROM_MAP_CFG		EEPROM_MOTOR_CFG + sizeof(Motor)

#pragma pack(push, 1)
typedef struct Motor{
	char name[8];
	char defaulMap;
	float torqueConst;
	float speedConst;
	float speedTorqueGrad;
	float friction;
	float frictionGrad;
	byte ack;
}Motor;
#pragma pack(pop)

#define MAX_MAP_NUM		4
#pragma pack(push, 1)
typedef struct MotorMap{
	char name[8];
	float a0;
	float a1;
	float a2;
	float syncTrh;
	float flatLev;
	boolean flatOut;
	boolean useSyncRect;
	boolean useSyncSafe;
	boolean useEnergyRecovery;
	byte ack;
}MotorMap;
#pragma pack(pop)

#define CMD_TIMEOUT			10000
#define CAN_DRIVER_CMD		0x80

const PROGMEM char GET_DATA_CMD[] = "GET";
const PROGMEM char SET_MOT_DATA_CMD[] = "SETMOT";
const PROGMEM char SET_MAP_DATA_CMD[] = "SETMAP";
const PROGMEM char INVALID_RX_CMD[] = "INV";
const PROGMEM char ERROR_RX_CMD[] = "ERR";
const PROGMEM char OK_RX_CMD[] = "OK";

class MotorDriverLoaderClass{

public:
	boolean initFromEEPROM();
	void onCanRequest(CanFrame*);
	const MotorMap& getMotorMap(int i){ return map[i]; }
	const Motor& getMotor(){ return motor; }

private:
	Motor motor;
	MotorMap map[MAX_MAP_NUM];

	byte getAck(byte* data, int size);

	void onGetRequest();
	void onSetMotorRequest();
	void onSetMapRequest(char mapNum);
};

extern MotorDriverLoaderClass driverLoader;

#endif