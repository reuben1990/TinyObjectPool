#include "TinyObjectPool.h"
#include <cstdio>

class Object {
public:
	bool init(char param1, short param2, int param3, long long param4) {
		mMember1 = param1;
		mMember2 = param2;
		mMember3 = param3;
		mMember4 = param4;
        return true;
	}

	void printSelf() {
		printf("self addr = %p\n", this);
	}

private:
	char mMember1;
	short mMember2;
	int mMember3;
	long long mMember4;
};


int main() {
	TinyObjectPool<Object> pool;
	
	Object* obj1 = pool.newInstance();
	obj1->init('a', 1, 1, 1);
	obj1->printSelf();

	Object* obj2 = pool.newInstance();
	obj2->init('b', 2, 2, 2);
	obj2->printSelf();

	pool.recycleInstance(obj1);
    pool.recycleInstance(obj2);

	Object* obj3 = pool.newInstance();
	obj3->init('c', 3, 3, 3);
	obj3->printSelf();
    
    return 0;
}