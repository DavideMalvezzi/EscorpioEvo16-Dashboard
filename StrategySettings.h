// StrategySettings.h

#ifndef _STRATEGYSETTINGS_h
#define _STRATEGYSETTINGS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <SD.h>
#include "Configuration.h"
#include "WayPoint.h"
#include "Vector.h"
#include "Logger.h"

#define STRAT_TAG	F("STRAT")


#define STRATEGY_CONFIG_FILE "Strategy//STRATEGY.CFG"
#define TRACK_SETTINGS	7
#define TRACK_LENGHT	0
#define TRACK_FINISH	1
#define RACE_LAPS		2
#define RACE_TIME		3
#define T_FIRST_LAP		4
#define T_LAP			5
#define T_LAST_LAP		6

#define PROFILE_ROW		2
#define PROFILE_LEN		1700

//#define STRATEGY_FIRSTLAP_FILE	"Strategy//FirstLap.txt"
//#define STRATEGY_LAP_FILE			"Strategy//Lap.txt"
//#define STRATEGY_LASTLAP_FILE		"Strategy//LastLap.txt"
	
#define LAP_PROFILE_NUM		3
#define MAX_PULSES_PER_LAP	6
#define STRAT_PULSES_FILE	"Strategy//STRATEGY.DAT"

#define START_LAP	0
#define GENERAL_LAP	1
#define END_LAP		2

/*
typedef struct LapProfile{
	unsigned short space[PROFILE_LEN];
	unsigned short time[PROFILE_LEN];
}LapProfile;
*/

typedef struct TrackData{
	unsigned short trackLenght;
	unsigned short trackFinish;
	unsigned short raceLaps;
	unsigned short raceTime;
	unsigned short firstLapTime;
	unsigned short generalLapTime;
	unsigned short lastLapTime;
}TrackData;

#pragma pack(push, 1)
typedef struct Pulse{
	float startSpace;
	float pulseLenght;
	int engineMap;
}Pulse;

typedef struct LapProfile{
	int pulsesCount;
	Pulse pulses[MAX_PULSES_PER_LAP];
}LapProfile;
#pragma pack(pop)


/*
class strategyClass {
public:
	LapProfile firstLap;
	LapProfile lastLap;
	LapProfile defaultLap;

};
*/

class StrategySettingsClass {
 	
public:

	boolean init();
	void debugTrackSettings();
	boolean isValid(){ return valid; }

	const TrackData& getTrackData(){ return trackData; }
	//LapProfile& getFirstLap(){ return firstLap; }
	//LapProfile& getGeneralLap(){ return generalLap; }
	//LapProfile& getLastLap(){ return lastLap; }

	LapProfile* getFirstLap(){ return &lapProfiles[START_LAP]; }
	LapProfile* getGeneralLap(){ return &lapProfiles[GENERAL_LAP]; }
	LapProfile* getLastLap(){ return &lapProfiles[END_LAP]; }

	float getRaceLenght() { return (trackData.raceLaps - 1)*trackData.trackLenght + trackData.trackFinish; }

	//strategyClass& getLapProfiles(){return  strategyclass;}

private:
	boolean valid;
	TrackData trackData;
	LapProfile lapProfiles[LAP_PROFILE_NUM];
	//strategyClass strategyclass;
	//LapProfile firstLap, lastLap, generalLap;

	//void loadLapProfile(const char* filePath, LapProfile& lap, int len);
	bool loadLapProfile();

};

extern StrategySettingsClass strategySettings;

#endif

