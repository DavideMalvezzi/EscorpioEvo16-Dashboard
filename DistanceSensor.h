// DistanceSensor.h

#ifndef _DISTANCESENSOR_h
#define _DISTANCESENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CanInterface.h"
#include "ChannelsBuffer.h"
#include "HWConfig.h"

#include <Timer.h>

#define DIST_TAG	F("DIST_SNS")

#define DIST_SENSOR_READ_TIME	200
#define	DIST_SENSOR_SAMPLES		10
#define DIST_SENSOR_TIMEOUT		750	//microseconds

class DistanceSensor{

public:
	void init();
	void update();
	float getLastRead(){ return distance; }

private:
	float distance;
	Timer readTimer;

	float microsecondsToCentimeters(unsigned long microseconds);
};


extern DistanceSensor distanceSensor;

#endif

