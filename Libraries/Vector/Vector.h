// Vector.h

#ifndef _VECTOR_h
#define _VECTOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Utils.h"

template <typename T>
class Vector {

public:
	Vector<T>(){
		this->size = this->capacity = 0;
		this->vector = NULL;
	}

	Vector<T>(int capacity){
		this->size = 0;
		this->capacity = capacity;
		this->vector = new T[capacity];
	}

	~Vector<T>(){
		delete[] vector;
	}


	void append(T b){
		ASSERT(size + 1 <= capacity, F("Vector::append Buffer full"));
		vector[size++] = b;
	}

	T at(int pos){
		ASSERT(pos < size, F("Vector::at pos > size"));
		return vector[pos];
	}

	void clear(){
		this->size = 0;
	}

	T* data(){
		return vector;
	}

	void debug(){
		LOG("Vector: ");
		LOG_ARR(vector, size, DEC);
	}

	boolean full(){
		return size == capacity;
	}

	int indexOf(T b, int from = 0){
		int i = from;
		while (i < size && vector[i] != b)i++;

		if (i < size)return i;

		return -1;
	}

	void insert(T b, int pos){
		ASSERT(pos < size, F("Vector::insert Pos > size"));
		ASSERT(pos + 1 < capacity, F("Vector::insert pos + 1 > capacity"));
		ASSERT(size + 1 <= capacity, F("Vector::insert vector full"));

		for (int i = size; i > pos; i--){
			vector[i] = vector[i - 1];
		}

		vector[pos] = b;
		size++;

	}

	void prepend(T b){
		insert(b, 0);
	}

	void remove(int pos, int lenght = 1){
		ASSERT(pos < size, F("Vector::remove pos > size"));
		ASSERT(pos > 0, F("Vector::remove pos < 0"));

		for (int i = pos; i < size - lenght; i++){
			vector[i] = vector[i + lenght];
		}

		size -= lenght;
	}

	void removeObj(T obj){
		int i = indexOf(obj);
		remove(i);
	}

	void removeFirst(){
		remove(0);
	}

	void removeLast(){
		remove(size - 1);
	}

	void resize(int newSize){
		T* old = vector;
		capacity = newSize;
		vector = new T[newSize];
		ASSERT(vector, F("Vector::resize vector null"));

		if (!old){
			memcpy(vector, old, size);
			delete[] old;
		}
	}

	void set(int pos, T b){
		ASSERT(pos < size, F("Vector::set pos > size"));
		vector[pos] = b;
	}

	void swap(int i, int j){
		ASSERT(i < size, F("Vector::swap first pos invalid"));
		ASSERT(j < size, F("Vector::swap second pos invalid"));

		T tmp = vector[i];
		vector[i] = vector[j];
		vector[j] = tmp;
	}

	T take(int pos){
		T v = vector[pos];
		remove(pos);
		return v;
	}

	int getSize(){ return size; }

	int getCapacity(){ return capacity; }

	T& operator[](int i){ return vector[i]; }

protected:
	int size, capacity;
	T* vector;
};
#endif

