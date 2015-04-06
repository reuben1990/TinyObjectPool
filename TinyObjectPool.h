#ifndef __TINY_OBJECT_POOL__
#define __TINY_OBJECT_POOL__

#include <new>
#include <cstdlib>
#include <cstring>

const size_t kPtrLen = sizeof(void *);

template<class T>
class TinyObjectPool {
public:
	TinyObjectPool()
	: mFreeHead(NULL)
	, mObjectSize(sizeof(T)) {
		//make sure we have enough space to store a pointer
		if (mObjectSize < kPtrLen)
			mObjectSize = kPtrLen;
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
            memcpy(inst, &mFreeHead, kPtrLen);
        } else {
            //set inst's first sizeOf(ptr) memory to NULL, for it's the only element
			memset(inst, 0, kPtrLen);
        }
        //set the head to inst
        mFreeHead = inst;
	}
private:
	T *mFreeHead;
	size_t mObjectSize;
};

#endif