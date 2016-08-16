// PhoneInterface.h

#ifndef _PHONEINTERFACE_h
#define _PHONEINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ByteBuffer.h>
#include <Configuration.h>
#include <ConsoleForm.h>

#include "HWConfig.h"
#include "Utils.h"

/**
* Class to interface to the external android app
*/

//Logger tag
#define PHONE_TAG				F("PHN")

//Cfg file
#define PHONE_CFG_FILE			"PHONE.CFG"

//Phone rx buffer size
#define PHONE_RX_BUFFER_SIZE	128

//Phone packets header
#define INFO_PACKET		"INF"
#define GPS_PACKET		"GPS"
#define ACC_PACKET		"ACC"
#define CALL_PACKET		"CALL"

//Phone commands header
#define CALL_CMD			"TEL:"

//Default phone number to call if the cfg file parse failed
#define DEFAULT_PHONE_NUM	F("+393663154577")


//Struct containing the infos sent by the phone
#pragma pack(push, 1)
typedef struct InfoData{
	char date[8];
	char time[8];
	byte ack;
}InfoData;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct GpsData{
	double latitude;
	double longitude;
	double altitude;
	double accuracy;
	double speed;
	byte ack;
}GpsData;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct AccData{
	byte status;
	double x;
	double y;
	double z;
	byte ack;
}AccData;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct CallData{
	boolean status;
	byte ack;
}CallData;
#pragma pack(pop)

//Events handler
typedef void(*GpsDataHandler)(const GpsData&);
//typedef void(*AccelerometerDataHandler)(AccData&);


class PhoneInterfaceClass{

public:
	//Function to call in the setup
	void init();

	//Function to call in the loop
	void update();

	//Request the phone to make a phone call
	void startCall();

	//Set the callback to invoke when a gps packet arrived
	void setGpsDataHandler(GpsDataHandler);

	//Check if there is an active phone call
	boolean isCallActive(){ return call.status; }

	//Start phone call button handler
	friend void onCallButtonPress(void* data);

private:
	//Cfg file properties
	enum Attr : byte{
		PHONE_NUM
	};

	//Phone number to call
	String phoneToCall;

	//Last received packets
	InfoData info;
	GpsData gps;
	AccData acc;
	CallData call;

	//RX byte buffer
	ByteBuffer rxBuffer;

	//Gps callback
	GpsDataHandler gpsHandler;

	//Parse incoming packets
	boolean parsePacket(const char* header, byte* buffer, int size);
};

//Phone interface instance
extern PhoneInterfaceClass phoneInterface;


#endif

