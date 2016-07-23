// Classe Interprete

#include "Interprete.h"
#include "ConsoleForm.h"

void Interprete::init(){
	/*
	const TrackData& trackData = strategySettings.getTrackData();
	LapProfile& firstLap = strategySettings.getFirstLap();
	LapProfile& generalLap = strategySettings.getGeneralLap();
	LapProfile& lastLap = strategySettings.getLastLap();

	float kFirstLap = firstLap.time[trackData.trackLenght] / trackData.firstLapTime / 100.0;
	float kLap = generalLap.time[trackData.trackLenght] / trackData.generalLapTime / 100.0;
	float kLastLap = lastLap.time[trackData.trackFinish] / trackData.lastLapTime / 100.0;

	//Profile correction
	for (int i = 0; i < trackData.trackLenght; i++){
		firstLap.space[i] *= kFirstLap;
		firstLap.time[i] /= kFirstLap;

		generalLap.space[i] *= kLap;
		generalLap.time[i] /= kLap;
	}

	for (int i = 0; i < trackData.trackFinish; i++){
		lastLap.space[i] *= kLastLap;
		lastLap.time[i] /= kLastLap;
	}

	output = 0;
	gapTime = 0;
	*/
	strategyTimer.setDuration(STRATEGY_STEP_PERIOD).start();
}

void Interprete::update(){
	if (strategySettings.isValid()){
		if (strategyTimer.hasFinished()){
			
			//Step strategy
			//Log.e(STRAT_TAG) << "Step" << Endl;
			output = step(
				wheelSensor.getFullLaps(),
				wheelSensor.getRelativeSpace()/*,
				wheelSensor.getTimeMillis() / 10,
				wheelSensor.getSpeed() * 360*/
			);

			//Update values in ChannelBuffer	
			channelsBuffer.setValue<byte>(CanID::GAS, getStrategyOutput());

			strategyTimer.start();

		}
	}
}

byte Interprete::step(byte fullLaps, unsigned long relPosition/*, unsigned long relTime, unsigned short speed*/){
	/*
	//laps, relpos in m*10e-2, reltime in s*10e-2, speed in km/h *10e-2  
	if (strategySettings.isValid()){
		byte stratOutput = output;
		LapProfile& firstLap = strategySettings.getFirstLap();
		LapProfile& generalLap = strategySettings.getGeneralLap();
		LapProfile& lastLap = strategySettings.getLastLap();
		const TrackData& trackData = strategySettings.getTrackData();

		unsigned long raceLenght = ((trackData.raceLaps - 1) * trackData.trackLenght + trackData.trackFinish) * 100;
		unsigned long raceTime = trackData.firstLapTime * 100 + (trackData.raceLaps - 2) * trackData.generalLapTime * 100 + trackData.lastLapTime * 100;


		//Select lap profile
		unsigned short* spaceProfile;
		unsigned short* timeProfile;

		if (currentLap == 0){
			spaceProfile = strategySettings.getFirstLap().space;
			timeProfile = strategySettings.getFirstLap().time;
		}
		else if (currentLap == trackData.raceLaps - 1){
			spaceProfile = strategySettings.getLastLap().space;
			timeProfile = strategySettings.getLastLap().time;
		}
		else{
			spaceProfile = strategySettings.getGeneralLap().space;
			timeProfile = strategySettings.getGeneralLap().time;
		}

		//Linear interpolation
		long i = ceil((float)relPosition / 100.0);
		float coeff = ((i * 100.0) - relPosition) / 100.0;
		float profileSpeed = spaceProfile[i] * coeff + (spaceProfile[i - 1] * (1.0 - coeff));

		//Determinazione Steorico
		int lp = 0;
		float theoricSpace;

		if (relTime > trackData.firstLapTime * 100){
			relTime -= trackData.firstLapTime * 100;

			while (lp < trackData.raceLaps - 2 && relTime > trackData.generalLapTime * 200){
				relTime -= trackData.generalLapTime * 100;
				lp++;
			}

			if (lp >= trackData.raceLaps - 2){
				//Last lap
				i = 0;
				while (i < trackData.trackLenght && lastLap.time[i] <= relTime)i++;

				coeff = ((float)(lastLap.time[i] - relTime)) / ((float)(lastLap.time[i] - lastLap.time[i - 1]));
				theoricSpace = (((i - 1) * coeff + (i * (1.0 - coeff))) + ((1.0 + lp) * trackData.trackLenght)) * 100.0;
			}
			else{
				//Giro tipo
				i = 0;
				while (i < trackData.trackLenght && timeProfile[i] <= relTime)i++;

				coeff = ((float)(timeProfile[i] - relTime)) / ((float)(timeProfile[i] - timeProfile[i - 1]));
				theoricSpace = (((i - 1) * coeff + (i * (1.0 - coeff))) + ((1.0 + lp)*trackData.trackLenght)) * 100.0;
			}
		}
		else {
			i = 0;
			while (i < trackData.trackLenght && firstLap.time[i] <= relTime)i++;

			coeff = ((float)(firstLap.time[i] - relTime)) / ((float)(firstLap.time[i] - firstLap.time[i - 1]));
			theoricSpace = (((i - 1.0) * coeff + (i * (1.0 - coeff)))) * 100.0;
		}

		// Aggiustamento con coefficiente k
		if (raceLenght < theoricSpace){
			theoricSpace = raceLenght;
		}

		float spaceLeft = raceLenght - (relPosition + (currentLap * trackData.trackLenght * 100));
		float theoricSpaceLeft = raceLenght - theoricSpace;
		float k = spaceLeft / theoricSpaceLeft;
		profileSpeed *= k;


#define hysteresisON (0.4 * 360)
#define hysteresisOFF (0.4 * 360)
		if (stratOutput != 0){
			if (speed >= profileSpeed + hysteresisON){
				stratOutput = 0;
			}
			else{
				stratOutput = 1;
			}
		}
		else{
			if (speed + hysteresisOFF <= profileSpeed){
				stratOutput = 1;
			}
			else{
				stratOutput = 0;
			}
		}


		gapTime = (long)((theoricSpace - (raceLenght - spaceLeft)) / (raceLenght / raceTime * 100) * 1000);

		output = stratOutput;

		return stratOutput;
	}
	*/

	byte strat = 0;
	LapProfile* refLap;

	if (fullLaps == START_LAP){
		refLap = strategySettings.getFirstLap();
	}
	else if (fullLaps == strategySettings.getTrackData().raceLaps - 1){
		refLap = strategySettings.getLastLap();
	}
	else {
		refLap = strategySettings.getGeneralLap();
	}

	//Log.e(STRAT_TAG) << "Rel space is " << relPosition << Endl;

	for (byte pulse = 0; pulse < refLap->pulsesCount; pulse++){
		if (relPosition >= refLap->pulses[pulse].startSpace){
			//Log.e(STRAT_TAG) << "Start space is " << refLap->pulses[pulse].startSpace << Endl;
			if ((relPosition - refLap->pulses[pulse].startSpace) <= refLap->pulses[pulse].pulseLenght){
				strat = refLap->pulses[pulse].engineMap;
				break;
			}
		}
	}
	
	return strat;
}

Interprete strategy;
