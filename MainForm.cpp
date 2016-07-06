
#include "MainForm.h"


void MainFormClass::init(Genie &genie){
	//Nothing to do here, for now

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

	unsigned short dc = (float)channelsBuffer.getValueAs<byte>(CanID::MOTOR_DUTY_CICLE) / 255 * 100;
	updateWidget(genie, GENIE_OBJ_GAUGE, DUTY_CICLE_BAR, dc);

	updateWidget(genie, GENIE_OBJ_LED_DIGITS, IST_SPEED_DIGITS, wheelSensor.getSpeed() * 100);
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, AVG_SPEED_DIGITS, wheelSensor.getAvgSpeed() * 100);

	updateWidget(genie, GENIE_OBJ_LED_DIGITS, LAP_DIGITS, wheelSensor.getLap());

	updateWidget(genie, GENIE_OBJ_LED_DIGITS, CURR_TIME_DIGITS, convertMillisToMinSec(wheelSensor.getRelativeMillis()));
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, LEFT_TIME_DIGITS, convertMillisToMinSec(wheelSensor.getLeftMillis()));
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, LAST_TIME_DIGITS, convertMillisToMinSec(wheelSensor.getLastRelativeMillis()));

	updateWidget(genie, GENIE_OBJ_USER_LED, RADIO_LED, phoneInterface.isCallActive());
	updateWidget(genie, GENIE_OBJ_USER_LED, GAS_LED, strategy.getStrat());

	String gap;
	if (strategy.getGap() >= 0){
		gap += '+';
	}
	else{
		gap += '-';
	}

	gap += strategy.getGap();

	updateString(genie, GAP_STRING, gap);
	updateString(genie, CONSUMPTION_STRING, String(wheelSensor.getEnergy(), 2));

}


unsigned short MainFormClass::convertMillisToMinSec(unsigned long time){
	time /= 1000;
	return (time / 60) * 10 + (time % 60) / 10;
}
MainFormClass mainForm;