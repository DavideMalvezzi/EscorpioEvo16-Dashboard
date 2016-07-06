/*
 Name:		DashBoard.ino
 Created:	8/25/2015 7:25:30 PM
 Author:	Davide Malvezzi
*/

#include "MapSelector.h"
#include "Logger.h"
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
#include "Shell.h"
#include "StrategySettings.h"	
#include "WdtUtils.h"		//Hacked: activated WDT in variant.cpp
#include "WheelSensor.h"	

#include <BitArray.h>
#include <Button.h>
#include <ByteBuffer.h>
#include <Configuration.h>
#include <due_can.h>
#include <DueTimer.h>
#include <genieArduino.h>	//Hacked: to reduce communication times
#include <Map.h>
#include <SPI.h>
#include <SD.h> 
#include <Timer.h>
#include <Utils.h>
#include <Vector.h>

//Hw settings
#define WDT_TIMEOUT	10000
#define SERIAL_BAUD	115200
#define SD_SS_PIN	28
#define BUTTON_NUM	3
#define RED_LED		34
#define YELLOW_LED  32

//TODO: put inside strategy
#define STRATEGY_STEP_PERIOD	500
Timer strategyTimer;
///////////////////////////

//Test stuff
boolean ledStatus;
int loops;
unsigned long avgExecutionTime;
Timer t, sec;

//Inits
void initPorts();
void initDataLogger();
void initStategy();
void initButtons();
//Updates
void updateStrategy();
//Events
void onCanPacketReceived(CAN_FRAME& frame);
void onGpsDataReceived(GpsData& gps);

//Code flag
#define INIT_DEBUG	
#define LOOP_DEBUG	
#define USE_WDT		
//#define ONLY_SHELL
#define STRAT_ON

//Log tag
#define INIT_TAG	F("INIT")
#define LOOP_TAG	F("LOOP")
#define BTN_TAG		F("BTN")

//SW info
#define SW_REV		F("10")
#define SW_INFO		String(F("Dashboard SW Rev ")) + SW_REV + String(F(" built ")) + F(__DATE__) + String(" ") + F(__TIME__)


void setup() {
	//Start timer to count init time
#ifdef INIT_DEBUG
	t.start();
#endif

	//Serial, SPI, digital pin
	initPorts();

#ifndef ONLY_SHELL
	//Display
	displayInterface.init();
	consoleForm.println(SW_INFO);
	consoleForm.println(F("Display OK"));
	Log.i(INIT_TAG) << SW_INFO << Endl;
	Log.i(LCD_TAG) << F("Display OK") << Endl;

	//Can, Channels
	initDataLogger();
	consoleForm.println(F("Datalogger OK"));
	Log.i(DL_TAG) << F("Datalogger OK") << Endl;

	//WheelSensor, Strategy
#ifdef STRAT_ON
	initStategy();
	consoleForm.println(F("Strategy OK"));
	Log.i(STRAT_TAG) << F("Strategy OK") << Endl;
#endif
	//Phone
	phoneInterface.init();
	phoneInterface.setGpsDataHandler(&onGpsDataReceived);
	consoleForm.println(F("Phone OK"));
	Log.i(PHONE_TAG) << F("Phone OK") << Endl;

	//Physical buttons
	initButtons();
	consoleForm.println(F("Buttons OK"));
	Log.i(BTN_TAG) << F("Buttons OK") << Endl;

	//Notify init completed
	consoleForm.println(F("Dashboard init OK"));
	Log.i(INIT_TAG) << F("Dashboard init OK") << Endl;

	//Go to the main form
	displayInterface.setCurrentForm(&mainForm);
#endif
	//Init time
#ifdef INIT_DEBUG
	Log.i(INIT_TAG) << F("Time needed to init: ") << t.elapsedTime() << Endl;
#endif

	//Init test stuff
#ifdef LOOP_DEBUG
	ledStatus = LOW;
	sec.setDuration(1000).start();
	avgExecutionTime = 0;
	loops = 0;
#endif

	//Enable WDT
	//WARNING: To enable the WDT it's needed to remove WDT_Disable in file variant.cpp
#ifdef USE_WDT
	enableWDT(WDT_TIMEOUT);
#else
	disableWDT();
#endif

}

void loop() {
	//Reset Watchdog
#ifdef USE_WDT
	resetWDT();
#endif

	//Test execution time
#ifdef LOOP_DEBUG
	t.start();
#endif
	//////////////////////
#ifndef ONLY_SHELL
	//Read all packet and update debug if necessary
	canInterface.update();

	//Phone update
	phoneInterface.update();

	//Strategy update
#ifdef STRAT_ON
	updateStrategy();
#endif

	//Datalogger update
	dataLogger.update();

	//Display update
	displayInterface.update();

	//Buttons update
	Button::update();
#endif
	//Shell update
	shell.update();
	
	//Test execution time
#ifdef LOOP_DEBUG
	avgExecutionTime += t.elapsedTime();
	loops++;

	if (sec.hasFinished()){
		Log.i(LOOP_TAG) << F("Loop call: ") << loops
						<< F("\t avgExecTime: ") << (float)avgExecutionTime / loops * 1000 << F(" us ")
						<< F("\t freeMem: ") << freeMemory() << Endl;

		ledStatus = !ledStatus;
		digitalWrite(YELLOW_LED, ledStatus);

		avgExecutionTime = 0;
		loops = 0;
		sec.start();
	}
#endif
	//////////////////////
}

//Inits functions
void initPorts(){
	INIT_SERIAL(Serial, SERIAL_BAUD);
	INIT_SD(SD, SD_SS_PIN);

	pinMode(RED_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);
	digitalWrite(RED_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);

	Log.init(&Serial);
	shell.init(&Serial);
}

void initDataLogger(){
	//Can
	canInterface.init(CAN_BPS_125K);
	canInterface.setCanEventCallBack(&onCanPacketReceived);

	//Datalogger
	channelsConfig.init();
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
	//Reverse logic		rising edge = release, falling edge = pressed
	Button::setMaxNumber(BUTTON_NUM);
	Button::add(RESET_BUTTON_PIN, NULL, &onResetButtonPress);
	Button::add(CALL_BUTTON_PIN, NULL, &onCallButtonPress);
	Button::add(CHANGE_FORM_BUTTON_PIN, NULL, &onChangeFormButtonPress);
}

//Update functions
void updateStrategy(){
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

//Events functions
void onCanPacketReceived(CAN_FRAME& frame){

	//Log.i(CAN_TAG) << F("Received ") << frame.id << " " << frame.length << " " << Hex << Log.array<byte>(frame.data.bytes, frame.length) << Endl;

	channelsBuffer.setValue(frame.id, frame.data.bytes, frame.length);

	switch (frame.id){
		case CanID::MOTOR_POWER:
			wheelSensor.setPower(channelsBuffer.getValueAs<float>(CanID::MOTOR_POWER));
			break;
	}

}


void onGpsDataReceived(GpsData& gps){
	byte i = 0;

	//Look if inside a waypoint 
	while (i < strategySettings.getWayPointsNum() && 
		!strategySettings.getWayPoint(i).ProcessNewPoint(gps.latitude, gps.longitude))i++;

	//If found -> process
	if (i < strategySettings.getWayPointsNum()){
		wheelSensor.processWayPoint(i);
		channelsBuffer.setValue(CanID::GPS_WAYPOINT, &i, sizeof(byte));
	}
}


//Buttons events
void onResetButtonPress(void* data){
#ifdef LOOP_DEBUG
	Log.i(BTN_TAG) << F("Wheel sensor reset button pressed") << Endl;
#endif
	wheelSensor.reset();
}

void onCallButtonPress(void* data){
#ifdef LOOP_DEBUG
	Log.i(BTN_TAG) << F("Call button pressed") << Endl;
#endif
	phoneInterface.call();
}

void onChangeFormButtonPress(void* data){
#ifdef LOOP_DEBUG
	Log.i(BTN_TAG) << F("Change form button pressed") << Endl;
#endif

	if (displayInterface.getCurrentForm() == &mainForm){
		displayInterface.setCurrentForm(&debugForm);
	}
	else if (displayInterface.getCurrentForm() == &debugForm){
		displayInterface.setCurrentForm(&mainForm);
	}
}

