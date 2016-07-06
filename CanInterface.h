// CanInterface.h

#ifndef _CANINTERFACE_h
#define _CANINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <due_can.h>

#include "ChannelsBuffer.h"

//Class containing an enum with all the CanID constants hard-coded
class CanID{
public:
	enum IDs : unsigned short {
		GPS_LATITUDE = 0x060,	//Latitude
		GPS_LONGITUDE = 0x061,	//Longitude
		GPS_DATE = 0x064,
		GPS_TIME = 0x065,
		GPS_VALID = 0x068,	//GPS valid flag

		MOTOR_MAP = 0x091,	//Current selected map
		MOTOR_POWER = 0x0A2,	//Power
		MOTOR_DUTY_CICLE = 0x0A5,	//Duty cicle

		GPS_WAYPOINT = 0x205,

		RADIO_STATUS = 0x209,  //Radio status flag
		RADIO_REQUEST = 0x20C,  //Radio request flag

		BMS_STATUS = 0x400,	//BMS status

		BATTERY_CELL_0 = 0x401,	//Battery cell 0 voltage. Other cells' ids are next this
		BATTERY_VOLTAGE = 0x410,	//Battery pack voltage
	};
};
	
class CanInterfaceClass {

public:
	void init(int canSpeed, unsigned short minID = 0, unsigned short maxID = 2048);
	CAN_FRAME& read();
	void send(unsigned short id, byte* data, byte size);

private:
	CAN_FRAME frame;

};

extern CanInterfaceClass canInterface;


#endif

