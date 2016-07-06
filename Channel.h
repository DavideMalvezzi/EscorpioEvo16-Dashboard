// Channel.h

#ifndef _CHANNEL_h
#define _CHANNEL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

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

	byte size, type;
	unsigned short ID;
	String name;
};

#endif

