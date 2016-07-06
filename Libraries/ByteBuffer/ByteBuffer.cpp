#include "ByteBuffer.h"


ByteBuffer::ByteBuffer(){
	this->size = this->capacity = 0;
	this->buffer = NULL;
}

ByteBuffer::ByteBuffer(int capacity){
	this->size = 0;
	this->capacity = capacity;
	this->buffer = new byte[capacity];
	ASSERT(buffer != NULL, F("ByteBuffer::ByteBuffer Buffer null"));
}

ByteBuffer::ByteBuffer(byte* b, int lenght){
	this->size = 0;
	this->capacity = lenght;
	this->buffer = new byte[lenght];
	append(b, lenght);
}

ByteBuffer::~ByteBuffer(){
	//LOGLN(F("ByteBuffer deleted"));
	delete[] buffer;
}


void ByteBuffer::append(byte b){
	ASSERT(size + 1 <= capacity, F("ByteBuffer::append Buffer full"));
	buffer[size++] = b;
}

void ByteBuffer::append(byte* b, int lenght){
	ASSERT(size + lenght <= capacity, F("ByteBuffer::append size + lenght > capacity"));
	memcpy(&buffer[size], b, lenght);
	size += lenght;

}

byte ByteBuffer::at(int pos){
	ASSERT(pos < size, F("ByteBuffer::at pos > size"));
	return buffer[pos];
}

void ByteBuffer::clear(){
	fill(0);
	this->size = 0;
}

byte* ByteBuffer::data(){
	return buffer;
}

void ByteBuffer::debug(){
	LOG_ARR(buffer, size, HEX);
}

void ByteBuffer::fill(byte b){
	this->size = this->capacity;
	memset(buffer, b, capacity);
}

int ByteBuffer::indexOf(byte b, int from){
	int i = from;
	while (i < size && buffer[i] != b)i++;

	if (i < size)return i;

	return -1;
}

int ByteBuffer::indexOf(byte* b, int lenght, int from){
	int found = -1;
	int index = from - 1;

	do{
		index = indexOf(b[0], index + 1);
		found = strncmp((char*)(&buffer[index]), (char*)b, lenght);
	} while (index != -1 && found != 0);

	return found != 0 ? -1 : index;

}

int ByteBuffer::indexOf(const char* b, int from){
	return indexOf((byte*)b, strlen(b), from);
}

void ByteBuffer::insert(byte b, int pos){
	ASSERT(pos < size, F("ByteBuffer::insert Pos > size"));
	ASSERT(pos + 1 < capacity, F("ByteBuffer::insert pos + 1 > capacity"));

	for (int i = size; i > pos; i--){
		buffer[i] = buffer[i - 1];
	}

	buffer[pos] = b;
	size++;
}

void ByteBuffer::remove(int pos, int length){
	ASSERT(pos < size, F("ByteBuffer::remove Pos > size"));

	for (int i = pos; i < size - length; i++){
		buffer[i] = buffer[i + length];
	}

	size -= length;
}

void ByteBuffer::resize(int newSize){
	byte* old = buffer;

	buffer = new byte[newSize];
	ASSERT(buffer != NULL, F("ByteBuffer::resize Buffer null"));

	if (!old){
		memcpy(buffer, old, size);
		capacity = newSize;
		delete[] old;
	}
}

void ByteBuffer::set(byte b, int pos){
	ASSERT(pos < size, F("ByteBuffer::set Pos > size"));
	buffer[pos] = b;
}

ByteBuffer ByteBuffer::subArray(int pos, int lenght){
	ASSERT(pos + lenght <= size, F("ByteBuffer::subArray pos + size > size"));
	return ByteBuffer(&buffer[pos], lenght);
}

void ByteBuffer::swap(int i, int j){
	ASSERT(i < size, F("ByteBuffer::swap First pos invalid"));
	ASSERT(j < size, F("ByteBuffer::swap Second pos invalid"));

	byte tmp = buffer[i];
	buffer[i] = buffer[j];
	buffer[j] = tmp;
}

String ByteBuffer::toString(){
	String s;
	s.reserve(size + 1);
		
	for (int i = 0; i < size; i++){
				s += (char)buffer[i];
	}

	//s += '\0';

	return s;
}

String ByteBuffer::toString(int from, int to){
	String s;
	s.reserve(to - from + 1);
	
	for(; from < to; from++){
		s += (char)buffer[from];
	}

	//s+='\0';

	return s;
}

String ByteBuffer::toBinString(int endiannes){
	String s, byte;
	s.reserve(size * 8 + 1);
	
	if(endiannes == LSBFIRST){
		for (int i = 0; i < size; i++){
			byte = String(buffer[i], BIN);
			for (int j = 0; j < 8 - byte.length(); j++){
				s += '0';
			}
			s += byte;
		}
	}
	else{
		for (int i = size - 1; i >= 0; i--){
			byte = String(buffer[i], BIN);
			for (int j = 0; j < 8 - byte.length(); j++){
				s += '0';
			}
			s += byte;
		}
	}
	
	s+='\0';

	return s;
}

String ByteBuffer::toHexString(){
	String s, byte;
	s.reserve(size * 2 + 1);
	for (int i = 0; i < size; i++){
		byte = String(buffer[i], HEX);
		for(int j = 0; j < 2 - byte.length(); j++){
			s += '0';
		}
		s += byte;
	}
	s += '\0';

	return s;
}

