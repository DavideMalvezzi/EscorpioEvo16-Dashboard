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

#define GPS_CONFIG_FILE "Gps.cfg"
#define WAYPOINT	6
#define WAY_LAT		0
#define WAY_LON		1
#define WAY_RAD		2
#define WAY_DIST	3
#define WAY_TIME	4
#define WAY_REF		5

#define STRATEGY_CONFIG_FILE "Strategy//Config.txt"
#define TRACK_SETTINGS	7
#define TRACK_LENGHT	0
#define TRACK_FINISH	1
#define RACE_LAPS		2
#define RACE_TIME		3
#define T_FIRST_LAP		4
#define T_LAP			5
#define T_LAST_LAP		6

#define PROFILE_ROW		2
#define PROFILE_COL		1700

#define STRATEGY_FIRSTLAP_FILE	"Strategy//FirstLap.txt"
#define STRATEGY_LAP_FILE		"Strategy//Lap.txt"
#define STRATEGY_LASTLAP_FILE	"Strategy//LastLap.txt"


class StrategySettingsClass {
 private:
	 Vector<WayPoint> gpsWayPoint;

	 void loadStrategy();
	 void loadGPSSettings();
	 void loadLapProfile(char* filePath, unsigned short profile[][PROFILE_COL], int row, int col);
	 String parseNextTrackData(File& file);
	
 public:
	unsigned short TrackData[TRACK_SETTINGS]; // TrackLenght, TrackFinish, RaceLaps, RaceTime, TFirstLap, TLap, TLastLap
	unsigned short FirstProfile[PROFILE_ROW][PROFILE_COL];
	unsigned short Profile[PROFILE_ROW][PROFILE_COL];
	unsigned short LastProfile[PROFILE_ROW][PROFILE_COL];
	bool Valid;

	void init();
	void debugGPSSettings();

	WayPoint& getWayPoint(int index){ return gpsWayPoint[index]; }
	int getWayPointsNum(){ return gpsWayPoint.getCapacity(); }
	

};

extern StrategySettingsClass strategySettings;

#endif

