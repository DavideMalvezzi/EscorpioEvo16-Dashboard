
#include "MainForm.h"


void MainFormClass::init(Genie &genie){
	popUpIndex = POP_UP_HIDE;
	prevMap = INVALID_MAP;
	popUpTimer.setDuration(POP_UP_TTL).start().forceTimeout();
}

void MainFormClass::update(Genie &genie){
	//Test widget
	/*
	updateWidget(genie, GENIE_OBJ_GAUGE, 0, (millis() / 10) % 100);

	updateWidget(genie, GENIE_OBJ_LED_DIGITS, 0, millis() % 9999);
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, 1, millis() % 9999);

	updateWidget(genie, GENIE_OBJ_LED_DIGITS, 2, millis() % 999);
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, 4, millis() % 999);
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, 5, millis() % 999);
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, 3, millis() % 9);

	updateString(genie, 1, String('+') + String(millis() % 999));
	updateString(genie, 2, String((float)(millis() % 99999) / 100, 2));
	*/

	//If the pop up show time has finished
	if (popUpTimer.hasFinished()){
		//If the pop up is still visible
		if (popUpIndex != POP_UP_HIDE){
			//Hide the pop up
			updateWidget(genie, GENIE_OBJ_USERIMAGES, POP_UP_IMG, POP_UP_HIDE);
			updateWidget(genie, GENIE_OBJ_STATIC_TEXT, LEFT_TIME_LABEL, 0);
			popUpIndex = POP_UP_HIDE;
		}
		//Update the widgets values
		updateWidgetsValues(genie);
	}
	else /*if (popUpTimer.isRunning())*/{
		//Update the widgets values
		updateWidget(genie, GENIE_OBJ_USERIMAGES, POP_UP_IMG, popUpIndex);
	}
	/*
	else{	
		updateWidgetsValues(genie);
	}
	*/

}

void MainFormClass::showPopUp(byte index, unsigned long duration){
	popUpIndex = index;
	popUpTimer.setDuration(duration).start();
}


void MainFormClass::updateCurrentMap(byte map){
	//If map is valid and is not the default map and is different from the previous map
	if (map != INVALID_MAP && map!=DEFAULT_MAP && prevMap != map){
		prevMap = map;
		//Show the map pop up
		showPopUp(map, POP_UP_TTL);
	}
}

void MainFormClass::updateCurrentMotorPower(byte power){
	//If the power is >= of 255 show the alert pop up
	if (power >= 255){
		showPopUp(POP_UP_WARN, 2 * POP_UP_TTL);
	}
}

void MainFormClass::updateWidgetsValues(Genie& genie){
	//Update all the widgets
	unsigned short dc = (float)channelsBuffer.getValueAs<byte>(CanID::MOTOR_DUTY_CICLE, 0) / 255.0 * 100.0;

	updateWidget(genie, GENIE_OBJ_GAUGE, DUTY_CICLE_BAR, dc);

	updateWidget(genie, GENIE_OBJ_LED_DIGITS, IST_SPEED_DIGITS, wheelSensor.getSpeed() * 3.6 * 100);
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, AVG_SPEED_DIGITS, wheelSensor.getAvgSpeed() * 3.6 * 100);

	updateWidget(genie, GENIE_OBJ_LED_DIGITS, LAP_DIGITS, wheelSensor.getLapNumber());

	updateWidget(genie, GENIE_OBJ_LED_DIGITS, CURR_TIME_DIGITS, convertMillisToMinSec(wheelSensor.getRelativeMillis()));
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, LEFT_TIME_DIGITS, convertMillisToMinSec(wheelSensor.getLeftMillis()));
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, LAST_TIME_DIGITS, convertMillisToMinSec(wheelSensor.getLastRelativeMillis()));

	updateWidget(genie, GENIE_OBJ_USER_LED, RADIO_LED, phoneInterface.isCallActive());
	updateWidget(genie, GENIE_OBJ_USER_LED, GAS_LED, strategy.getStrategyOutput() != 0 ? true : false);
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, PREF_MAP_DIGITS, strategy.getStrategyOutput());

	updateString(genie, GAP_STRING, getGapString());
	updateString(genie, CONSUMPTION_STRING, String(wheelSensor.getEnergy() / 1000.0, 2));
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, LAP_SPACE_DIGITS, wheelSensor.getRelativeSpace());

	//If something failed on the loading show "Safe Mode"
	if (!channelsConfig.isValid() || !strategySettings.isValid()){
		updateString(genie, SAFE_MODE_STRING, F("Safe Mode"));
	}
}

unsigned short MainFormClass::convertMillisToMinSec(unsigned long time){
	time /= 1000;
	return (time / 60) * 100 + (time % 60);
}

String MainFormClass::getGapString(){
	char sign = wheelSensor.getGapMillis() >= 0 ? '+' : ' ';
	String gap;
	gap += wheelSensor.getGapMillis() / 1000;
	while (gap.length() < 3)gap = '0' + gap;
	gap = sign + gap;
	return gap;
}

MainFormClass mainForm;