
#include "StrategySettings.h"
#include "ConsoleForm.h"

boolean StrategySettingsClass::init(){

	Configuration cfg;

	if (cfg.loadFromFile(STRATEGY_CONFIG_FILE) == FILE_VALID){

		valid = true;

		//Load track data
		trackData.trackLenght = cfg.getProperty(TRACK_LENGHT).asInt();
		trackData.trackFinish = cfg.getProperty(TRACK_FINISH).asInt();

		trackData.raceLaps = cfg.getProperty(RACE_LAPS).asInt();
		trackData.raceTime = cfg.getProperty(RACE_TIME).asInt();

		trackData.firstLapTime = cfg.getProperty(T_FIRST_LAP).asInt();
		trackData.generalLapTime = cfg.getProperty(T_LAP).asInt();
		trackData.lastLapTime = cfg.getProperty(T_LAST_LAP).asInt();

		debugTrackSettings();

		/*
		//Load first lap
		if (trackData.firstLapTime != 0){
			loadLapProfile(STRATEGY_FIRSTLAP_FILE, firstLap, trackData.trackLenght + 1);
			consoleForm.println(F("First lap profile OK"));
			Log.i(STRAT_TAG) << F("First lap profile OK") << Endl;
		}
		else{
			consoleForm.println(F("First lap profile SKIP"));
			Log.w(STRAT_TAG) << F("First lap profile SKIP") << Endl;
			valid = false;
		}

		//Load general lap
		if (trackData.generalLapTime != 0){
			loadLapProfile(STRATEGY_FIRSTLAP_FILE, generalLap, trackData.trackLenght + 1);
			consoleForm.println(F("General lap profile OK"));
			Log.i(STRAT_TAG) << F("General lap profile OK") << Endl;
		}
		else{
			consoleForm.println(F("General lap profile SKIP"));
			Log.w(STRAT_TAG) << F("General lap profile SKIP") << Endl;
			valid = false;
		}

		//Load last lap
		if (trackData.lastLapTime != 0){
			loadLapProfile(STRATEGY_FIRSTLAP_FILE, lastLap, trackData.trackLenght + 1);
			consoleForm.println(F("Last lap profile OK"));
			Log.i(STRAT_TAG) << F("Last lap profile OK") << Endl;
		}
		else{
			consoleForm.println(F("Last lap profile SKIP"));
			Log.w(STRAT_TAG) << F("Last lap profile SKIP") << Endl;
			valid = false;
		}
		*/

		if (loadLapProfile()){
			consoleForm.println(F("Laps profiles loaded!"));
			Log.i(STRAT_TAG) << F("Laps profiles loaded!") << Endl;
		}
		else{
			consoleForm.println(F("Failed to load laps profiles!"));
			Log.e(STRAT_TAG) << F("Failed to load laps profiles!") << Endl;
			valid = false;
		}
		
	}
	else{
		consoleForm.println(cfg.getErrorMsg());
		Log.e(STRAT_TAG) << cfg.getErrorMsg() << Endl;

		valid = false;
	}

	return valid;
}

void StrategySettingsClass::debugTrackSettings(){
	Log.i(STRAT_TAG) << trackData.trackLenght << " "
		<< trackData.trackFinish << " "
		<< trackData.raceLaps << " "
		<< trackData.raceTime << " "
		<< trackData.firstLapTime << " "
		<< trackData.generalLapTime << " "
		<< trackData.lastLapTime << Endl;
}

/*
void StrategySettingsClass::loadLapProfile(const char* filePath, LapProfile& profile, int len){
	if (SD.exists(filePath)){
		File config = SD.open(filePath, O_READ);
		if (config){
			config.setTimeout(0);

			config.readStringUntil(',');
			for (int i = 0; i < len; i++){
				profile.time[i] = config.readStringUntil(',').toInt();
			}

			config.readStringUntil('\n');

			config.readStringUntil(',');
			for (int i = 0; i < len; i++){
				profile.space[i] = config.readStringUntil(',').toInt();
			}
		}
		else{
			consoleForm.println(String(filePath) + F(" error on opening!"));
			Log.e(STRAT_TAG) << filePath << F(" error on opening!") << Endl;

			valid = false;
		}

	}
	else{
		consoleForm.println(String(filePath) + F(" file not found!"));
		Log.e(STRAT_TAG) << filePath << F(" file not found!") << Endl;

		valid = false;
	}
}
*/

bool StrategySettingsClass::loadLapProfile(){
	if (SD.exists(STRAT_PULSES_FILE)){
		File config = SD.open(STRAT_PULSES_FILE, O_READ);
		if (config){
			for (int i = 0; i < LAP_PROFILE_NUM; i++){
				config.readBytes((byte*)&lapProfiles[i], sizeof(LapProfile));
				//Log.w(STRAT_TAG) << "Lap " << i << "  " << lapProfiles[i].pulsesCount << Endl;
				//delay(100);
				for (int j = 0; j < lapProfiles[i].pulsesCount; j++){
					/*
					Log.w(STRAT_TAG) << lapProfiles[i].pulses[j].engineMap << "  "
						<< lapProfiles[i].pulses[j].pulseLenght << "  "
						<< lapProfiles[i].pulses[j].startSpace << "  " << Endl;
					delay(100);
					*/
				}
			}

			return true;
		}
		Log.e(STRAT_TAG) << F("Laps profiles file opening error!") << Endl;
	}
	Log.e(STRAT_TAG) << F("Laps profiles file don't exists!") << Endl;


	return false;
}

StrategySettingsClass strategySettings;

