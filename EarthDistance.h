#ifndef _EDISTANCE_h
#define _EDISTANCE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

typedef struct GpsPoint 
{
	float LAT, LON;
};

/// @brief Earth's quatratic mean radius for WGS-84  
#define EARTH_RADIUS_IN_METERS  ((double)(6372797.560856))

/** @brief Computes the arc, in radian, between two WGS-84 positions. 
  * 
  * The result is equal to <code>Distance(from,to)/EARTH_RADIUS_IN_METERS</code> 
  *    <code>= 2*asin(sqrt(h(d/EARTH_RADIUS_IN_METERS )))</code> 
  * 
  * where:<ul> 
  *    <li>d is the distance in meters between 'from' and 'to' positions.</li> 
  *    <li>h is the haversine function: <code>h(x)=sin�(x/2)</code></li> 
  * </ul> 
  * 
  * The haversine formula gives: 
  *    <code>h(d/R) = h(from.lat-to.lat)+h(from.lon-to.lon)+cos(from.lat)*cos(to.lat)</code> 
  * 
  * @sa http://en.wikipedia.org/wiki/Law_of_haversines 
  */  

double ArcInRadians(GpsPoint from, GpsPoint to);
  
/** @brief Computes the distance, in meters, between two WGS-84 positions. 
  * 
  * The result is equal to <code>EARTH_RADIUS_IN_METERS*ArcInRadians(from,to)</code> 
  * 
  * @sa ArcInRadians 
  */

double DistanceInMeters(GpsPoint from, GpsPoint to);


#endif