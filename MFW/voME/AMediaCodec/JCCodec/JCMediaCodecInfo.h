#ifndef __JC_MEDIA_CODEC_INFO__H
#define __JC_MEDIA_CODEC_INFO__H

#include "JCObject.h"

class JCMediaCodecInfo : public JCObject
{
public:
	JCMediaCodecInfo(JNIEnv* env);
	virtual ~JCMediaCodecInfo();
public:
	jstring      getName();
	jobjectArray getSupportedTypes();
	bool         isEncoder();
protected:
	bool  onInit();
private:	
	jmethodID   mGetName;
	jmethodID   mGetSupportedTypes;
	jmethodID   mIsEncoder;
};

#endif // __JC_MEDIA_CODEC_INFO__H

