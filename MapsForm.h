// MapsForm.h

#ifndef _MAPSFORM_h
#define _MAPSFORM_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "CanInterface.h"
#include "Configuration.h"
#include "DisplayInterface.h"
#include "Timer.h"

#define MOTOR_CFG_FILE		"MOTOR.CFG"
#define MOTOR					7
#define MOTOR_NAME				0
#define MOTOR_DEF_MAP			1
#define MOTOR_TORQUE_CONST		2
#define MOTOR_SPEED_CONST		3
#define MOTOR_SPD_TORQUE_GRAD	4
#define MOTOR_FRICTION			5
#define MOTOR_FRICTION_GRAD		6

#define MAPS_CFG_FILE		"MAPS.CFG"
#define MAP					10
#define MAP_NAME			0
#define MAP_A0				1
#define MAP_A1				2
#define MAP_A2				3
#define MAP_SYNC_RECT		4
#define MAP_SYNC_SAFE		5
#define MAP_SYNC_TRH		6
#define MAP_EN_RECOVERY		7
#define MAP_FLAT_OUT		8
#define MAP_FLAT_LEV		9

#define GET_DATA_BUTTON		0
#define LOAD_MOT_BUTTON		1
#define LOAD_MAP_BUTTON		2
#define OK_BUTTON			3
#define UP_BUTTON			4
#define DOWN_BUTTON			5

#define LEFT_TEXT_LIST		3
#define RIGHT_TEXT_LIST		4
#define STATUS_STRING		5

#define	LIST_BUFFER_SIZE	128
#define TEXT_LIST_COLS		15
#define TEXT_LIST_ROWS		21

#define GETTING_DATA_STATE	0
#define LOADING_MOT_STATE	1
#define LOADING_MAP_STATE	2

#define CMD_TIMEOUT			10000

#define CAN_DRIVER_CMD		0x80
#define GET_DATA_CMD		"GET"
#define SET_MOT_DATA_CMD	"SETMOT"
#define SET_MAP_DATA_CMD	"SETMAP"
#define INVALID_RX_CMD		"INV"
#define ERROR_RX_CMD		"ERR"
#define OK_RX_CMD			"OK"

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


class MapsFormClass : public DisplayForm{

public:
	void init(Genie& genie);
	void update(Genie& genie);
	void onEnter(Genie& genie);
	void onEvent(Genie& genie, genieFrame& evt);
	int getFormIndex(){ return 3; }

private:
	char currentListIndex, currentState;
	String leftList, rightList, statusString;
	Configuration motorCfg, mapCfg;

	void onGetDataButtonPressed(Genie& genie);
	bool getMotorData();
	bool getMapData();

	void onLoadMotorButtonPressed(Genie& genie);
	void onLoadMapButtonPressed(Genie& genie);

	void onOkButtonPressed(Genie& genie);
	void setMotorData();
	void setMapData();

	void onUpButtonPressed(Genie& genie);
	void onDownButtonPressed(Genie& genie);

	byte getAck(byte* data, int size);

	void showConfig(Genie& genie);

	void clearString(Genie& genie, String& list, int index);

};

extern MapsFormClass mapsForm;


#endif

