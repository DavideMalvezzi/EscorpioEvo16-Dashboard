
#include "SteerSensor.h"

void SteerSensor::init(){
	steerAvg = 0;
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

		float adcVsteer = steerAvg - STEER_V_OFFSET;
		float steerDist = adcVsteer * STEER_V_TO_DIST;
		float steerAngle = steerDist * STEER_D_TO_DEG_C1 + steerDist * steerDist * STEER_D_TO_DEG_C2;  // Variabile da loggare
		float ackFactor = ESCORPIO_TRACK*0.5*((steerAngle >= 0) ? (-1) : (1));
		float steerRadInv = tan(steerAngle / 180 * PI) / (ESCORPIO_WHEELBASE + tan(steerAngle / 180 * PI) * ackFactor); // Variabile da loggare

		channelsBuffer.setValue<float>(CanID::STEER_SNS, steerAvg);
		channelsBuffer.setValue<float>(CanID::STEER_ANGLE, steerAngle);
		channelsBuffer.setValue<float>(CanID::STEER_RADINV, steerRadInv);

		//Log.i(STEER_TAG) << "Readed voltage is " << steerAvg << Endl;
		readTimer.start();
	}
}


SteerSensor steerSensor;
