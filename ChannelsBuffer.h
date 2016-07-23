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

/**
* Class containing a buffer with all the lastest CAN packets value for each channel
*/

//Logger tag
#define CHBUF_TAG	F("CHBUF")

class ChannelsBufferClass {

public:
	//Init function to call inside the setup
	void init();

	//Debug on serial
	void debug();

	//Template function which return the lastest channel values if the TTL is valid, else return a default value
	//Example:		int a = channelBuffer.getValueAs<int>(0x60, 0);
	template <typename T>
	T getValueAs(unsigned short id, T defaultValue){
		//If CFG file was loaded
		if (channelsConfig.isValid()){
			//Search the channel
			int index = channelsConfig.getChannelIndex(id);
			//If channel exists and its TTL is still valid
			if (index != -1 && !channelsConfig.getChannelByIndex(index)->hasTTLFinished()){
				//Return the converted value
				return buffer[index].as<T>();
			}
		}
		//Else return the default value
		return defaultValue;
	}

	//Template function which return the lastest channel values
	template <typename T>
	T getValueAs(unsigned short id){
		//If CFG file was loaded
		if (channelsConfig.isValid()){
			//Search the channel
			int index = channelsConfig.getChannelIndex(id);
			//If channel exists
			if (index != -1){
				//Return the converted value
				return buffer[index].as<T>();
			}
		}
		//Else return NAN
		return NAN;
	}

	//Return a ByteBuffer copy associated to the channel
	ByteBuffer getValueAsByteArray(unsigned short id);

	//Return the lastest channel value converted to string
	String getValueAsString(unsigned short id);

	//Return true if the TTL timer associated to a channel has not finished, else false
	boolean isValueUpdated(unsigned short id);

	//Set the lastest value associated to a channel
	void setValue(unsigned short id, byte* data, unsigned short size);
	
	//Template function which template set the lastest value associated to a channel
	//Example:		channelBuffer.setValue<int>(0x60, 10);
	template <typename T>
	T setValue(unsigned short id, T value){
		setValue(id, (byte*)&value, sizeof(T));
	}

	//Send all buffer data on a serial port in the format	ID(2 byte) Data(n byte) 
	void sendOnStream(UARTClass* stream);

	//Return the buffer size in byte
	unsigned short getBufferSize(){ return bufferSize; }


private:
	//Buffer size
	unsigned short bufferSize;

	//Vector containing a ByteBuffer instance for each channel
	Vector<ByteBuffer> buffer;

	//Function to convert an unsigned integer to string
	String uintToString(Channel* channel, byte* data);
	

	//Function to convert a signed integer to string
	String intToString(Channel* channel, byte* data);

};

extern ChannelsBufferClass channelsBuffer;


#endif

