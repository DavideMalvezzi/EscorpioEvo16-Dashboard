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

#define PHONE_TAG				F("PHN")
#define PHONE_CFG_FILE			"PHONE.CFG"
#define PHONE_RX_BUFFER_SIZE	128

#define INFO_PACKET		"INF"
#define GPS_PACKET		"GPS"
#define ACC_PACKET		"ACC"
#define CALL_PACKET		"CALL"

#define CALL_CMD			"TEL:"
#define DEFAULT_PHONE_NUM	F("+393663154577")

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
	void init();
	void update();
	void startCall();

	void setGpsDataHandler(GpsDataHandler);

	boolean isCallActive(){ return call.status; }

	friend void onCallButtonPress(void* data);

private:
	enum Attr : byte{
		PHONE_NUM
	};

	String phoneToCall;

	InfoData info;
	GpsData gps;
	AccData acc;
	CallData call;

	ByteBuffer rxBuffer;
	GpsDataHandler gpsHandler;


	boolean parsePacket(const char* header, byte* buffer, int size);
};

extern PhoneInterfaceClass phoneInterface;


#endif

