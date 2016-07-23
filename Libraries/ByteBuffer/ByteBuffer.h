// ByteBuffer.h

#ifndef _BYTEBUFFER_h
#define _BYTEBUFFER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Utils.h"

class ByteBuffer {

public:
	ByteBuffer();
	ByteBuffer(int capacity);
	ByteBuffer(byte* b, int size);
	~ByteBuffer();

	void append(byte b);
	void append(byte* b, int size);
	byte at(int pos);
	void clear();
	byte* data();
	void debug();
	void fill(byte b);
	int indexOf(byte b, int from = 0);
	int indexOf(byte* b, int lenght, int from = 0);
	int indexOf(const char* b, int from = 0);
	void insert(byte b, int pos);
	void prepend(byte b){ insert(b, 0); }
	void remove(int pos, int lenght = 1);
	void removeFirst(){ remove(0); };
	void removeLast(){ remove(size - 1); };
	void resize(int newSize);
	void set(byte b, int pos);
	ByteBuffer subArray(int pos, int size);
	void swap(int i, int j);
	String toString();
	String toString(int from, int to);
	String toBinString(int endianness);
	String toHexString();
	

	int getSize(){ return size; }
	int getCapacity() { return capacity; }

	byte& operator[](int i){ return buffer[i]; }
	
	template <typename T>
	friend void operator<<(ByteBuffer& b, T v){ b.append((byte*)(&v), sizeof(v)); }

	template <typename T>
	friend void operator>>(ByteBuffer& b, T& v){ memcpy(&v, b.buffer, sizeof(v)); b.remove(0, sizeof(v)); }

	template <typename T>
	T as(){ return *(reinterpret_cast<T*>(buffer)); }

	/*
	friend void operator<<(ByteBuffer& b, char v){ b.append((byte*)(&v), sizeof(v)); }
	friend void operator<<(ByteBuffer& b, short v){ b.append((byte*)(&v), sizeof(v)); }
	friend void operator<<(ByteBuffer& b, int v){ b.append((byte*)(&v), sizeof(v)); }
	friend void operator<<(ByteBuffer& b, long int v){ b.append((byte*)(&v), sizeof(v)); }
	friend void operator<<(ByteBuffer& b, float v){ b.append((byte*)(&v), sizeof(v)); }

	friend void operator>>(ByteBuffer& b, char& v){ memcpy(&v, b.buffer, sizeof(v)); b.remove(0, sizeof(v)); }
	friend void operator>>(ByteBuffer& b, short& v){ memcpy(&v, b.buffer, sizeof(v)); b.remove(0, sizeof(v)); }
	friend void operator>>(ByteBuffer& b, int& v){ memcpy(&v, b.buffer, sizeof(v)); b.remove(0, sizeof(v)); }
	friend void operator>>(ByteBuffer& b, long int& v){ memcpy(&v, b.buffer, sizeof(v)); b.remove(0, sizeof(v)); }
	friend void operator>>(ByteBuffer& b, float& v){ memcpy(&v, b.buffer, sizeof(v)); b.remove(0, sizeof(v)); }
	*/

private:
	int size, capacity;
	byte* buffer;
};

#endif

