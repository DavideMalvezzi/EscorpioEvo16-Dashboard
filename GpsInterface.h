// GpsInterface.h

#ifndef _GPSINTERFACE_h
#define _GPSINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ConsoleForm.h"
#include "Logger.h"
#include "WayPoint.h"

#include <Vector.h>

#define GPS_TAG F("GPS")
#define GPS_CONFIG_FILE "GPS.CFG"

class GpsInterface{

public:

	enum Attr : byte{
		 Latitude,
		 Longitude,		
		 Radius,		
		 Distance,	
		 Time,	
		 Reference,		
		 ATTR_COUNT
	};

	bool init();
	bool isValid(){ return valid; }
	void debugSettings();
	WayPoint& getWayPoint(int index){ return gpsWayPoint[index]; }
	int getWayPointsCount(){ return gpsWayPoint.getCapacity(); }
	
private:
	bool valid;
	Vector<WayPoint> gpsWayPoint;

};

extern GpsInterface gps;

#endif

