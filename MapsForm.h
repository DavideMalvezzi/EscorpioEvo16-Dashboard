// MapsForm.h

#ifndef _MAPSFORM_h
#define _MAPSFORM_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

/**
* Class representing the form used to load from remote the driver map set
*/

#include "CanInterface.h"
#include "Configuration.h"
#include "ConsoleForm.h"
#include "DisplayInterface.h"
#include "LCDForm.h"
#include "LCDStringList.h"
#include "LCDStringMsg.h"
#include "Timer.h"

//Motor related

//Motor cfg file
#define MOTOR_CFG_FILE		"MOTOR.CFG"

//Motor pattern to convert into struct the cfgs
//For more info read the description into the Configuration.h library
#define MOTOR_TYPES			F("s8cffffff")

//Need to remove all the compiler optimization on the struct to enable compatibility with other systems
#pragma pack(push, 1)

//Struct representing a single motor configuration
typedef struct Motor{
	char name[8];
	char defaultMap;
	float torqueConst;
	float speedConst;
	float speedTorqueGrad;
	float friction;
	float frictionGrad;
	float gearTrain;

	//Enum representing the cfg files properties
	enum Attr : byte{
		Name,
		DefaultMap,
		TorqueConst,
		SpeedConst,
		SpeedTorqueGrad,
		Friction,
		FrictionGrad,
		GearTrain,
		ATTR_COUNT
	};
}Motor;
#pragma pack(pop)

//Map set related

//Mapset cfg file
#define MAPS_CFG_FILE		"MAPSET.CFG"

//Mapset pattern to convert into struct the cfgs
//For more info read the description into the Configuration.h library
#define MAPS_TYPES			F("s8ffffffbbbb")

//Max count of maps per mapset inside the cfg file
#define MAPS_PER_SET		4

//Need to remove all the compiler optimization on the struct to enable compatibility with other systems
#pragma pack(push, 1)

//Struct representing a single map configuration inside a mapset
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

	//Enum representing the cfg files properties
	enum Attr : byte{
		Name,
		A0,
		A1,
		A2,
		SyncTrh,
		FlatLev,
		MaxSpeed,
		FlatOut,
		UseSyncRect,
		UseSyncSafe,
		UseEnergyRecovery,
		ATTR_COUNT
	};
}MotorMap;
#pragma pack(pop)

//Button indexs
#define GET_MOTOR_BUTTON	3
#define GET_MAPSET_BUTTON	4
#define LOAD_MOT_BUTTON		5
#define LOAD_MAP_BUTTON		6
#define UP_BUTTON			7
#define DOWN_BUTTON			8
#define ENTER_BUTTON		9
#define EXIT_BUTTON			10
#define OK_BUTTON			11
#define BACK_BUTTON			13

//String-list
#define PROP_TEXT_LIST		4
#define DETAIL_TEXT_LIST	5
#define VALUE_TEXT_LIST		6
#define STATUS_STRING		7
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

//Tx/Rx command used to get/set the motor and the mapset in the driver
#define GET_MOTOR_DATA_CMD	"GETMOT"
#define GET_MAPSET_DATA_CMD	"GETMAP"
#define SET_MOT_DATA_CMD	"SETMOT"
#define SET_MAP_DATA_CMD	"SETMAP"

class MapsFormClass : public LCDForm{

public:
	//LCDForm abstract method implementation
	void init(Genie& genie);

	//LCDForm abstract method implementation
	void update(Genie& genie);
	
	//LCDForm abstract method implementation
	//
	void onEnter(Genie& genie);
	
	//LCDForm abstract method implementation
	//Handles the touch screen
	void onEvent(Genie& genie, genieFrame& evt);

	//LCDForm abstract method implementation
	int getFormIndex(){ return 3; }

private:
	//Cfg file for the motor
	Configuration motorCfg;

	//Cfg file for the mapset
	Configuration mapCfg;

	//Loaded mapset
	MotorMap mapSet[MAPS_PER_SET];

	//Pointer representin which StringList is currently in use
	LCDStringList* workingList;

	//StringList instances
	//Each of this instances represent a column where the properties will be listed
	LCDStringList propList, detailList, valueList;

	//Widget to show messages
	LCDStringMsg statusMsg;

	//Current state
	ViewState currentState;

	//Get from the driver the current motor config
	void getMotorData();

	//Get from the driver the current mapset config
	void getMapSetData();

	//Set the driver motor config
	CanStreamResult setMotorData();

	//Set the driver mapset config
	CanStreamResult setMapData();

	//Buttons handler functions
	void onLoadMotorButtonPressed(Genie& genie);
	void onLoadMapButtonPressed(Genie& genie);
	void onUpButtonPressed(Genie& genie);
	void onDownButtonPressed(Genie& genie);
	void onEnterButtonPressed(Genie& genie);
	void onExitButtonPressed(Genie& genie);
	void onOkButtonPressed(Genie& genie);

	//Show on the string list the motor properties in the first column from the left
	void loadMotorProperties();

	//Show on the string list the motor details in the second column from the left
	void loadMotorDetails();

	//Show on the string list the mapset properties in the first column from the left
	void loadMapSetProperties();

	//Show on the string list the mapset details in the second column from the left
	void loadMapSetDetails();

	//Show on the string list the mapset values in the third column from the left
	void loadMapSetValues();

	//Show on the string list the motor properties get from the driver in the second column from the left
	void loadGetMapSetValues();

	//Clear all the string list
	void clearAll();

};

//Maps form instance
extern MapsFormClass mapsForm;


#endif

