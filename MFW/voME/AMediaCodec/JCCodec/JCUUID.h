#ifndef __JC_UUID__H
#define __JC_UUID__H

#include "JCObject.h"

class JCUUID : public JCObject
{
public:
	JCUUID(JNIEnv* env);
	virtual ~JCUUID();

public:
	bool createFromString(const char* inStr);
	bool createFromBytes(const unsigned char* inBytes, int inLen=128);
protected:
	bool onInit();
private:
	jmethodID        mFromString;
	jmethodID        mNameUUIDFromBytes;
};
#endif // __JC_UUID__H
