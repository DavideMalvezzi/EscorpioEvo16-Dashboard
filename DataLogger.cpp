
#include "DataLogger.h"

void DataLoggerClass::init(){
	Configuration cfg;
	//Load cfg file
	if (cfg.loadFromFile(DL_CFG_FILE) == FILE_VALID){
		//Update cfg
		updateCfg(cfg);
		//Create new log file
		createNewLogFile();
	}
	else {
		//Error on loading, searching a backup
		consoleForm.println(F("No valid Datalogger cfg file found"));
		LOGLN(F("No valid Datalogger cfg file found"));

		if (cfg.loadFromBackup() == FILE_VALID){
			//Update cfg
			updateCfg(cfg);
			//Create new log file
			createNewLogFile();
		}
		else{
			//No valid backup, load default values
			consoleForm.println(F("No valid backup found"));
			consoleForm.println(F("Loading Datalogger default config"));
			LOGLN(F("No valid backup found"));
			LOGLN(F("Loading Datalogger default config"));

			logFileName = LOG_FILE_FOLDER;
			logFileName += DEFAULT_LOG_NAME;
			createNewLogFile();

			logTimer.setDuration(1000 / DEFAULT_SAMPLE_PER_S).start();
		}
	}
}

void DataLoggerClass::update(){
	//If it's time to log!
	if (logTimer.hasFinished()){
		Channel* channel;
		//Open file
		File logFile = SD.open(logFileName, O_WRITE);
		//Print them all
		if (logFile){
			for (int i = 0; i < channelsConfig.getChannelCount(); i++){
				channel = channelsConfig.getChannelByIndex(i);

				if (channelsBuffer.isValueUpdated(channel->ID)){
					logFile.print(
						channelsBuffer.getValueAsString(channel->ID)
					);
				}
				else{
					logFile.print(" ");
				}

				logFile.print(",");
			}
			logFile.println();
			logFile.close();

			channelsBuffer.invalidAllData();
			
			//Restart log timer
			LOGLN(F("Logged"));
		}
		else{
			LOGLN(F("Error opening log file!"));
		}

		logTimer.start();
	}
}

void DataLoggerClass::updateCfg(Configuration& cfg){
	//Build new log file name
	logFileName = LOG_FILE_FOLDER;
	logFileName += cfg[LOG_FILE_NAME].asString() + cfg[LOG_FILE_NUM].asString() + cfg[LOG_FILE_FORMAT].asString();

	//Set log-timer
	logTimer.setDuration(1000 / cfg[SAMPLE_PER_SECOND].asInt()).start();

	//Save next log file name-index
	cfg.setValue(LOG_FILE_NUM, cfg[LOG_FILE_NUM].asInt() + 1);
	cfg.saveChanges();
}

void DataLoggerClass::createNewLogFile(){
	//Print data header on the file
	Channel* c;
	File logFile = SD.open(logFileName, O_WRITE | O_CREAT);
	//Check if the file is opened correctly
	while (!logFile){
		consoleForm.println(F("Error creating log file! Trying again..."));
		LOGLN(F("Error creating log file! Trying again..."));
		delay(1000);

		logFile = SD.open(logFileName, O_WRITE | O_CREAT);
	} 

	for (int i = 0; i < channelsConfig.getChannelCount(); i++){
		c = channelsConfig.getChannelByIndex(i);
		logFile.print(c->name);
		logFile.print(F(" (0x"));
		logFile.print(c->ID, HEX);
		logFile.print(F("),"));
	}
	logFile.println();
	logFile.close();
}


DataLoggerClass dataLogger;

