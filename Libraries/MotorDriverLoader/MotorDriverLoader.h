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
#define EEPROM_MOTOR_ACK	(EEPROM_MOTOR_CFG + sizeof(Motor))
#define EEPROM_MAP_CFG		(EEPROM_MOTOR_ACK + 1)
#define EEPROM_MAP_ACK		(EEPROM_MAP_CFG + sizeof(MotorMap) * MAPS_PER_SET)

#pragma pack(push, 1)
typedef struct Motor{
	char name[8];
	char defaulMap;
	float torqueConst;
	float speedConst;
	float speedTorqueGrad;
	float friction;
	float frictionGrad;
	float gearTrain;
}Motor;
#pragma pack(pop)

#define MAPS_PER_SET		4
#pragma pack(push, 1)
typedef struct MotorMap{
	char name[8];
	float a0;
	float a1;
	float a2;
	float syncTrh;                
	float flatLev;                
	float maxSpeed;               
	boolean flatOut;      
	boolean useSyncRect;          
	boolean useSyncSafe;
	boolean useEnergyRecovery;
}MotorMap;
#pragma pack(pop)

#define CMD_TIMEOUT			3000
#define CAN_DRIVER_CMD		0x80

const PROGMEM char GET_MOTOR_DATA_CMD[] = "GETMOT";
const PROGMEM char GET_MAP_DATA_CMD[] = "GETMAP";
const PROGMEM char SET_MOT_DATA_CMD[] = "SETMOT";
const PROGMEM char SET_MAP_DATA_CMD[] = "SETMAP";
const PROGMEM char ERROR_CMD[] = "ERR";
const PROGMEM char OK_CMD[] = "OK";

#define PACKET_DELAY	50
enum StreamResult{
	SUCCES,
	ERROR,
	TIMEOUT,
	WRONG_ACK,
	ABORT
};

class MotorDriverLoaderClass{

public:
	boolean initFromEEPROM();
	boolean isMotorValid(){ return motorValid; }
	boolean isMapSetValid(){ return mapSetValid; }
	void onCanRequest(CanFrame*);
	MotorMap* getMotorMap(int index){ return &map[index % MAPS_PER_SET]; }
	Motor* getMotor(){ return &motor; }

private:
	boolean motorValid, mapSetValid;
	Motor motor;
	MotorMap map[MAPS_PER_SET];

	void onGetMotorRequest();
	void onGetMapRequest();
	void onSetMotorRequest();
	void onSetMapRequest();

	StreamResult streamOverCan(unsigned short canID, byte* buffer, int size);
	StreamResult waitForStreamOverCan(unsigned short canID, byte* buffer, int expectedBytes);
	byte getAck(byte* data, int size);

};

extern MotorDriverLoaderClass driverLoader;

#endif