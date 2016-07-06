// CanInterfaceUno.h

#ifndef _CANINTERFACEUNO_h
#define _CANINTERFACEUNO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <mcp_can.h>
#include <Utils.h>

#define RX_BUFFER_SIZE		8

//On Arduino or older
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
	#define MCP_RX_INT_PIN		2
	#define MCP_RX_INTERRUPT	INT0
	#define MCP_RST_PINT		7
	#define MCP_CS_PIN			10
//On Arduino micro
#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega32u4__)
	#define MCP_RX_INT_PIN		1
	#define MCP_RX_INTERRUPT    INT3
	#define MCP_CS_PIN			0
#else
	//Malvez 16/12/2015
	//Should abort compilation if the selected board is not supported. 
	//The #error is not recognized by this compiler but still throw error because of it so don't care 
	#error CanInterface.h doesn't support the selected board!
#endif


typedef struct CanFrame{
	unsigned short ID;
	byte size;
	byte data[8];
}CanFrame;

class CanInterfaceUnoClass{

public:
	
	void init(int speed);
	void sendFrame(unsigned short ID, byte* data, byte size);
	int available();
	void readFrame(CanFrame* temp);

	friend void onMcpRxInterrupt();

private:
	int rxPos;
	volatile int rxLast;
	MCP_CAN can;
	volatile CanFrame rxBuffer[RX_BUFFER_SIZE];


};

extern CanInterfaceUnoClass canInterface;

#endif

