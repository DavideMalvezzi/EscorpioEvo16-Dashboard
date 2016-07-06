
#include "StrategySettings.h"
#include "ConsoleForm.h"

void StrategySettingsClass::init(){
	loadStrategy();
	loadGPSSettings();
}

void StrategySettingsClass::loadGPSSettings(){
	Configuration cfg;
	float lat, lon, radius, time, space;
	int isReference;

	if (cfg.loadFromFile(GPS_CONFIG_FILE)){
		gpsWayPoint.resize(cfg.getPropertyCount() / WAYPOINT);
		for (int i = 0; i<gpsWayPoint.getCapacity(); i++){
			lat = cfg[WAYPOINT * i + WAY_LAT].asFloat();
			lon = cfg[WAYPOINT * i + WAY_LON].asFloat();
			radius = cfg[WAYPOINT * i + WAY_RAD].asFloat();
			space = cfg[WAYPOINT * i + WAY_DIST].asFloat();
			time = cfg[WAYPOINT * i + WAY_TIME].asFloat();
			isReference = cfg[WAYPOINT * i + WAY_REF].asInt();
			gpsWayPoint[i].init(lat, lon, radius, time, space, (isReference == 0) ? false : true);
		}
	}
	else{
		consoleForm.println("GPS configuration file not found!");
		ASSERT(false, "GPS configuration file not found!");
	}
}

void StrategySettingsClass::debugGPSSettings(){
	WayPoint w;

	LOGLN("========== GPS loaded config: ==========");
	LOG("Waypoints: "); LOGLN(gpsWayPoint.getCapacity());
	for (int i = 0; i < gpsWayPoint.getCapacity(); i++){
		w = gpsWayPoint[i];
		LOG(w.getLat()); LOG("  ");
		LOG(w.getLon()); LOG("  ");
		LOG(w.getRadius()); LOG("  ");
		LOG(w.getSpaceReference()); LOG("  ");
		LOG(w.getTime()); LOG("  ");
		LOGLN(w.IsReference());
	}
	LOGLN("========================================");
}

void StrategySettingsClass::loadStrategy(){
	Configuration cfg;
	Valid = true;

	if (cfg.loadFromFile(STRATEGY_CONFIG_FILE)){	

		LOGLN("========== Track loaded config: ==========")
		for (int i = 0; i < TRACK_SETTINGS; i++){
			TrackData[i] = cfg.getProperty(i).asInt();
			LOG(cfg.getProperty(i).getName()); LOG(" = "); LOGLN(TrackData[i]);
		}
		LOGLN("========================================");
	}
	else{
		consoleForm.println("Strategy configuration file not found!");
		ASSERT(false, "Strategy configuration file not found!");
	}

	loadLapProfile(STRATEGY_FIRSTLAP_FILE, FirstProfile, PROFILE_ROW, TrackData[TRACK_LENGHT] + 1);
	LOGLN("FIRST_LAP_PROFILE_LOADED");
	consoleForm.println("FIRST_LAP_PROFILE_LOADED");

	loadLapProfile(STRATEGY_LAP_FILE, Profile, PROFILE_ROW, TrackData[TRACK_LENGHT] + 1);
	LOGLN("GENERAL_LAP_PROFILE_LOADED");
	consoleForm.println("GENERAL_LAP_PROFILE_LOADED");

	loadLapProfile(STRATEGY_LASTLAP_FILE, LastProfile, PROFILE_ROW, TrackData[TRACK_LENGHT] + 1);
	LOGLN("LAST_LAP_PROFILE_LOADED");
	consoleForm.println("LAST_LAP_PROFILE_LOADED");

}

void StrategySettingsClass::loadLapProfile(char* filePath, unsigned short profile[][PROFILE_COL], int row, int col){
	if (SD.exists(filePath)){
		File config = SD.open(filePath, O_READ);

		for (int i = 0; i < row; i++){
			config.readStringUntil(',');
			for (int j = 0; j < col; j++){
				profile[i][j] = config.readStringUntil(',').toInt();
			}
			config.readStringUntil('\n');
		}
	}
	else{
		consoleForm.println(String(filePath) + " file not found!");
		ASSERT(false, String(filePath) + " file not found!");
	}
}




String StrategySettingsClass::parseNextTrackData(File& file){
	String value;
	value = file.readStringUntil('\n');
	value = value.substring(value.indexOf(',') + 1);
	value.trim();
	return value;
}


StrategySettingsClass strategySettings;

