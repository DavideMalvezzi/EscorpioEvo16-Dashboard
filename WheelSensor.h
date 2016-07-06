#ifndef _WHEELSENSOR_h
#define _WHEELSENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "WayPoint.h"

#define WHEEL_CFR			1.496   // wheel cfr in m
#define RESET_BUTTON_PIN	4

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

public:
	friend void TC6_Handler();
	friend void Timer5_Handler();
	friend void onResetButtonPress(void* data = NULL);

	void init (void);
	void processWayPoint (unsigned char WayPointIndex);
	void setPower (float pwr) { power = pwr; }
	float getSpeed() { return Speed; }
	float getSpace() { return Space; }
	uint32_t getTimeMillis() { return TimeMillis; }
	float getAvgSpeed() { return AvgSpeed; }
	float getEnergy() { return Energy; }
	unsigned int getLap() { return FullLaps; }
	float getRelativeSpace() { return RelativeSpace; }
	uint32_t getRelativeMillis() { return TimeMillis-LastFinishTime; }
	uint32_t getLastRelativeMillis() { return LastRelativeMillis; }
	void reset();

};

extern WheelSensorClass wheelSensor;

#endif

