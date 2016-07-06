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
#include "variant.h"		//Hacked: activated WDT in variant.cpp
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

#define WDT_TIMEOUT	10000
#define SERIAL_BAUD	115200
#define SD_SS_PIN	28
#define BUTTON_NUM	3
#define RED_LED		34
#define YELLOW_LED  32

//TODO: put inside strategy
#define STRATEGY_STEP_PERIOD	500
Timer strategyTimer;

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
//WDT
void enableWDT(uint32_t timeout);

#define INIT_DEBUG	
#define LOOP_DEBUG	
#define USE_WDT		
#define SW_REV		F("9")
#define SW_INFO		String(F("Dashboard SW Rev ")) + SW_REV + String(F(" built ")) + F(__DATE__) + String(" ") + F(__TIME__)


void setup() {
	//Start timer to count init time
#ifdef INIT_DEBUG
	t.start();
#endif
	
	//Serial, SPI, digital pin
	initPorts();
	
	//Display
	displayInterface.init();
	consoleForm.println(SW_INFO);
	consoleForm.println(F("Display OK"));
	LOGLN(SW_INFO);
	LOGLN(F("Display OK"));

	//Can, Channels
	initDataLogger();
	consoleForm.println(F("Datalogger OK"));
	LOGLN(F("Datalogger OK"));

	//WheelSensor, Strategy
	initStategy();
	consoleForm.println(F("Strategy OK"));
	LOGLN(F("Strategy OK"));

	//Phone
	phoneInterface.init();
	phoneInterface.setGpsDataHandler(&onGpsDataReceived);
	consoleForm.println(F("Phone OK"));
	LOGLN(F("Phone OK"));

	//Physical buttons
	initButtons();
	consoleForm.println(F("Buttons OK"));
	LOGLN(F("Buttons OK"));

	//Notify init completed
	consoleForm.println(F("Dashboard init OK"));
	LOGLN(F("Dashboard init OK"));

	//Go to the main form
	displayInterface.setCurrentForm(&mainForm);

	//Init time
#ifdef INIT_DEBUG
	LOG(F("Time needed to init: ")); LOGLN(t.elapsedTime());
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
	WDT_Disable(WDT);
#endif
}

void loop() {
	//Reset Watchdog
#ifdef USE_WDT
	WDT_Restart(WDT);
#endif

	//Test execution time
#ifdef LOOP_DEBUG
	t.start();
#endif
	//////////////////////

	//Read all packet and update debug if necessary
	canInterface.update();

	//Phone update
	phoneInterface.update();

	//Strategy update
	updateStrategy();

	//Datalogger update
	dataLogger.update();

	//Display update
	displayInterface.update();

	//Buttons update
	Button::update();

	//Test execution time
	avgExecutionTime += t.elapsedTime();
	loops++;
#ifdef LOOP_DEBUG
	if (sec.hasFinished()){
		LOG("Loop call: "); LOG(loops); 
		LOG("       avgExTime: "); LOG((float)avgExecutionTime * 1000 / loops); LOG(" us ");
		LOG("       freeMem: "); LOGLN(freeMemory());

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
	/*
	LOGLN("Received");
	LOGLN(frame.id);
	LOGLN(frame.length);
	LOG_ARR(frame.data.bytes, frame.length, HEX);
	*/
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
	LOGLN(F("WHEEL_SENSOR_RESET_BUTTON_PRESSED"));
#endif
	wheelSensor.reset();
}

void onCallButtonPress(void* data){
#ifdef LOOP_DEBUG
	LOGLN(F("CALL_BUTTON_PRESSED"));
#endif
	phoneInterface.call();
}

void onChangeFormButtonPress(void* data){
#ifdef LOOP_DEBUG
	LOGLN(F("FORM CHANGE_BUTTON_PRESSED"));
#endif

	if (displayInterface.getCurrentForm() == &mainForm){
		displayInterface.setCurrentForm(&debugForm);
	}
	else if (displayInterface.getCurrentForm() == &debugForm){
		displayInterface.setCurrentForm(&mainForm);
	}
}

//WDT
void enableWDT(uint32_t timeout){
	//this assumes the slow clock is running at 32.768 kHz watchdog frequency is therefore 32768 / 128 = 256 Hz 
	timeout = timeout * 256 / 1000;
	if (timeout == 0)
		timeout = 1;
	else if (timeout > 0xFFF)
		timeout = 0xFFF;
	//	timeout = WDT_MR_WDRSTEN | WDT_MR_WDRPROC | WDT_MR_WDV(timeout) | WDT_MR_WDD(timeout);
	timeout = WDT_MR_WDRSTEN | WDT_MR_WDV(timeout) | WDT_MR_WDD(timeout);

	WDT_Enable(WDT, timeout);
}