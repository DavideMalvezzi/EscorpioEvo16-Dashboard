
#include "GpsInterface.h"


bool GpsInterface::init(){
	Configuration cfg;
	float lat, lon, radius, time, space;
	int isReference;

	valid = true;

	if (cfg.loadFromFile(GPS_CONFIG_FILE) == FILE_VALID){
		gpsWayPoint.resize(cfg.getPropertyCount() / ATTR_COUNT);
		for (int i = 0; i < gpsWayPoint.getCapacity(); i++){
			lat = cfg[Latitude].asFloat();
			lon = cfg[Longitude].asFloat();
			radius = cfg[Radius].asFloat();
			space = cfg[Distance].asFloat();
			time = cfg[Time].asFloat() * 1000;
			isReference = cfg[Reference].asInt();
			gpsWayPoint[i].init(lat, lon, radius, time, space, (isReference == 0) ? false : true);
		}
	}
	else{
		valid = false;
		consoleForm.println(cfg.getErrorMsg());
		Log.e(GPS_TAG) << cfg.getErrorMsg() << Endl;
	}

	return valid;
}

void GpsInterface::debugSettings(){
	WayPoint w;

	Log.i(GPS_TAG) << F("Waypoints: ") << gpsWayPoint.getCapacity() << Endl;
	for (int i = 0; i < gpsWayPoint.getCapacity(); i++){
		w = gpsWayPoint[i];

		Log.i(GPS_TAG) << F("Waypoint ") << i << " "
			<< w.getLat() << " "
			<< w.getLon() << " "
			<< w.getRadius() << " "
			<< w.getSpaceReference() << " "
			<< w.getTime() << " "
			<< w.isReference() << Endl;
	}

}

GpsInterface gps;