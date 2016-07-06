
#include "BitArray.h"

BitArray::BitArray(){
	this->size = 0;
	this->bits = NULL;
}
BitArray::BitArray(int size){
	this->size = size;
	this->bits = new byte[(int)ceil((float)size / 8)];
}
BitArray::~BitArray(){
	delete[] bits;
}

void BitArray::clearBit(int index){
	int q = index / 8;
	int r = index % 8;
	bits[q] &= ~(1 << r);

}
void BitArray::fill(bool value){
	for (int i = 0; i < size; i++){
		value ? setBit(i) : clearBit(i);
	}
}

bool BitArray::getBit(int index){
	int q = index / 8;
	int r = index % 8;
	return bits[q] & (1 << r);
}

int BitArray::getSize(){
	return size;
}

void BitArray::resize(int newSize){
	byte* old = bits;

	bits = new byte[(int)ceil((float)newSize / 8)];
	ASSERT(bits != NULL, F("BitArray::resize Buffer null"));

	if (!old){
		memcpy(bits, old, min(size, newSize));
		size = newSize;
		delete[] old;
	}
}

void BitArray::setBit(int index){
	int q = index / 8;
	int r = index % 8;
	bits[q] |= (1 << r);
}

String BitArray::toString(){
	String str;
	str.reserve(size);
	for (int i = 0; i < size; i++){
		str.concat(getBit(i) ? '1' : '0');
	}
	return str;
}

