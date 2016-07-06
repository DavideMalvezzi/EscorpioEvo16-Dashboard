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
#include "ConsoleForm.h"
#include "DisplayInterface.h"
#include "LCDForm.h"
#include "LCDStringList.h"
#include "LCDStringMsg.h"
#include "Timer.h"

//Motor related
#define MOTOR_CFG_FILE		"MOTOR.CFG"
#define MOTOR_TYPES			F("s8cfffff")
#pragma pack(push, 1)
typedef struct Motor{
	char name[8];
	char defaultMap;
	float torqueConst;
	float speedConst;
	float speedTorqueGrad;
	float friction;
	float frictionGrad;

	enum Attr : byte{
		Name,
		DefaultMap,
		TorqueConst,
		SpeedConst,
		SpeedTorqueGrad,
		Friction,
		FrictionGrad,
		ATTR_COUNT
	};
}Motor;
#pragma pack(pop)

//Map set related
#define MAPS_CFG_FILE		"MAPS.CFG"
#define MAPS_TYPES			F("s8fffffbbbb")
#define MAPS_PER_SET		4
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

	enum Attr : byte{
		Name,
		A0,
		A1,
		A2,
		SyncTrh,
		FlatLev,
		FlatOut,
		UseSyncRect,
		UseSyncSafe,
		UseEnergyRecovery,
		ATTR_COUNT
	};
}MotorMap;
#pragma pack(pop)

//Button indexs
#define GET_MOTOR_BUTTON	0
#define GET_MAPSET_BUTTON	1
#define LOAD_MOT_BUTTON		2
#define LOAD_MAP_BUTTON		3
#define UP_BUTTON			4
#define DOWN_BUTTON			5
#define ENTER_BUTTON		6
#define EXIT_BUTTON			7
#define OK_BUTTON			8


//String-list indexs
#define PROP_TEXT_LIST		4
#define DETAIL_TEXT_LIST	5
#define VALUE_TEXT_LIST		6
#define STATUS_STRING		7

//String-list props
#define	LIST_BUFFER_SIZE	128

//View state
enum ViewState : byte{
	NOTHING_LOADED,
	GETTING_MOTOR_STATE,
	GETTING_MAPSET_STATE,
	LOADING_MOT_STATE_PROP,
	LOADING_MOT_STATE_DETAIL,
	LOADING_MAP_STATE_PROP,
	LOADING_MAP_STATE_DETAIL,
	LOADING_MAP_STATE_VALUE
};

//Can cmd related
#define CAN_DRIVER_CMD		0x80
#define CMD_TIMEOUT			3000
#define GET_MOTOR_DATA_CMD	"GETMOT"
#define GET_MAPSET_DATA_CMD	"GETMAP"
#define SET_MOT_DATA_CMD	"SETMOT"
#define SET_MAP_DATA_CMD	"SETMAP"
#define ERROR_CMD			"ERR"
#define OK_CMD				"OK"

#define PACKET_DELAY	50
enum StreamResult{
	SUCCES,
	ERROR,
	TIMEOUT,
	WRONG_ACK,
	ABORT
};

class MapsFormClass : public LCDForm{

public:
	void init(Genie& genie);
	void update(Genie& genie);
	void onEnter(Genie& genie);
	void onEvent(Genie& genie, genieFrame& evt);
	int getFormIndex(){ return 3; }

private:
	Configuration motorCfg, mapCfg;
	LCDStringList* workingList;
	LCDStringList propList, detailList, valueList;
	LCDStringMsg statusMsg;
	ViewState currentState;


	//Get
	void getMotorData();
	void getMapSetData();

	//Set
	StreamResult setMotorData();
	StreamResult setMapData();

	//Buttons
	void onLoadMotorButtonPressed(Genie& genie);
	void onLoadMapButtonPressed(Genie& genie);
	void onUpButtonPressed(Genie& genie);
	void onDownButtonPressed(Genie& genie);
	void onEnterButtonPressed(Genie& genie);
	void onExitButtonPressed(Genie& genie);
	void onOkButtonPressed(Genie& genie);

	//Show
	void loadMotorProperties();
	void loadMotorDetails();

	void loadMapSetProperties();
	void loadMapSetDetails();
	void loadMapSetValues();

	void clearAll();

	//Rx/Tx
	StreamResult streamOverCan(unsigned short canID, const char* openStreamCmd, byte* buffer, int size);
	StreamResult waitForStreamOverCan(unsigned short canID, const char* openStreamCmd, byte* buffer, int expectedBytes);
	byte getAck(byte* data, int size);

};

extern MapsFormClass mapsForm;


#endif

