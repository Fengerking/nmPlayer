#ifndef __JC_BUFFER_INFO__H
#define __JC_BUFFER_INFO__H
#include "JCObject.h"


// call sequence
// 1) constructor
// 2) init
// 3) setObj, this value should get from Java side;
// 4) update,
// 5) then can get the right value

class JCBufferInfo: public JCObject
{
public:
	JCBufferInfo(JNIEnv* env);
	virtual ~JCBufferInfo();

public:
	bool   createObject();
	// when you want to get public values, the caller should set obj and call update
	bool   update();
	
protected:
	bool onInit();
public:
	int         mFlags;
	int         mOffset;
	long long   mTimeUs;
	int         mSize;
private:
	jfieldID     mFlagsID;
	jfieldID     mOffsetID;
	jfieldID     mTimeUsID;
	jfieldID     mSizeID;
	jmethodID    mCons;
};
#endif // __JC_BUFFER_INFO__H
