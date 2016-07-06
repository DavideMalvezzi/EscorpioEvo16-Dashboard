// ChannelsBuffer.h

#ifndef _CHANNELSBUFFER_h
#define _CHANNELSBUFFER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <BitArray.h>
#include <ByteBuffer.h>
#include <Vector.h>

#include "Channel.h"
#include "ChannelsConfig.h"
#include "Logger.h"

#define CHBUF_TAG	F("CHBUF")


class ChannelsBufferClass {

public:
	void init();
	void debug();

	template <typename T>
	T getValueAs(unsigned short id){
		if (channelsConfig.isValid()){
			int index = channelsConfig.getChannelIndex(id);
			if (index != -1){
				return buffer[index].as<T>();
			}
		}
		return NAN;
	}

	ByteBuffer getValueAsByteArray(unsigned short id);
	String getValueAsString(unsigned short id);

	bool isValueUpdated(unsigned short id);
	void invalidAllData();

	void setValue(unsigned short id, byte* data, unsigned short size);
	template <typename T>
	T setValue(unsigned short id, T value){
		setValue(id, (byte*)&value, sizeof(T));
	}

	void sendOnStream(UARTClass* stream);
	unsigned short getBufferSize(){ return bufferSize; }


private:
	unsigned short bufferSize;
	BitArray updateFlags;
	Vector<ByteBuffer> buffer;

	String uintToString(Channel* channel, byte* data);
	String intToString(Channel* channel, byte* data);

};

extern ChannelsBufferClass channelsBuffer;


#endif

