
#include "SteerSensor.h"

void SteerSensor::init(){
	steerAvg = 0;
	analogReadResolution(12);
	readTimer.setDuration(STEER_SENSOR_READ_TIME).start();
}

void SteerSensor::update(){
	if (readTimer.hasFinished()){
		steerAvg = 0;
		for (int i = 0; i < STEER_SENSOR_SAMPLES; i++){
			steerAvg += analogRead(STEER_SENSOR_PIN);
		}
		steerAvg /= STEER_SENSOR_SAMPLES;
		steerAvg = steerAvg / 4096 * 3.3;

		// Calcolo di TrackRadiusInv
		float m;
		//Calcolo dell'angolo
		if (steerAvg >= Centrale){
			//angolo positivo
			m = ((float)RefDist) / (float)((float)Dist_dx - (float)Centrale);
		}
		else{
			//angolo negativo
			m = ((float)RefDist) / (float)((float)Centrale - (float)Dist_sx);
		}

		float q = -m * Centrale;
		float Dist = m * (steerAvg + q);
		float Angle = asin(Dist / SensorB);
		steerAngle = Angle * 10000;
		float tgangle = tan(Angle);

		if (tgangle == 0){
			//allora vado dritto
			steerValue = 0;
		}
		else{
			//sto curvando
			float radius = sqrt(Passo * Passo + pow((Carreggiata / 2.0) + (Passo / tgangle), 2));
			steerValue = (float)(1000.0 / radius);
		}

		Log.i(STEER_TAG) << "Readed voltage is " << steerAvg << Endl;
		readTimer.start();
	}
}


SteerSensor steerSensor;
