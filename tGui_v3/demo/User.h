#ifndef _USER_H_
#define _USER_H_
#include "tObject/tWidget.h"
class User :public tWidget
{
public:
	User(const char*  n, tObject* obj);
	~User() {};
	virtual void show();
	void slot() { printf("depress_solt\n"); }
	void slot1() { printf("hello world\n"); }
	void slot2() { printf("release_slot\n"); }
};


#endif // !_USER_H_


