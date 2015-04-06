#ifndef __TINY_OBJECT_POOL__
#define __TINY_OBJECT_POOL__

#include <new>
#include <cstdlib>
#include <cstring>

template<class T>
class TinyObjectPool {
public:
	TinyObjectPool()
	: mFreeHead(NULL)
	, mObjectSize(sizeof(T)) {
        mPtrLen = sizeof(T *);
		//make sure we have enough space to store a pointer
		if (mObjectSize < mPtrLen)
			mObjectSize = mPtrLen;
	}

	T* newInstance() {
		char *mem;
		if (!mFreeHead) {
            //no useable object in pool, malloc one
			mem = (char *)malloc(mObjectSize);
		} else {
            //retrive one dump mem from list
			char *next = (char *)&mFreeHead;
			mem = (char *)mFreeHead;
			mFreeHead = (T *)next;
		}
		return new(mem) T();
	}

	void recycleInstance(T* inst) {
		inst->~T();
        if (mFreeHead) {
            //set inst's first sizeOf(ptr) memory to pre head address
            memcpy(inst, &mFreeHead, mPtrLen);
        } else {
            //set inst's first sizeOf(ptr) memory to NULL, for it's the only element
			memset(inst, 0, mPtrLen);
        }
        //set the head to inst
        mFreeHead = inst;
	}
private:
	T *mFreeHead;
	size_t mObjectSize;
    size_t mPtrLen;
};

#endif