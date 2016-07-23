// Channel.h

#ifndef _CHANNEL_h
#define _CHANNEL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Timer.h>

/**
* Class representing all the information relative to a single CAN packet
*/

//TTL timer duration
#define CH_BUFFER_TTL	1500

class Channel{

public:
	//Data type enum
	enum DataTypes : char{
		BIT_FLAG =	'B',
		U_INTEGER = 'U',
		INTEGER =	'I',
		DECIMAL =	'D',
		STRING =	'S'
	};

	//CFG file attributes
	enum Attr : byte{
		CanID,
		Name,
		Size,
		Type,
		ATTR_COUNT
	};


	Channel();

	//Setters
	void setSize(byte size);
	void setID(unsigned short ID);
	void setDataType(DataTypes type);
	void setName(String name);

	//Getters
	byte getSize();
	unsigned short getID();
	DataTypes getDataType();
	String getName();

	//Reset TTL timer
	void resetTTLTimer();
	//Check if the TTL timer has finished
	bool hasTTLFinished();

private:
	//CAN packet size
	byte size;
	
	//CAN packet ID
	unsigned short ID;
	
	//CAN packet data type
	DataTypes type;
	
	//Associated channel name
	String name;

	//TTL timer (time-to-live), if has not finished the last channel value is still valid
	Timer ttlTimer;
};

#endif

