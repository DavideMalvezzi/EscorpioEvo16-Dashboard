
#include "CanInterfaceUno.h"

void onMcpRxInterrupt(){
	int index = canInterface.rxLast;
	byte size;
	byte buffer[8];

	canInterface.can.readMsgBuf(&size, buffer);
	canInterface.rxBuffer[index].ID = canInterface.can.getCanId();
	canInterface.rxBuffer[index].size = size;
	for(int i = 0; i <size; i++){
		canInterface.rxBuffer[index].data[i] = buffer[i];
	}
	
	canInterface.rxLast = (canInterface.rxLast + 1) % RX_BUFFER_SIZE;;
}

void CanInterfaceUnoClass::init(int speed){
	pinMode(MCP_RX_INT, INPUT_PULLUP);  

	//On arduino set the reset pin also
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
	pinMode(MCP_RST_PINT, OUTPUT);
	digitalWrite(MCP_RST_PINT, HIGH);
#endif

	int res = can.begin(speed, MCP_CS_PIN);
	if (res == CAN_OK){
		LOGLN(F("CAN_INIT_OK"));
	}
	else{
		ASSERT(false, F("CAN_INIT_FAIL"));
	}

	SPI.usingInterrupt(MCP_RX_INTERRUPT);
	attachInterrupt(MCP_RX_INTERRUPT, onMcpRxInterrupt, FALLING);

	rxPos = 0;
	rxLast = 0;
}

void CanInterfaceUnoClass::sendFrame(unsigned short ID, byte* data, byte size){
	SPI.beginTransaction(SPISettings());
	can.sendMsgBuf(ID, false, size, data);
	SPI.endTransaction();
}

int CanInterfaceUnoClass::available(){
	int count = rxLast - rxPos;
	if(count < 0 ) count += RX_BUFFER_SIZE;
	return count;
}

void CanInterfaceUnoClass::readFrame(CanFrame* temp){
	int index = rxPos;
	if (available()){
		temp->ID = rxBuffer[index].ID;
		temp->size = rxBuffer[index].size;

		for (int i = 0; i < temp->size; i++){
			temp->data[i] = rxBuffer[index].data[i];
		}

		rxPos = (rxPos + 1) % RX_BUFFER_SIZE;
	}
	else{
		temp->ID = 0;
		temp->size = 0;
	}
}


CanInterfaceUnoClass canInterface;