#ifndef __CheckUseTime_H__
#define __CheckUseTime_H__

#include "windows.h"
class CheckUseTime
{
public:
	CheckUseTime(void);
	~CheckUseTime(void);
	void CheckTimeIn();
	void CheckTimeOut();
	int TimeDelay();
private:
	
};

#endif