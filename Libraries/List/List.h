// List.h

#ifndef _LIST_h
#define _LIST_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Ref.h"
#include "Pool.h"

template <typename T>
class Node : public Poollable{

public:
	Node<T>(){
		this->next = NULL;
	}

	Node<T>(T value){
		this->value = value;
		this->next = NULL;
	}

	Node<T>(T value, Node* next){
		this->value = value;
		this->next = next;
	}

	void reset(){
		next = NULL;
	}

	T value;
	Node* next;
};

template <typename T>
class List{

public:
	
	List<T>(int maxPoolObj = 16, int initPoolWith = 0){
		pCurrent = pFront = pRear = NULL;
		nodePool = new Pool<Node<T> >(maxPoolObj, initPoolWith);
	}

	~List<T>(){
		clear();
	}

	void append(T value){
		Node<T>* node = nodePool->obtain();
		node->value = value;
		if (pFront == NULL){
			pFront = pRear = node;
		}
		else{
			pRear->next = node;
			pRear = node;
		}
	}

	void clear(){
		reset();
		while (next()){
			remove();
		}
	}

	T& current(){
		ASSERT(pCurrent != NULL, F("List::current invalid call to current"));
		return pCurrent->value;
	}

	void debug(){
		LOG(F("List"));
		reset();
		while (next()){
			Serial.print(current()); Serial.print(" ");
		}
		Serial.println();
	}

	void findNext(T value){
		while (next() && pCurrent->value != value);
	}

	boolean isEmpty(){
		return pFront == NULL;
	}

	boolean next(){
		if (pCurrent == NULL){
			pCurrent = pFront;
		}
		else{
			pCurrent = pCurrent->next;
		}

		return pCurrent != NULL;
	}

	void prepend(T value){
		if (pFront == NULL){
			append(value);
		}
		else{
			Node<T>* node = nodePool->obtain();
			node->value = value;			
			node->next = pFront;
			//node->retain();
			pFront = node;
		}
	}

	void previous(){
		Node<T>* c = pCurrent;
		reset();
		while (next() && pCurrent->next != c);
	}

	void reset(){
		pCurrent = NULL;
	}

	void remove(){
		if (pCurrent != NULL){
			
			if (pCurrent == pFront){
				pFront = pFront->next;
				nodePool->free(pCurrent);
				//pCurrent->release();
				pCurrent = NULL;
			}
			else {
				Node<T>* e = pCurrent;
				previous();
				pCurrent->next = e->next;
				nodePool->free(e);
				//e->release();
			}

			if (pCurrent->next == NULL){
				pRear = pCurrent;
			}

		}
	}

	void insertAfter(T value){
		Node<T>* node = nodePool->obtain();
		Node<T>* after = pCurrent->next;
		node->value = value;
		node->next = after;
		//node->retain();
		pCurrent->next = node;
		

		if (after == NULL){
			pRear = node;
		}
	}

	void insertPrev(T value){
		if (pCurrent == pFront){
			prepend(value);
		}
		else{
			Node<T>* node = nodePool->obtain();
			node->value = value;
			//node->retain();

			Node<T>* curr = pCurrent;
			previous();

			pCurrent->next = node;
			node->next = curr;
			pCurrent = curr;
		}
	}

	T value(){
		ASSERT(pCurrent != NULL, F("List::value invalid call to value"));
		return pCurrent->value;
	}

protected:
	static Pool<Node<T> >* nodePool;

	Node<T>* pFront;
	Node<T>* pRear;
	Node<T>* pCurrent;
};

template<typename T>
Pool<Node<T> >* List<T>::nodePool = NULL;

#endif

