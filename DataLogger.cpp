
#include "DataLogger.h"

void DataLoggerClass::init(){
	Configuration cfg;

	//Load cfg file
	if (cfg.loadFromFile(DL_CFG_FILE)){
		//Debug print
		//cfg.debug();
		//Create new log file
		createNewLogFile(cfg);

		//Set log timer
		logTimer.setDuration(1000.0f / cfg[SAMPLE_PER_SECOND].asInt()).start();
	}
	else{
		consoleForm.println(F("DataLogger configuration file not found!"));
		ASSERT(false, F("DataLogger configuration file not found!"));
	}

	
}

void DataLoggerClass::update(){
	//If it's time to log!
	if (logTimer.hasFinished()){
		//Open file
		File logFile = SD.open(logFileName, O_WRITE);
		//Print them all
		for (int i = 0; i < channelsConfig.getChannelCount(); i++){
			//Get all values as String and DON'T clear updated flag
			logFile.print(
				channelsBuffer.getValueAsString(channelsConfig.getChannelByIndex(i)->ID)
			);
			logFile.print(",");
		}
		logFile.println();
		logFile.close();

		//Restart log timer
		logTimer.start();
		LOGLN("LOG");
	}
}


void DataLoggerClass::createNewLogFile(Configuration& cfg){
	//Build new log file name
	logFileName = LOG_FILE_FOLDER;
	logFileName += cfg[LOG_FILE_NAME].asString() + cfg[LOG_FILE_NUM].asString() + cfg[LOG_FILE_FORMAT].asString();
	//Save next log file name-index
	cfg.setValue(LOG_FILE_NUM, cfg[LOG_FILE_NUM].asInt() + 1);
	cfg.updateCfgFile();

	//Print data header on the file
	Channel* c;
	File logFile = SD.open(logFileName, O_WRITE | O_CREAT);
	//Check if the file is opened correctly
	while (!logFile){
		logFile = SD.open(logFileName, O_WRITE | O_CREAT);
		LOGLN(F("Error creating log file! Trying again..."));
		consoleForm.println(F("Error creating log file! Trying again..."));
		delay(1000);
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

