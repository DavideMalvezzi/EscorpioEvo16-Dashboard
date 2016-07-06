
#include "MainForm.h"


void MainFormClass::init(Genie &genie){
	popUpIndex = POP_UP_HIDE;
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

	if (popUpTimer.hasFinished()){
		if (popUpIndex != POP_UP_HIDE){
			updateWidget(genie, GENIE_OBJ_USERIMAGES, POP_UP_IMG, POP_UP_HIDE);
			updateWidget(genie, GENIE_OBJ_STATIC_TEXT, LEFT_TIME_LABEL, 0);
			popUpIndex = POP_UP_HIDE;
		}
		updateWidgetsValues(genie);
	}
	else /*if (popUpTimer.isRunning())*/{
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
	if (map != INVALID_MAP && map!=DEFAULT_MAP && popUpIndex != map){
		showPopUp(map, POP_UP_TTL);
	}
}

void MainFormClass::updateCurrentMotorPower(byte power){
	if (power >= 255){
		showPopUp(POP_UP_WARN, 2 * POP_UP_TTL);
	}
}

void MainFormClass::updateWidgetsValues(Genie& genie){
	unsigned short dc = (float)channelsBuffer.getValueAs<byte>(CanID::MOTOR_DUTY_CICLE, 0) / 255.0 * 100.0;

	updateWidget(genie, GENIE_OBJ_GAUGE, DUTY_CICLE_BAR, dc);

	updateWidget(genie, GENIE_OBJ_LED_DIGITS, IST_SPEED_DIGITS, wheelSensor.getSpeed() * 3.6 * 100);
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, AVG_SPEED_DIGITS, wheelSensor.getAvgSpeed() * 3.6 * 100);

	updateWidget(genie, GENIE_OBJ_LED_DIGITS, LAP_DIGITS, wheelSensor.getLapNumber());

	updateWidget(genie, GENIE_OBJ_LED_DIGITS, CURR_TIME_DIGITS, convertMillisToMinSec(wheelSensor.getRelativeMillis()));
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, LEFT_TIME_DIGITS, convertMillisToMinSec(wheelSensor.getLeftMillis()));
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, LAST_TIME_DIGITS, convertMillisToMinSec(wheelSensor.getLastRelativeMillis()));

	updateWidget(genie, GENIE_OBJ_USER_LED, RADIO_LED, phoneInterface.isCallActive());
	updateWidget(genie, GENIE_OBJ_USER_LED, GAS_LED, strategy.getStrategyOutput());


	updateString(genie, GAP_STRING, getGapString());
	updateString(genie, CONSUMPTION_STRING, String(wheelSensor.getEnergy() / 1000.0, 2));

	if (!channelsConfig.isValid() /* || !strategySettings.isValid() */){
		updateString(genie, SAFE_MODE_STRING, F("Safe Mode"));
	}
}

unsigned short MainFormClass::convertMillisToMinSec(unsigned long time){
	time /= 1000;
	return (time / 60) * 100 + (time % 60);
}

String MainFormClass::getGapString(){
	char sign = wheelSensor.getGapMillis() >= 0 ? '+' : '-';
	String gap;
	gap += strategy.getGap() / 1000;
	while (gap.length() < 3)gap = '0' + gap;
	gap = sign + gap;
	return gap;
}

MainFormClass mainForm;