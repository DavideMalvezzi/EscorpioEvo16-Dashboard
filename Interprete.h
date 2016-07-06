#ifndef _INTERPRETE_H
#define _INTERPRETE_H

#include "math.h"
#include "Utils.h"

class Interprete {

private:
	long gapTime; // Gap in ms
	unsigned char output; // Output Strategy

public:

	long getGap() { return gapTime; };
	byte getStrategyOutput() { return output; };

	void init();
	byte step(byte currentLap, unsigned long relPosition, unsigned long relTime, unsigned short speed);

};

extern Interprete strategy;
#endif