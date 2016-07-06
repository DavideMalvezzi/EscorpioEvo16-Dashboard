
#include "PhoneInterface.h"


void PhoneInterfaceClass::init(){
	//Init
	INIT_SERIAL(BLSerial, BL_SERIAL_BAUD);
	rxBuffer.resize(PHONE_RX_BUFFER_SIZE);

	
	Configuration cfg;
	if (cfg.loadFromFile(PHONE_CFG_FILE)){
		phoneToCall = cfg.getProperty(PHONE_NUM).asString();
	}
	else{
		consoleForm.println(F("Phone configuration file not found!"));
		ASSERT(false, F("Phone configuration file not found!"));
	}
	
	//Call not active
	callActive = false;

}


void PhoneInterfaceClass::update(){
	//Read all bytes on serial
	if (BLSerial.available()){
		while (BLSerial.available() && rxBuffer.getSize() < rxBuffer.getCapacity()){
			rxBuffer.append(BLSerial.read());
		}

		if (parsePacket(INFO_PACKET, (byte*)&info, sizeof(InfoData))){
			//TODO: put in the ChannelsBuffer
		}

		if (parsePacket(GPS_PACKET, (byte*)&gps, sizeof(GpsData))){
			//TODO: put in the ChannelsBuffer
		}

		if (parsePacket(ACC_PACKET, (byte*)&acc, sizeof(AccData))){
			//TODO: put in the ChannelsBuffer
		}

		if (parsePacket(CALL_PACKET, (byte*)&callActive, sizeof(boolean))){
			//TODO: put in the ChannelsBuffer
		}
	}
}

void PhoneInterfaceClass::call(){
	BLSerial.print(CALL_CMD);
	BLSerial.println(phoneToCall);
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



PhoneInterfaceClass phoneInterface;