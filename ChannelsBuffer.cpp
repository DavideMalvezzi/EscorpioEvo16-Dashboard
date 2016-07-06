
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
		buffer[i].fill(0x0);

		bufferSize += c->size;
	}

	//Resize the updated bit flags
	updateFlags.resize(size);
	updateFlags.fill(0);
}

void ChannelsBufferClass::debug(){
	LOGLN(F("=========== Channels data buffer: ==========="));
	for (int i = 0; i < buffer.getCapacity(); i++){
		LOG(channelsConfig.getChannelByIndex(i)->name);
		LOG(F(": "));
		buffer[i].debug();
	}
	LOGLN(F("========================================"));
}

String ChannelsBufferClass::getValueAsString(unsigned short id, bool clearUpdatedFlag){
	int index = channelsConfig.getChannelIndex(id);
	//If channel's config found
	if (index != -1){
		Channel* c = channelsConfig.getChannelByIndex(index);

		//Clear updated flag if requested
		if (clearUpdatedFlag){
			updateFlags.clearBit(index);
		}

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
				return String(buffer[index].as<double>(), 6);

			case Channel::INTEGER:
				//Need to convert to double because not exist 8 byte int String constructor 
				return String((double)convertMemToInt(c, buffer[index].data()));

			case Channel::U_INTEGER:
				//Need to convert to double because not exist 8 byte int String constructor 
				return String((double)convertMemToUint(c, buffer[index].data()));

			case Channel::STRING:
				return buffer[index].toString();

			default:
				LOG(F("WARNING: Unknown conversion type channel ")); LOG(id); LOG(" to type "); LOGLN((int)c->type);
		}
	}
	

	//Error
	return "nil";
}



ByteBuffer ChannelsBufferClass::getValueAsByteArray(unsigned short id){
	int index = channelsConfig.getChannelIndex(id);
	//Return a copy 
	if (index != -1){
		ByteBuffer& b = buffer[index];
		return ByteBuffer(b.data(), b.getSize());
	}
	return ByteBuffer();
}

void ChannelsBufferClass::setValue(unsigned short id, byte* data, unsigned short size){
	int index = channelsConfig.getChannelIndex(id);
	if (index != -1){
		if (channelsConfig.getChannelByIndex(index)->size != size){
			LOG(F("WARNING: ChannelsBuffer::setValue  expected size != received size for channel ")); LOGLN(id);
		}

		buffer[index].clear();
		buffer[index].append(data, size);
		updateFlags.setBit(index);
	}
	else {
		LOG(F("WARNING: ChannelsBuffer::setValue  unknow packet id = ")); LOGLN(id);
	}
	
}

bool ChannelsBufferClass::isValueUpdated(unsigned short id){
	int index = channelsConfig.getChannelIndex(id);
	if (index != -1){
		return updateFlags.getBit(index);
	}
	return false;
}

//TODO: Test this
unsigned long long int ChannelsBufferClass::convertMemToUint(Channel* channel, byte* data){
	unsigned long long value = 0;

	for (int i = 0; i < channel->size; i++){
		value |= data[i] << (8 * i);
	}

	return value;

	/*
	byte mem[8];

	for (int i = 0; i < channel->size; i++){
		mem[i] = data[i];
	}

	for (int i = channel->size; i < sizeof(long long int); i++){
		mem[i] = 0x00;
	}

	return *(reinterpret_cast<unsigned long long int*>(mem));
	*/
}

//TODO: Test this
signed long long int ChannelsBufferClass::convertMemToInt(Channel* channel, byte* data){
	long long value = 0;

	for (int i = 0; i < channel->size; i++){
		value |= data[i] << (8 * i);
	}

	if (value & (1 << (8 * channel->size - 1))){
		for (int i = channel->size; i < sizeof(long long int); i++){
			value |= 0xFF << (8 * i);
		}
	}

	return value;

	/*
	byte mem[8];

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

	return *(reinterpret_cast<long long int*>(mem));
	*/
}

ChannelsBufferClass channelsBuffer;