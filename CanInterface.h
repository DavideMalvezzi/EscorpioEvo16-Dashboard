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
		//Gps
		GPS_LATITUDE = 0x060,	//Latitude
		GPS_LONGITUDE = 0x061,	//Longitude
		GPS_DATE = 0x064,		//Date dd/mm/yy
		GPS_TIME = 0x065,		//Time hh:MM:ss
		GPS_VALID = 0x068,		//GPS valid flag

		//Motor
		MOTOR_DRIVER_CMD = 0x80,	//Set/Get Motor driver current settings
		MOTOR_MAP = 0x091,			//Current selected map
		MOTOR_POWER = 0x0A2,		//Power
		MOTOR_DUTY_CICLE = 0x0A5,	//Duty cicle

		GPS_WAYPOINT = 0x205,

		RADIO_STATUS = 0x209,  //Radio status flag
		RADIO_REQUEST = 0x20C,  //Radio request flag

		//Bms
		BMS_STATUS = 0x400,	//BMS status
		BATTERY_CELL_0 = 0x401,	//Battery cell 0 voltage. Other cells' ids are next this
		BATTERY_VOLTAGE = 0x410,	//Battery pack voltage
	};
};
	

//Can debug
#define CANSerial				SerialUSB
#define CAN_SERIAL_BAUD			9600
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

class CanInterfaceClass {

public:
	void init(int canSpeed, unsigned short minID = 0, unsigned short maxID = 2048);

	int available();
	CAN_FRAME& read();
	void send(CanID::IDs id, byte* data, byte size);
	void update();

	//Stream
	CanStreamResult streamOverCan(CanID::IDs canID, const char* openStreamCmd, byte* buffer, int size);
	CanStreamResult waitForStreamOverCan(CanID::IDs canID, const char* openStreamCmd, byte* buffer, int expectedBytes);
	byte getAck(byte* data, int size);

	//Debug
	friend void onCanPacketCallBack(CAN_FRAME*);
	void setCanDebugSerialPort(Stream* debugSerial);


private:
	CAN_FRAME frame;
	ByteBuffer rxBuffer;
	Stream* debugSerial;

	//Debug
	void readFromDebugSerial();
	void parseDebugCmd();
};

extern CanInterfaceClass canInterface;


#endif

