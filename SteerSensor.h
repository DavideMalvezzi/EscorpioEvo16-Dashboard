// SteerSensor.h

#ifndef _STEERSENSOR_h
#define _STEERSENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Timer.h>

#include "HWConfig.h"
#include "Logger.h"

#define STEER_TAG	F("STEER")

#define STEER_SENSOR_READ_TIME	200
#define	STEER_SENSOR_SAMPLES	10

#define   RefAngle    10.0
#define   RefDist     0.017
#define   SensorB     0.098
#define   Passo       1.600
#define   Carreggiata 0.520
#define	  Centrale	  755
#define   Dist_dx     633
#define   Dist_sx     870


class SteerSensor{

public:
	void init();
	void update();

	float getLastRead(){ return steerAvg; }

private:
	short steerValue;
	float steerAvg, steerAngle;
	Timer readTimer;
};

extern SteerSensor steerSensor;

#endif

