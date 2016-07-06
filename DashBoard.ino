/*
 Name:		DashBoard.ino
 Created:	8/25/2015 7:25:30 PM
 Author:	Davide Malvezzi
*/

#include "Channel.h"
#include "ConsoleForm.h"
#include "DebugForm.h"
#include "CanInterface.h"
#include "ChannelsBuffer.h"
#include "ChannelsConfig.h"
#include "DataLogger.h"
#include "DisplayInterface.h"
#include "Interprete.h"
#include "LCDForm.h"
#include "LCDStringList.h"
#include "LCDStringMsg.h"
#include "MainForm.h"
#include "MapsForm.h"
#include "PhoneInterface.h"
#include "StrategySettings.h"
#include "variant.h"	//TODO: activate WDT in variant.cpp
#include "WheelSensor.h"

#include <BitArray.h>
#include <Button.h>
#include <ByteBuffer.h>
#include <Configuration.h>
#include <due_can.h>
#include <DueTimer.h>
#include <genieArduino.h>
#include <Map.h>
#include <SPI.h>
#include <SD.h> 
#include <Timer.h>
#include <Utils.h>
#include <Vector.h>

#define SERIAL_BAUD		9600
#define SD_SS_PIN		28
#define BUTTON_NUM		3

#define RED_LED		34
#define YELLOW_LED  32

#define STRATEGY_STEP_PERIOD	500
Timer strategyTimer;

//Test stuff
int loops;
unsigned long avgExecutionTime;
Timer t, sec;

//Inits
void initPorts();
void initDataLogger();
void initStategy();
void initButtons();
//Updates
void updateDataLogger();
void updateStrategy();

#define INIT_DEBUG	1
#define LOOP_DEBUG	1

void setup() {
	//Serial, SPI, digital pin
	initPorts();

	//Display
	displayInterface.init();
#ifdef INIT_DEBUG
	LOGLN(F("DISPLAY_INIT"));
	consoleForm.println(F("DISPLAY_INIT"));
#endif

	//Can, Channels
	initDataLogger();
#ifdef INIT_DEBUG
	LOGLN(F("DATALOGGER SETTINGS"));
	LOGLN(F("DATALOGGER_INIT"));
	//channelsConfig.debug(); 	//Print channels list
	consoleForm.println(F("DATALOGGER_INIT"));
#endif

	//WheelSensor, Strategy
	//initStategy();
#ifdef INIT_DEBUG
	LOGLN(F("STRATEGY_INIT"));
	consoleForm.println(F("STRATEGY_INIT"));
#endif

	//Phone
	phoneInterface.init();
#ifdef INIT_DEBUG
	LOGLN(F("PHONE_INTERFACE_INIT"));
	consoleForm.println(F("PHONE_INTERFACE_INIT"));
#endif

	//Physical buttons
	initButtons();
#ifdef INIT_DEBUG
	LOGLN(F("BUTTONS_INIT"));
	consoleForm.println(F("BUTTONS_INIT"));
#endif

	//Notify init completed
#ifdef INIT_DEBUG
	LOGLN(F("INIT_COMPLETED"));
	consoleForm.println(F("INIT_COMPLETED"));
#endif

	//Advance to the main form
	onChangeFormButtonPress();	

	//TODO: remove
	//Init test stuff
	sec.setDuration(1000).start();
	avgExecutionTime = 0;
	loops = 0;

	//TODO: enable
	//Watchdog timer
	//WDT_Disable(WDT);
}

void loop() {
	//Reset Watchdog
	//WDT_Restart(WDT);

	//Test execution time
	t.start();
	/////////////////////

	//Read data from Can and save on SD
	updateDataLogger();

	//Update WheelSensor, WayPoints and Strategy
	//TODO: uncomment
	//updateStrategy();

	//Phone update
	phoneInterface.update();

	//Buttons update
	Button::update();

	//Display update
	displayInterface.update();


	//Test execution time
	avgExecutionTime += t.elapsedTime();
	loops++;

	if (sec.hasFinished()){
		LOG("Loop call: "); LOG(loops); 
		LOG("       avgTime: "); LOG((float)avgExecutionTime / loops);
		LOG("       freeMem: "); LOGLN(freeMemory());
		avgExecutionTime = 0;
		loops = 0;
		sec.start();
	}
	
	//////////////////////
}

//Inits functions
void initPorts(){
	//Init ports
	INIT_SERIAL(Serial, SERIAL_BAUD);
	INIT_SD(SD, SD_SS_PIN);

	pinMode(RED_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);
	digitalWrite(RED_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);
}

void initDataLogger(){
	//Can
	canInterface.init(CAN_BPS_125K);

	//Datalogger
	if (!channelsConfig.init()){
		consoleForm.println(F("Channels configuration file not found!"));
		ASSERT(false, F("Channels configuration file not found!"));
	}
	channelsBuffer.init();
	dataLogger.init();
}

void initStategy(){
	//WheelSensor
	wheelSensor.init(); //Reset is called in the init

	//Strategy
	strategySettings.init();
	strategy.setTFirstProfile(strategySettings.FirstProfile[0]);
	strategy.setSFirstProfile(strategySettings.FirstProfile[1]);
	strategy.setTProfile(strategySettings.Profile[0]);
	strategy.setSProfile(strategySettings.Profile[1]);
	strategy.setTLastProfile(strategySettings.LastProfile[0]);
	strategy.setSLastProfile(strategySettings.LastProfile[1]);
	strategy.setTrackData(strategySettings.TrackData);
	strategy.init();

	strategyTimer.setDuration(STRATEGY_STEP_PERIOD).start();
}

void initButtons(){
	//Reverse logic rising edge = release falling edge = pressed
	Button::setMaxNumber(BUTTON_NUM);
	Button::add(RESET_BUTTON_PIN, NULL, &onResetButtonPress);
	Button::add(CALL_BUTTON_PIN, NULL, &onCallButtonPress);
	Button::add(CHANGE_FORM_BUTTON_PIN, NULL, &onChangeFormButtonPress);
}

//Update functions
void updateDataLogger(){
	CAN_FRAME frame;

	//Can update
	frame = canInterface.read();
	while (frame.length != 0){
		channelsBuffer.setValue(frame.id, frame.data.bytes, frame.length);
		frame = canInterface.read();
	}
	
	//Datalogger update
	dataLogger.update();
}

void updateStrategy(){
	//Wheelsensor update
	if (channelsBuffer.isValueUpdated(CanID::MOTOR_POWER)){
		//true -> clear updated flag so it is not updated while a new while is received
		wheelSensor.setPower(channelsBuffer.getValueAs<float>(CanID::MOTOR_POWER, true));
	}

	//Waypoint check
	if (channelsBuffer.isValueUpdated(CanID::GPS_VALID)){
		//true -> set updated flag to false, so while a new value is not received skip the control
		if (channelsBuffer.getValueAs<bool>(CanID::GPS_VALID, true)){
			byte i = 0;
			double lat = channelsBuffer.getValueAs<float>(CanID::GPS_LATITUDE);
			double lon = channelsBuffer.getValueAs<float>(CanID::GPS_LONGITUDE);

			//Look if inside a waypoint 
			while (i < strategySettings.getWayPointsNum() 
				&& !strategySettings.getWayPoint(i).ProcessNewPoint(lat, lon))i++;

			//If found -> process
			if (i < strategySettings.getWayPointsNum()){
				wheelSensor.processWayPoint(i);
				channelsBuffer.setValue(CanID::GPS_WAYPOINT, &i, sizeof(byte));
			}
		}
	}

	//Strategy update
	if (strategyTimer.hasFinished()){
		strategy.step(
			wheelSensor.getLap(),
			wheelSensor.getRelativeSpace() * 100,
			wheelSensor.getRelativeMillis() / 10,
			wheelSensor.getSpeed() * 360
		);
		strategyTimer.start();
	}
}

//Buttons events
void onResetButtonPress(void* data){
	LOGLN("WHEEL_SENSOR_RESET_BUTTON_PRESSED");
	wheelSensor.reset();
}

void onCallButtonPress(void* data){
	LOGLN("CALL_BUTTON_PRESSED");
	phoneInterface.call();
}

void onChangeFormButtonPress(void* data){
	LOGLN("FORM CHANGE_BUTTON_PRESSED");
	displayInterface.nextForm();
}


