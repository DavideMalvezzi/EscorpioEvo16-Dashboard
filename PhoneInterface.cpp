
#include "PhoneInterface.h"


void PhoneInterfaceClass::init(){
	//Init
	rxBuffer.resize(PHONE_RX_BUFFER_SIZE);
	
	//Load cfg
	Configuration cfg;
	if (cfg.loadFromFile(PHONE_CFG_FILE) == FILE_VALID){
		phoneToCall = cfg.getProperty(PHONE_NUM).asString();
	}
	else{
		phoneToCall = DEFAULT_PHONE_NUM;
		consoleForm.println(cfg.getErrorMsg());
		Log.e(PHONE_TAG) << cfg.getErrorMsg() << Endl;
	}
	
	//No handler
	gpsHandler = NULL;

	//Reset module
	digitalWrite(BL_RST_PIN, HIGH);
}

void PhoneInterfaceClass::update(){
	/*
	while (1){
		while (BL_SERIAL.available()){
			Log << Hex << BL_SERIAL.read() << " ";
		}
	}
	*/

	//Read all bytes on serial
	if (BL_SERIAL.available()){
		while (BL_SERIAL.available() && rxBuffer.getSize() < PHONE_RX_BUFFER_SIZE){
			rxBuffer.append(BL_SERIAL.read());
		}

		//Log << "buffer size: " << rxBuffer.getSize() << Endl;
		//Log << "buffer: " << Hex << Log.array<byte>(rxBuffer.data(), rxBuffer.getSize()) << Endl;

		if (parsePacket(INFO_PACKET, (byte*)&info, sizeof(InfoData))){
			if (info.ack == getAck((byte*)&info, sizeof(InfoData) - 1)){
				channelsBuffer.setValue(CanID::DATE, (byte*)info.date, sizeof(info.date));
				channelsBuffer.setValue(CanID::TIME, (byte*)info.time, sizeof(info.time));
				
				//Log << "parsed info" << Endl;
				//Log << "buffer size: " << rxBuffer.getSize() << Endl;
				//Log << "buffer: " << Hex << Log.array<byte>(rxBuffer.data(), rxBuffer.getSize()) << Endl;
			}
			else{
				Log.e(PHONE_TAG) << F("INVALID INFO DATA") << Endl;
				Log.e(PHONE_TAG) << Hex << Log.array<byte>((byte*)&info, sizeof(InfoData)) << Endl;
			}			
		}

		if (parsePacket(GPS_PACKET, (byte*)&gps, sizeof(GpsData))){
			if (gps.ack == getAck((byte*)&gps, sizeof(GpsData) - 1)){
				channelsBuffer.setValue<double>(CanID::GPS_LATITUDE,	gps.latitude);
				channelsBuffer.setValue<double>(CanID::GPS_LONGITUDE,	gps.longitude);
				channelsBuffer.setValue<double>(CanID::GPS_ALTITUDE,	gps.altitude);
				channelsBuffer.setValue<double>(CanID::GPS_SPEED,		gps.speed);
				channelsBuffer.setValue<double>(CanID::GPS_ACCURACY,	gps.accuracy);

				//Invoke gps handler
				if (gpsHandler != NULL){
					gpsHandler(gps);
				}

				//Log << "parsed gps" << Endl;
				//Log << "buffer size: " << rxBuffer.getSize() << Endl;
				//Log << "buffer: " << Hex << Log.array<byte>(rxBuffer.data(), rxBuffer.getSize()) << Endl;
			}
			else{
				Log.e(PHONE_TAG) << F("INVALID GPS DATA") << Endl;
				Log.e(PHONE_TAG) << Hex << Log.array<byte>((byte*)&gps, sizeof(GpsData)) << Endl;
			}
		}

		if (parsePacket(ACC_PACKET, (byte*)&acc, sizeof(AccData))){
			if (acc.ack == getAck((byte*)&acc, sizeof(AccData) - 1)){
				channelsBuffer.setValue<double>(CanID::ACC_X,		acc.x);
				channelsBuffer.setValue<double>(CanID::ACC_Y,		acc.y);
				channelsBuffer.setValue<double>(CanID::ACC_Z,		acc.z);
				channelsBuffer.setValue<byte>(CanID::ACC_STATUS,	acc.status);
				
				//Log << "parsed acc" << Endl;
				//Log << "buffer size: " << rxBuffer.getSize() << Endl;
				//Log << "buffer: " << Hex << Log.array<byte>(rxBuffer.data(), rxBuffer.getSize()) << Endl;
			}
			else{
				Log.e(PHONE_TAG) << F("INVALID ACC DATA") << Endl;
				Log.e(PHONE_TAG) << Hex << Log.array<byte>((byte*)&acc, sizeof(AccData)) << Endl;
			}
		}

		
		if (parsePacket(CALL_PACKET, (byte*)&call, sizeof(CallData))){
			channelsBuffer.setValue<boolean>(CanID::CALL_STATUS,	call.status);
		}
		

		//Log << "buffer size: " << rxBuffer.getSize() << Endl;
		//Log << "buffer: " << Hex << Log.array<byte>(rxBuffer.data(), rxBuffer.getSize()) << Endl;

	}
}

void PhoneInterfaceClass::startCall(){
	BL_SERIAL.print('\n');
	BL_SERIAL.print(CALL_CMD);
	BL_SERIAL.print(phoneToCall);
	BL_SERIAL.print('\n');
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