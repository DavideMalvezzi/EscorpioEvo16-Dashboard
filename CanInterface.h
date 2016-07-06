// CanInterface.h

#ifndef _CANINTERFACE_h
#define _CANINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <due_can.h>
#include <Timer.h>

#include "ChannelsBuffer.h"

//Class containing an enum with all the CanID constants hard-coded
class CanID{
public:
	enum IDs : unsigned short {
		//Phone
		DATE = 0x60,			//Date dd/mm/yy
		TIME = 0x61,			//Time hh:MM:ss
		GPS_LATITUDE = 0x62,	//Latitude
		GPS_LONGITUDE = 0x63,	//Longitude
		GPS_ALTITUDE = 0x64,	//Altitude
		GPS_ACCURACY = 0x65,	//Accuracy radius
		GPS_SPEED = 0x66,		//GPS valid flag

		CALL_STATUS = 0x70,		//Voice call status		0 - no call		1 - in call
		ACC_X = 0x71,			//Accelerometer x value
		ACC_Y = 0x72,			//Accelerometer y value
		ACC_Z = 0x73,			//Accelerometer z value
		ACC_STATUS = 0x74,		//Accelerometer values precision	

		//Motor
		DRIVER_SETTINGS_CMD = 0x80,	//Set/Get motor driver mapset/motor settings
		DRIVER_SET_MAP_CMD =  0x81,	//Set motor current map

		MOTOR_MAP = 0x91,			//Current selected map

		//MOTOR_CURRENT = 0xA0,		//Current
		//MOTOR_VOLTAGE = 0xA1,		//Voltage
		MOTOR_POWER = 0xA2,			//Power
		//MOTOR_TORQUE = 0xA3,		//Torque
		//MOTOR_RPM = 0xA4,			//RPM
		MOTOR_DUTY_CICLE = 0xA5,	//Duty cicle

		//Strategy
		//IST_VEL = 0x200,
		//AVG_VEL = 0x201,
		//DISTANCE = 0x202,
		//RACE_TIME = 0x203,
		//LAP = 0x204,
		GPS_WAYPOINT = 0x205,
		//REL_SPACE = 0x206,
		//REL_TIME = 0x207,
		//LAST_TIME = 0x208,
		//GAP = 0x209,
		//ENERGY = 0x20A,
		//GAS = 0x20B,

		//Bms
		BMS_STATUS = 0x400,			//BMS status
		BATTERY_CELL_0 = 0x401,		//Battery cell 0 voltage. Other cells' ids are next this
		//...
		PACK_VOLTAGE = 0x410,		
		//CHG_VOLTAGE = 0x411,
		//LOAD_VOLTAGE = 0x412,
		//PACK_CURRENT = 0x413,
		//TEMP_1 = 0x414,
		//TEMP_2 = 0x415,

	};
};
	
//Can debug
#define DebugSerial				SerialUSB
#define CAN_SERIAL_TIMEOUT		5000
#define CAN_SERIAL_BAUD			115200
#define CAD_MSG_HEADER			"CAD"	//CAN Analyzer Data
#define CAN_MSG_HEADER			"CAN"	//SEND + CHANNEL + SIZE + DATA
#define DEBUG_RX_BUFFER_SIZE	32

//Can stream
#define PACKET_DELAY	50
#define CMD_TIMEOUT		3000
#define ERROR_CMD		"ERR"
#define OK_CMD			"OK"

enum CanStreamResult{
	SUCCES,
	ERROR,
	TIMEOUT,
	WRONG_ACK,
	ABORT
};

typedef void(*CanEventHandler)(CAN_FRAME&);


class CanInterfaceClass {

public:
	void init(int canSpeed, unsigned short minID = 0, unsigned short maxID = 2048);
	void update();

	void setCanEventCallBack(CanEventHandler);
	void removeCanEventCallBack();

	int available();
	CAN_FRAME& read();
	void send(CanID::IDs id, byte* data, byte size);

	//Stream
	CanStreamResult streamOverCan(CanID::IDs canID, const char* openStreamCmd, byte* buffer, int size);
	CanStreamResult waitForStreamOverCan(CanID::IDs canID, const char* openStreamCmd, byte* buffer, int expectedBytes);
	byte getAck(byte* data, int size);

	//Debug
	void setCanDebugSerialPort(Stream* debugSerial);


private:
	Stream* debugSerial;
	CAN_FRAME frame;
	ByteBuffer rxBuffer;
	CanEventHandler canEvent;

	//Debug
	void writePacketOnDebugSerial(CAN_FRAME& packet);
	void readFromDebugSerial();
	void parseSerialDebugCmd();
};

extern CanInterfaceClass canInterface;


#endif

