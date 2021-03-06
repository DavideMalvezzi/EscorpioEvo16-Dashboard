#ifndef _WHEELSENSOR_h
#define _WHEELSENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <DueTimer.h>

#include "CanInterface.h"
#include "ChannelsBuffer.h"
#include "GpsInterface.h"
#include "StrategySettings.h"
#include "WayPoint.h"

#define WHEEL_CFR			1.496   // wheel cfr in m
#define WHEEL_SENSOR_UPDATE_TIME	500
#define WHEEL_SENSOR_RESET_TIME		5000

class WheelSensorClass {
	
private:
	void startTimer (Tc *tc, uint32_t channel, IRQn_Type irq);
	volatile float Speed;          // in m/s
	volatile float Space;          // in m
	volatile float RelativeSpace;  // in m
	volatile unsigned int FullLaps; 
	volatile uint32_t TimeMillis;  // time elpased in ms
	volatile uint32_t LastRelativeMillis;  // last time lap in ms
	volatile uint32_t LastFinishTime; // in ms
	volatile float AvgSpeed;       // in m/s
	volatile float Energy;
	volatile float power;
	volatile uint32_t speedtmr;
	volatile float lastGap;
	volatile bool gapIsValid;

	Timer updateTimer;

public:
	friend void TC6_Handler();
	friend void Timer5_Handler();
	friend void onResetButtonPress(void* data = NULL);

	void init (void);
	void processWayPoint (unsigned char WayPointIndex);
	void setPower (float pwr) { power = pwr; }
	float getSpeed() { return Speed; }
	float getSpace() { return Space; }
	float getLastGap() { gapIsValid = false; return lastGap; }
	bool isGapValid(){ return gapIsValid; }
	uint32_t getTimeMillis() { return TimeMillis; }
	float getAvgSpeed() { return AvgSpeed; }
	float getEnergy() { return Energy; }
	unsigned int getLapNumber() { return FullLaps+1; }
	unsigned int getFullLaps() { return FullLaps; }
	float getRelativeSpace() { return RelativeSpace; }
	uint32_t getRelativeMillis() { return TimeMillis-LastFinishTime; }
	uint32_t getLastRelativeMillis() { return LastRelativeMillis; }
	uint32_t getLeftMillis(){ return strategySettings.getTrackData().raceTime * 1000 - getTimeMillis(); }
	float getCorrectedSpace() { return (FullLaps*strategySettings.getTrackData().trackLenght) + RelativeSpace; }
	int getGapMillis() { return TimeMillis - getCorrectedSpace() / strategySettings.getRaceLenght() * strategySettings.getTrackData().raceTime * 1000; }
	void reset();
	void update();

};

extern WheelSensorClass wheelSensor;

#endif

