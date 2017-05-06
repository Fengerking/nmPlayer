#ifndef __JC_MEDIA_CODEC_LIST__H
#define __JC_MEDIA_CODEC_LIST__H

#include "JCObject.h"
class JCMediaCodecInfo;

class JCMediaCodecList : public JCObject
{
public:
	JCMediaCodecList(JNIEnv* env);
	virtual ~JCMediaCodecList();
public:
	int    getCodecCount();
	JCMediaCodecInfo* getCodecInfoAt(int index);
protected:
	bool  onInit();
private:	
	jmethodID   mGetCodecCount;
	jmethodID   mGetCodecInfoAt;
};

#endif // __JC_MEDIA_CODEC_LIST__H
