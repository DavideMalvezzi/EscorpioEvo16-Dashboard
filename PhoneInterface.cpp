#include "PhoneInterface.h"
#include "ConsoleForm.h"
#include <Button.h>


void PhoneInterfaceClass::init(){
	INIT_SERIAL(BLSerial, BL_SERIAL_BAUD);
	rxBuffer.resize(RX_BUFFER_SIZE);
	cmdFunctions.resize(CMD_NUMBER);

	callActive = false;

	addCmdFunction(DATE_CMD, &PhoneInterfaceClass::parseDate);
	addCmdFunction(TIME_CMD, &PhoneInterfaceClass::parseTime);

	addCmdFunction(GSP_LAT_CMD, &PhoneInterfaceClass::parseLat);
	addCmdFunction(GSP_LON_CMD, &PhoneInterfaceClass::parseLon);
	addCmdFunction(GSP_ALT_CMD, &PhoneInterfaceClass::parseAlt);
	addCmdFunction(GSP_SPD_CMD, &PhoneInterfaceClass::parseSpeed);
	addCmdFunction(GSP_ACC_CMD, &PhoneInterfaceClass::parseAccuracy);

	addCmdFunction(ACC_X_CMD, &PhoneInterfaceClass::parseAccX);
	addCmdFunction(ACC_Y_CMD, &PhoneInterfaceClass::parseAccY);
	addCmdFunction(ACC_Z_CMD, &PhoneInterfaceClass::parseAccZ);

	addCmdFunction(CALL_ENDED_CMD, &PhoneInterfaceClass::parseCallStatus);

	LOGLN("PHONE_INTERFACE_INIT");
	consoleForm.println("PHONE_INTERFACE_INIT");
}


void PhoneInterfaceClass::update(){
	while (BLSerial.available()){
		rxBuffer.append(BLSerial.read());
	}

	String cmd, param;
	int index = rxBuffer.indexOf("\r\n");

	while (index != -1){
		cmd = rxBuffer.subArray(0, index).toString();
		param = cmd.substring(cmd.indexOf(" ") + 1);
		cmd = cmd.substring(0, cmd.indexOf(" "));

		if (cmdFunctions.contains(cmd)){
			(this->*(cmdFunctions.value(cmd)))(param);
		}
	
		rxBuffer.remove(0, index + 2);
		index = rxBuffer.indexOf("\r\n");
	}

}

void PhoneInterfaceClass::addCmdFunction(String cmd, void(PhoneInterfaceClass::*function)(String)){
	cmdFunctions.insert(cmd, function);
}

String PhoneInterfaceClass::nextParameter(String& param){
	int index = param.indexOf(" ");
	if (index != -1){
		String next = param.substring(0, index + 1);
		param = param.substring(index + 1);
		return next;
	}
	return param;
}

void PhoneInterfaceClass::parseDate(String param){
	date = nextParameter(param);
}

void PhoneInterfaceClass::parseTime(String param){
	time = nextParameter(param);
}

void PhoneInterfaceClass::parseLat(String param){
	gpsLat = nextParameter(param).toFloat();
}

void PhoneInterfaceClass::parseLon(String param){
	gpsLon = nextParameter(param).toFloat();
}

void PhoneInterfaceClass::parseAlt(String param){
	gpsAlt = nextParameter(param).toFloat();
}

void PhoneInterfaceClass::parseSpeed(String param){
	gpsSpeed = nextParameter(param).toFloat();
}

void PhoneInterfaceClass::parseAccuracy(String param){
	gpsAccuracy = nextParameter(param).toFloat();
}

void PhoneInterfaceClass::parseAccX(String param){
	accX = nextParameter(param).toFloat();
}

void PhoneInterfaceClass::parseAccY(String param){
	accY = nextParameter(param).toFloat();
}

void PhoneInterfaceClass::parseAccZ(String param){
	accZ = nextParameter(param).toFloat();
}

void PhoneInterfaceClass::parseCallStatus(String param){
	callActive = false;
}


PhoneInterfaceClass phoneInterface;