// Timer.h

#ifndef _TIMER_h
#define _TIMER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


class Timer {

public:
	Timer(){
		this->duration = 0;
		this->started = false;
	}

	Timer(int duration){
		this->duration = duration;
		this->started = false;
	}

	boolean isRunning() { return started; }

	unsigned long elapsedTime(){
		return millis() - startTime;
	}

	boolean hasFinished(){
		if(millis() - startTime >= duration){
			started = false;
			return true;
		}
		
		return false;
	}

	Timer& forceTimeout(){
		startTime = millis() - duration;
		return *this;
	}

	Timer& start(){
		this->startTime = millis();
		this->started = true;
		return *this;
	}

	Timer& setDuration(unsigned long duration){
		this->duration = duration;
		return *this;
	}

protected:
	boolean started;
	unsigned long startTime, duration;
};


#endif

