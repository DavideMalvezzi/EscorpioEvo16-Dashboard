#include "EarthDistance.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


/** @brief Computes the arc, in radian, between two WGS-84 positions. 
  * 
  * The result is equal to <code>Distance(from,to)/EARTH_RADIUS_IN_METERS</code> 
  *    <code>= 2*asin(sqrt(h(d/EARTH_RADIUS_IN_METERS )))</code> 
  * 
  * where:<ul> 
  *    <li>d is the distance in meters between 'from' and 'to' positions.</li> 
  *    <li>h is the haversine function: <code>h(x)=sin²(x/2)</code></li> 
  * </ul> 
  * 
  * The haversine formula gives: 
  *    <code>h(d/R) = h(from.lat-to.lat)+h(from.lon-to.lon)+cos(from.lat)*cos(to.lat)</code> 
  * 
  * @sa http://en.wikipedia.org/wiki/Law_of_haversines 
  */  

double ArcInRadians(GpsPoint from, GpsPoint to) {  
    double latitudeArc  = (from.LAT- to.LAT) * DEG_TO_RAD;  
    double longitudeArc = (from.LON - to.LON) * DEG_TO_RAD;  
    double latitudeH = sin(latitudeArc * 0.5);  
    latitudeH *= latitudeH;  
    double lontitudeH = sin(longitudeArc * 0.5);  
    lontitudeH *= lontitudeH;  
    double tmp = cos(from.LAT*DEG_TO_RAD) * cos(to.LAT*DEG_TO_RAD);  
    return 2.0 * asin(sqrt(latitudeH + tmp*lontitudeH));  
}
  
/** @brief Computes the distance, in meters, between two WGS-84 positions. 
  * 
  * The result is equal to <code>EARTH_RADIUS_IN_METERS*ArcInRadians(from,to)</code> 
  * 
  * @sa ArcInRadians 
  */

double DistanceInMeters(GpsPoint from, GpsPoint to) {  
    return EARTH_RADIUS_IN_METERS * ArcInRadians(from, to);  
} 
