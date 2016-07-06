#ifndef _WAYPOINT_h
#define _WAYPOINT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "EarthDistance.h"

//GPS
#define CAN_LAT			0x60
#define CAN_LON			0x61
#define CAN_GPSVALID	0x68
#define CAN_WAYPOINT	0x205 


class WayPoint {

private:
	GpsPoint CentralPoint;
	float Radius;
	float Timeout;
	char pointSignal;
	float lastdistance;
	float spaceReference;
	bool isReference;
	uint32_t lastmillis;

public:
	void init(float CentralLAT, float CentralLON, float radius, float timeout, float space, bool isRef);
	bool ProcessNewPoint (float lat, float lon);
	float getLat(){return CentralPoint.LAT;}
	float getLon(){return CentralPoint.LON;}
	float getRadius(){return Radius;}
	float getTime(){return Timeout;}
	float getSpaceReference() {return spaceReference;}
	bool  IsReference() {return isReference;}

};

#endif

