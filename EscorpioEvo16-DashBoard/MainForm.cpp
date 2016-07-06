
#include "MainForm.h"
#include "ChannelsBuffer.h"
#include "PhoneInterface.h"
#include "Interprete.h"
#include "StrategySettings.h"
#include "WheelSensor.h"


void MainFormClass::init(Genie &genie){

}

void MainFormClass::update(Genie &genie){
	//UPDATE CONSUMPTION
	genie.WriteObject(GENIE_OBJ_LED_DIGITS, 0, wheelSensor.getEnergy());
	
	//UPDATE CALL STATUS
	genie.WriteObject(GENIE_OBJ_USER_LED, 1, phoneInterface.isCallActive()); 

	//UPDATE TIME LEFT
	int timeLeftVal = strategySettings.TrackData[RACE_TIME] - (wheelSensor.getTimeMillis() / 1000);
	genie.WriteObject(GENIE_OBJ_LED_DIGITS, 1, ((timeLeftVal / 60 * 100) + (timeLeftVal % 60)));

	//UPDATE GAS
	genie.WriteObject(GENIE_OBJ_USER_LED, 0, strategy.getStrat()); 

	//UPDATE SPEED
	int spdVal = (int)(wheelSensor.getSpeed() * 360);
	genie.WriteObject(GENIE_OBJ_ANGULAR_METER, 0, spdVal / 100);
	genie.WriteObject(GENIE_OBJ_LED_DIGITS, 2, spdVal);

	//UPDATE AVERAGE 
	int avgVal = (int)(wheelSensor.getAvgSpeed() * 360);
	genie.WriteObject(GENIE_OBJ_ANGULAR_METER, 1, avgVal / 100);
	genie.WriteObject(GENIE_OBJ_LED_DIGITS, 3, avgVal);

	//UPDATE LAP
	genie.WriteObject(GENIE_OBJ_LED_DIGITS, 6, wheelSensor.getLap());

	//UPDATE BAR
	int pwrVal = channelsBuffer.getValueAs<float>(CAN_DC) * 100 / 255;
	genie.WriteObject(GENIE_OBJ_GAUGE, 0, pwrVal);

	//UPDATE CURRENT LAP TIME
	int lapTimeVal = wheelSensor.getRelativeMillis() / 1000;
	genie.WriteObject(GENIE_OBJ_LED_DIGITS, 4, (lapTimeVal / 60) * 100 + lapTimeVal % 60);

	//UPDATE LAST TIME
	int lastTimeVal = wheelSensor.getLastRelativeMillis() / 1000;
	genie.WriteObject(GENIE_OBJ_LED_DIGITS, 5, lastTimeVal);

	//UPDATE STRATEGY GAP
	String gapVal = String(strategy.getGap() / 1000);   
	genie.WriteStr(0, (char*)gapVal.c_str());
}

MainFormClass mainForm;