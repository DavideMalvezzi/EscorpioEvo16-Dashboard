#ifndef _INTERPRETE_H
#define _INTERPRETE_H

#include "math.h"
#include "Timer.h"
#include "Utils.h"

#define STRATEGY_STEP_PERIOD	500


class Interprete {

private:
	byte output; // Output Strategy
	long gapTime; // Gap in ms
	Timer strategyTimer;

public:

	long getGap() { return gapTime; };
	byte getStrategyOutput() { return output; };

	void init();
	byte step(byte fullLaps, unsigned long relPosition/*, unsigned long relTime, unsigned short speed*/);
	void update();

};

extern Interprete strategy;
#endif