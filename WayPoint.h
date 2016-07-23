#ifndef _WAYPOINT_h
#define _WAYPOINT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "EarthDistance.h"
#include "Logger.h"

class WayPoint {

private:
	GpsPoint CentralPoint;
	float Radius;
	float Timeout;
	char pointSignal;
	float lastdistance;
	float spaceReference;
	bool reference;
	uint32_t lastmillis;

public:
	void init(float CentralLAT, float CentralLON, float radius, float timeout, float space, bool isRef);
	bool processNewPoint (float lat, float lon);
	float getLat(){return CentralPoint.LAT;}
	float getLon(){return CentralPoint.LON;}
	float getRadius(){return Radius;}
	float getTime(){return Timeout;}
	float getSpaceReference() {return spaceReference;}
	bool  isReference() {return reference;}

};

#endif

