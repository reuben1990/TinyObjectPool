#ifndef __TINY_OBJECT_POOL__
#define __TINY_OBJECT_POOL__

#include <new>
#include <cstdlib>
#include <cstring>
#include <cassert>

template<class T>

class TinyObjectPool {

public:

	TinyObjectPool()
	: mFreeHead(NULL)
	, mObjectSize(sizeof(T))
    , mPtrLen(sizeof(T *))
    , mListLen(0) {
		//make sure we have enough space to store a pointer
		if (mObjectSize < mPtrLen)
			mObjectSize = mPtrLen;
	}

	~TinyObjectPool() {
        if (mListLen > 0)
            freeInstance(0, mListLen - 1);
	}

	//try retrieve one recycled element from the list
	T* newInstance() {
		char *mem;
		if (!mFreeHead) {
            //no useable object in pool, malloc one
			mem = (char *)malloc(mObjectSize);
		} 
		else {
            //retrive one dump mem from list
			T *next = getNextPtr(mFreeHead);
			mem = (char *)mFreeHead;
			mFreeHead = next;
		}
		return new(mem) T();
	}

	//destruct one element, and link it to the recycle list
	void recycleInstance(T* inst) {
		inst->~T();
        modifyNextPtr(inst, mFreeHead);
        //set the head to inst
        mFreeHead = inst;
        ++ mListLen;
	}

private:

	//modify one element's head to point to next element
	void modifyNextPtr(T *element, T *nextElement) {
		if (nextElement)
			//set inst's first sizeOf(ptr) memory to pre head address
			memcpy(element, &nextElement, mPtrLen);
		else
			//set inst's first sizeOf(ptr) memory to NULL, for it's the only element
			memset(element, 0, mPtrLen);
	}

	//get next element whose address is written in current element's head
	T* getNextPtr(T *element) {
        T* ret;
        memcpy(&ret, element, mPtrLen);
		return ret;
	}

	//free some unused element, and link the rest again
	void freeInstance(size_t begin, size_t end) {

		assert(end >= begin && "end position must bigger than begin position");
		assert(begin >= 0 && "begin position must be positive or zero");
		assert(end < mListLen && "end position over border");

		size_t counter = 0;
		T *beforeFaultElement = NULL;
		T *afterFaultElement  = NULL;
		T *elementIter        = mFreeHead;
		while (elementIter) {
			if (counter >= begin) {
				if (counter == end) {
					afterFaultElement = getNextPtr(elementIter);
					free(elementIter);
					break;
				} else {
					T *currentElementToFree = elementIter;
					elementIter = getNextPtr(elementIter);
					free(currentElementToFree);
				}

			} else {
				beforeFaultElement = elementIter;
				elementIter = getNextPtr(elementIter);
			}
			++ counter;
		}
		if (beforeFaultElement) {
			modifyNextPtr(beforeFaultElement, afterFaultElement);
		} else {
			mFreeHead = afterFaultElement;
		}
		size_t freeLen = end - begin + 1;
		mListLen -= freeLen;
	}

	T *mFreeHead;				//linkList head
	size_t mObjectSize;			//size of object
    size_t mPtrLen;				//size of pointer
    size_t mListLen;			//size of link list
};

#endif