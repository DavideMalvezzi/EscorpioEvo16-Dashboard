// Pool.h

#ifndef _POOL_h
#define _POOL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Ref.h"
#include "RefVector.h"

class Poollable{
public:
	virtual void reset() = 0;

};

template <typename T>
class Pool {

public:
	Pool<T>(int maxSize, int initObjs = 0){
		pool.resize(maxSize);
		for (int i = 0; i < initObjs; i++){
			pool.append(new T);
		}
	}

	~Pool<T>(){
		pool.clear();
	}

	boolean full(){
		return pool.full();
	}

	T* obtain(){
		if (pool.getSize() > 0){
			//LOG("Return used obj");
			//Use take because doesn't modify the refCount
			return pool.take(0);
		}
		T* r = new T;
		//r->retain();
		//LOG("Return new obj");
		return r;
	}

	void free(T* obj){
		if (!pool.full()){
			obj->reset();
			pool.append(obj);
			//LOG("Free used obj");
		}
		else{
			LOG("Release used obj, pool full");
			delete obj;
		}
		//Release the obj because if is added reference count is > 1 else if not added need to be deleted
		//obj->release();
	}

	int getCapacity(){
		pool.getCapacity();
	}

	int getSize(){
		pool.getSize();
	}


protected:
	Vector<T*> pool;

};


#endif

