#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "WayPoint.h"

// deg, deg, meters, millis
void WayPoint::init(float CentralLAT, float CentralLON, float radius, float timeout, float space, bool isRef)
{
	Radius = radius;
	Timeout = timeout;
	CentralPoint.LAT = CentralLAT;
	CentralPoint.LON = CentralLON;
	spaceReference = space;
	reference = isRef;
	pointSignal = 0;
	lastmillis = 0;
}

bool WayPoint::processNewPoint (float lat, float lon)
{
	bool ret = false;
	GpsPoint newpoint;
	newpoint.LAT = lat; 
	newpoint.LON = lon;
	float distance = DistanceInMeters(CentralPoint, newpoint);

	Log.e("WAY") << "Distance is " << distance << Endl;

	if(distance<=Radius)
	{ // il punto si trova nella circonferenza
		if (pointSignal == 0)
		{
			if (((millis()-lastmillis)>Timeout)||(lastmillis==0)) // Solo se è passato un timeout dall'ultimo ingresso
			{
				pointSignal = 1;
				lastdistance = distance;
				lastmillis = millis(); // Salvo l'istante di tempo in cui entro
			}
			else
			{
				// Il punto è nella circonferenza, ma timeout non è trascorso
				lastmillis = millis(); // Salvo l'istante di tempo in cui entro
			}
		}
		else
		{  //Sono già entrato una volta nel cerchio
			if (distance>lastdistance)
			{ // Comincio ad allontanarmi: aggancio
				pointSignal = 0;
				ret = true;
			}
			else // non mi sto allontanando
			{ 
				lastdistance = distance;
			}
		}
	}
	else
	{ // il punto non si trova nella circonferenza
		if (pointSignal) // Se avevo comunque agganciato il punto
		{
			ret = true;
		}
		pointSignal = 0;
	}

	return ret;
}