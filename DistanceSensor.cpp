
#include "DistanceSensor.h"


void DistanceSensor::init(){
	pinMode(DIST_SENSOR_TRIG_PIN, OUTPUT);
	pinMode(DIST_SENSOR_ECHO_PIN, INPUT);

	readTimer.setDuration(DIST_SENSOR_READ_TIME).start();
}

void DistanceSensor::update(){
	int readCount = 0;
	unsigned long readValue = 0, duration = 0;

	if (readTimer.hasFinished()){

		for (int i = 0; i < DIST_SENSOR_SAMPLES; i++){
			digitalWrite(DIST_SENSOR_TRIG_PIN, LOW);
			delayMicroseconds(10);
			digitalWrite(DIST_SENSOR_TRIG_PIN, HIGH);
			delayMicroseconds(10);
			digitalWrite(DIST_SENSOR_TRIG_PIN, LOW);

			readValue = pulseIn(DIST_SENSOR_ECHO_PIN, HIGH, DIST_SENSOR_TIMEOUT);

			if (readValue > 0){
				duration += readValue;
				readCount++;
			}

			delay(1);
		}
		
		if (readCount != 0){
			distance = microsecondsToCentimeters((float)duration / readCount) / 100;
			channelsBuffer.setValue<float>(CanID::DIST_SNS, distance);
			Log.e(DIST_TAG) << "Distance is " << distance << Endl;
		}
		else{
			Log.e(DIST_TAG) << F("All readed timed out!!") << Endl;
		}

		readTimer.start();
	}
	
}

float DistanceSensor::microsecondsToCentimeters(unsigned long microseconds) {
	// The speed of sound is 340 m/s or 29 microseconds per centimeter.
	// The ping travels out and back, so to find the distance of the
	// object we take half of the distance travelled.
	return (float)microseconds / 29.0 / 2.0;
}




DistanceSensor distanceSensor;

