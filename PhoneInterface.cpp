
#include "PhoneInterface.h"


void PhoneInterfaceClass::init(){
	//Init
	INIT_SERIAL(BLSerial, BL_SERIAL_BAUD);
	rxBuffer.resize(PHONE_RX_BUFFER_SIZE);
	
	//Load cfg
	Configuration cfg;
	if (cfg.loadFromFile(PHONE_CFG_FILE) == FILE_VALID){
		phoneToCall = cfg.getProperty(PHONE_NUM).asString();
	}
	else{
		consoleForm.println(cfg.getErrorMsg());
		Log.assert(false, cfg.getErrorMsg());
	}

	//Call not active
	callActive = false;

	//No handler
	gpsHandler = NULL;

}

void PhoneInterfaceClass::update(){
	//Read all bytes on serial
	if (BLSerial.available()){
		while (BLSerial.available() && rxBuffer.getSize() < rxBuffer.getCapacity()){
			rxBuffer.append(BLSerial.read());
		}

		if (parsePacket(INFO_PACKET, (byte*)&info, sizeof(InfoData))){
			channelsBuffer.setValue(CanID::DATE, (byte*)info.date, sizeof(info.date));
			channelsBuffer.setValue(CanID::TIME, (byte*)info.date, sizeof(info.date));
		}

		if (parsePacket(GPS_PACKET, (byte*)&gps, sizeof(GpsData))){
			channelsBuffer.setValue(CanID::GPS_LATITUDE,	(byte*)&gps.latitude,	sizeof(gps.latitude));
			channelsBuffer.setValue(CanID::GPS_LONGITUDE,	(byte*)&gps.longitude,	sizeof(gps.longitude));
			channelsBuffer.setValue(CanID::GPS_ALTITUDE,	(byte*)&gps.altitude,	sizeof(gps.altitude));
			channelsBuffer.setValue(CanID::GPS_SPEED,		(byte*)&gps.speed,		sizeof(gps.speed));
			channelsBuffer.setValue(CanID::GPS_ACCURACY,	(byte*)&gps.accuracy,	sizeof(gps.accuracy));
			
			//Invoke gps handler
			if (gpsHandler != NULL){
				gpsHandler(gps);
			}
		}

		if (parsePacket(ACC_PACKET, (byte*)&acc, sizeof(AccData))){
			channelsBuffer.setValue(CanID::ACC_X,		(byte*)&acc.x,		sizeof(acc.x));
			channelsBuffer.setValue(CanID::ACC_Y,		(byte*)&acc.y,		sizeof(acc.y));
			channelsBuffer.setValue(CanID::ACC_Y,		(byte*)&acc.z,		sizeof(acc.z));
			channelsBuffer.setValue(CanID::ACC_STATUS,	(byte*)&acc.status, sizeof(acc.status));
		}

		if (parsePacket(CALL_PACKET, (byte*)&callActive, sizeof(boolean))){
			channelsBuffer.setValue(CanID::CALL_STATUS, (byte*)&callActive, sizeof(callActive));
		}
	}
}

void PhoneInterfaceClass::call(){
	BLSerial.print(CALL_CMD);
	BLSerial.print(phoneToCall);
	BLSerial.print('\n');
}

boolean PhoneInterfaceClass::parsePacket(const char* header, byte* buffer, int size){
	int index;
	int headerSize = strlen(header);

	if (rxBuffer.getSize() >= size + headerSize){
		index = rxBuffer.indexOf(header);
		if (index != -1){
			index += headerSize;
			if (rxBuffer.getSize() - index >= size){
				for (int i = 0; i < size; i++){
					buffer[i] = rxBuffer.at(index + i);
				}
				rxBuffer.remove(index - headerSize, size + headerSize);
				return true;
			}
		}
	}
	return false;
}

void PhoneInterfaceClass::setGpsDataHandler(GpsDataHandler gpsHandler){
	this->gpsHandler = gpsHandler;
}


PhoneInterfaceClass phoneInterface;