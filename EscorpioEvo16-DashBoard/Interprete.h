#ifndef _INTERPRETE_H
#define _INTERPRETE_H

#include "Utils.h"

class Interprete {

private:
	long GAP; // Gap in ms
	unsigned char output; // Output Strategy
	float k;
	unsigned short *TrackData; // TrackLenght, TrackFinish, RaceLaps, RaceTime, TFirstLap, TLap, TLastLap
	unsigned short *TLap, *SLap;
	unsigned short *TLastLap, *SLastLap;
	unsigned short *TFirstLap, *SFirstLap;
	float kfirstlap, klap, klastlap;

public:
	void setTFirstProfile (unsigned short *p) { TFirstLap = p; };
	void setSFirstProfile (unsigned short *p) { SFirstLap = p; };
	void setTProfile (unsigned short *p) { TLap = p; };
	void setSProfile (unsigned short *p) { SLap = p; };
	void setTLastProfile (unsigned short *p) { TLastLap = p; };
	void setSLastProfile (unsigned short *p) { SLastLap = p; };
	void setTrackData (unsigned short *p) { TrackData = p; };

	long getGap() { return GAP; };
	unsigned char getStrat() { return output; };

	void init();
	unsigned char step(unsigned char FullLaps, unsigned long RelativePosition, unsigned long RelativeTime, unsigned short Speed);

};

extern Interprete strategy;
#endif