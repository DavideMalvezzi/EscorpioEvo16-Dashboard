
#include "ChannelsBuffer.h"

void ChannelsBufferClass::init(){
	Channel* c;
	//Resize the buffer
	int size = channelsConfig.getChannelCount();
	buffer.resize(size);

	bufferSize = 0;
	//Create the "matrix" to cointain byte-array data
	for (int i = 0; i < size; i++){
		c = channelsConfig.getChannelByIndex(i);
		buffer[i].resize(c->size);
		buffer[i].clear();

		bufferSize += c->size;
	}

	//Resize the updated bit flags
	updateFlags.resize(size);
	updateFlags.fill(0);
}

void ChannelsBufferClass::debug(){
	/*
	LOGLN(F("=========== Channels data buffer: ==========="));
	for (int i = 0; i < buffer.getCapacity(); i++){
		LOG(channelsConfig.getChannelByIndex(i)->name);
		LOG(F(": "));
		buffer[i].debug();
	}
	LOGLN(F("========================================"));
	*/
}

String ChannelsBufferClass::getValueAsString(unsigned short id){
	if (channelsConfig.isValid()){
		int index = channelsConfig.getChannelIndex(id);
		//If channel's config found
		if (index != -1){
			Channel* c = channelsConfig.getChannelByIndex(index);

			//Convert to arduino String obj
			switch (c->type){
				case Channel::BIT_FLAG:
					return buffer[index].toBinString();

				case Channel::DECIMAL:
					//If size <= is float
					//Need this difference because conversion is a copy 'n paste of memory
					if (c->size <= 4){
						return String(buffer[index].as<float>(), 6);
					}
					
					return String(buffer[index].as<double>(), 10);

				case Channel::INTEGER:
					return intToString(c, buffer[index].data());

				case Channel::U_INTEGER:
					return uintToString(c, buffer[index].data());

				case Channel::STRING:
					return buffer[index].toString();

				default:
					Log.e(CHBUF_TAG) << F("in getValueAsString\t Unknown conversion type channel ") << id << F(" to type ") << c->type << Endl;
			}
		}
	}
	//Error
	return F("nil");
}

ByteBuffer ChannelsBufferClass::getValueAsByteArray(unsigned short id){
	if (channelsConfig.isValid()){
		int index = channelsConfig.getChannelIndex(id);
		//Return a copy 
		if (index != -1){
			ByteBuffer& b = buffer[index];
			return ByteBuffer(b.data(), b.getSize());
		}
	}
	return ByteBuffer();
}

void ChannelsBufferClass::setValue(unsigned short id, byte* data, unsigned short size){
	if (channelsConfig.isValid()){
		int index = channelsConfig.getChannelIndex(id);
		if (index != -1){

			if (size < channelsConfig.getChannelByIndex(index)->size){
				Log.w(CHBUF_TAG) << F("in setValue\t Received size < expected size for channel ") << id << Endl;
			}
			else if (size > channelsConfig.getChannelByIndex(index)->size){
				Log.w(CHBUF_TAG) << F("in setValue\t Received size > expected size for channel ") << id << Endl;
			}
			
			if (size <= channelsConfig.getChannelByIndex(index)->size){
				buffer[index].clear();
				buffer[index].append(data, size);
				updateFlags.setBit(index);
			}

		}
		else {
			Log.w(CHBUF_TAG) << F("ChannelsBuffer::setValue  unknow packet id = ") << id << Endl;
		}
	}
}

bool ChannelsBufferClass::isValueUpdated(unsigned short id){
	if (channelsConfig.isValid()){
		int index = channelsConfig.getChannelIndex(id);
		if (index != -1){
			return updateFlags.getBit(index);
		}
	}
	return false;
}

void ChannelsBufferClass::invalidAllData(){
	updateFlags.fill(0);
}


//Assuming CPU is LITTLE-ENDIAN
String ChannelsBufferClass::uintToString(Channel* channel, byte* data){
	byte mem[8];
	char temp[21]; //64 bit unsigned integer max digit number + \0
	String value;


	for (int i = 0; i < channel->size; i++){
		mem[i] = data[i];
	}

	for (int i = channel->size; i < sizeof(long long int); i++){
		mem[i] = 0x00;
	}

	sprintf(temp, "%llu", *(reinterpret_cast<unsigned long long int*>(mem)));
	value += temp;
	return value;
}

//Assuming CPU is LITTLE-ENDIAN
String ChannelsBufferClass::intToString(Channel* channel, byte* data){
	byte mem[8];
	char temp[21]; //64 bit signed integer max digit number + \0
	String value;

	for (int i = 0; i < channel->size; i++){
		mem[i] = data[i];
	}

	if (mem[channel->size - 1] & (1 << 7)){
		for (int i = channel->size; i < sizeof(long long int); i++){
			mem[i] = 0xFF;
		}
	}
	else{
		for (int i = channel->size; i < sizeof(long long int); i++){
			mem[i] = 0x00;
		}
	}

	sprintf(temp, "%lld", *(reinterpret_cast<long long int*>(mem)));
	value += temp;
	return value;

}

void ChannelsBufferClass::sendOnStream(UARTClass* stream){
	Channel* c;
	for (int i = 0; i < channelsConfig.getChannelCount(); i++){
		c = channelsConfig.getChannelByIndex(i);
		stream->write((byte*)&c->ID, sizeof(c->ID));
		stream->write(buffer[i].data(), buffer[i].getCapacity());
	}
}

ChannelsBufferClass channelsBuffer;