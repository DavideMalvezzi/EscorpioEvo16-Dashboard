// Channel.h

#ifndef _CHANNEL_h
#define _CHANNEL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Timer.h>

#define CH_BUFFER_TTL	1500

class Channel{

public:
	enum DataTypes : char{
		BIT_FLAG =	'B',
		U_INTEGER = 'U',
		INTEGER =	'I',
		DECIMAL =	'D',
		STRING =	'S'
	};

	enum Attr : byte{
		CanID,
		Name,
		Size,
		Type,
		ATTR_COUNT
	};

	Channel();

	void setSize(byte size);
	void setID(unsigned short ID);
	void setDataType(DataTypes type);
	void setName(String name);

	byte getSize();
	unsigned short getID();
	DataTypes getDataType();
	String getName();

	void resetTTLTimer();
	bool hasTTLFinished();

private:
	byte size;
	unsigned short ID;
	DataTypes type;
	String name;
	Timer ttlTimer;
};

#endif

