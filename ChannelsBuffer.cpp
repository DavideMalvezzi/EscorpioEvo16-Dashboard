#include "ChannelsBuffer.h"

void ChannelsBufferClass::init(){
	Channel* c;
	int size = channelsConfig.getChannelCount();
	buffer.resize(size);

	bufferSize = 0;
	for (int i = 0; i < size; i++){
		c = channelsConfig.getChannelByIndex(i);
		buffer[i].resize(c->size);
		buffer[i].fill(0x0);

		bufferSize += c->size;
	}

	updateFlags.resize(size);
	updateFlags.fill(0);
}

void ChannelsBufferClass::debug(){
	LOGLN("=========== Channels data buffer: ===========");
	for (int i = 0; i < buffer.getCapacity(); i++){
		LOG(channelsConfig.getChannelByIndex(i)->name);
		LOG(": ");
		buffer[i].debug();
	}
	LOGLN("========================================");
}

String ChannelsBufferClass::getValueAsString(unsigned short id){
	int index = channelsConfig.getChannelIndex(id);
	if (index != -1){
		Channel* c = channelsConfig.getChannelByIndex(index);
		updateFlags.clearBit(index);

		switch (c->type){
			case BIT_FLAG:
				return buffer[index].toBinString();

			case DECIMAL:
				if (c->size <= 4){
					return String(buffer[index].as<float>(), 6);
				}
				return String(buffer[index].as<double>(), 6);

			case INTEGER:
				return String((double)convertMemToInt(c, buffer[index].data()));

			case U_INTEGER:
				return String((double)convertMemToUint(c, buffer[index].data()));

			case STRING:
				return buffer[index].toString();
		}
	}

	return "nil";
}



ByteBuffer ChannelsBufferClass::getValueAsByteArray(unsigned short id){
	int index = channelsConfig.getChannelIndex(id);
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
			LOG(F("WARNING: ChannelsBuffer::setValue  expected size != received size for channels ")); LOGLN(id);
		}

		buffer[index].clear();
		buffer[index].append(data, size);
		updateFlags.setBit(index);
	}
	else {
		LOG(F("WARNING: ChannelsBuffer::setValue	unknow packet id = ")); LOGLN(id);
	}
	
}

bool ChannelsBufferClass::isValueUpdated(unsigned short id){
	int index = channelsConfig.getChannelIndex(id);
	if (index != -1){
		return updateFlags.getBit(index);
	}
	return false;
}

unsigned long long int ChannelsBufferClass::convertMemToUint(Channel* channel, byte* data){
	byte mem[8];

	for (int i = 0; i < channel->size; i++){
		mem[i] = data[i];
	}

	for (int i = channel->size; i < sizeof(long long int); i++){
		mem[i] = 0x00;
	}

	return *(reinterpret_cast<unsigned long long int*>(mem));
}

signed long long int ChannelsBufferClass::convertMemToInt(Channel* channel, byte* data){
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
}

ChannelsBufferClass channelsBuffer;