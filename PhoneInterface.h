// PhoneInterface.h

#ifndef _PHONEINTERFACE_h
#define _PHONEINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ByteBuffer.h>
#include <Map.h>

#define BLSerial		Serial2
#define BL_SERIAL_BAUD	9600
#define RX_BUFFER_SIZE	128

#define CALL_BUTTON_PIN	2

#define CMD_NUMBER	11
#define DATE_CMD	"DATE"
#define TIME_CMD	"TIME"

#define GSP_LAT_CMD	"GLAT"
#define GSP_LON_CMD	"GLON"
#define GSP_ALT_CMD	"GALT"
#define GSP_SPD_CMD	"GSPD"
#define GSP_ACC_CMD	"GACC"

#define ACC_X_CMD	"ACCX"
#define ACC_Y_CMD	"ACCY"
#define ACC_Z_CMD	"ACCZ"

#define START_CALL_CMD	"STARTCALL"
#define END_CALL_CMD	"ENDCALL"
#define	CALL_ENDED_CMD	"CALLENDED"

class PhoneInterfaceClass{

public:
	void init();
	void update();

	String getDate(){ return date; }
	String getTime(){ return time; }

	double getLatitude(){ return gpsLat; }
	double getLongitude(){ return gpsLon; }
	double getAltitude(){ return gpsAlt; }
	double getGPSAccuracy(){ return gpsAccuracy; }
	double getSpeed(){ return gpsSpeed; }

	double gettAccX(){ return accX; }
	double gettAccY(){ return accY; }
	double gettAccZ(){ return accZ; }

	boolean isCallActive(){ return callActive; }

	friend void onCallButtonPress(void* data);

private:
	boolean callActive;
	double gpsLat, gpsLon, gpsAlt, gpsSpeed, gpsAccuracy;
	double accX, accY, accZ;
	String date, time;

	ByteBuffer rxBuffer;
	Map<String, void(PhoneInterfaceClass::*)(String)> cmdFunctions;

	void addCmdFunction(String cmd, void(PhoneInterfaceClass::*functions)(String));
	String nextParameter(String& param);

	void parseDate(String param);
	void parseTime(String param);

	void parseLat(String param);
	void parseLon(String param);
	void parseAlt(String param);
	void parseSpeed(String param);
	void parseAccuracy(String param);

	void parseAccX(String param);
	void parseAccY(String param);
	void parseAccZ(String param);

	void parseCallStatus(String param);



};

extern PhoneInterfaceClass phoneInterface;


#endif

