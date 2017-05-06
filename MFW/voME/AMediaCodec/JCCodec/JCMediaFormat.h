#ifndef __JC_MEDIA_FORMAT__H
#define __JC_MEDIA_FORMAT__H
#include "JCObject.h"

class JCMediaFormat : public JCObject
{
public:
	JCMediaFormat(JNIEnv* env);
	virtual ~JCMediaFormat();

public:
	struct MFInfo
	{
		int    mWidth;
		int    mHeight;
		char*  mMime;
		int    mMaxInputSize;
		float  mFrameRate;
		long   mDuration;
		int    mColorFormat;
		int    mChannels;
		int    mSampleRate;
	};
	
	MFInfo* getMediaInfo(const bool isVideo = true);
	bool    createVideoFormat(const char* mime, int width, int height);
	bool    createAudioFormat(const char* mime, int sampleRate, int channels);
	int     getIntValue(const char* name);
	long    getLongValue(const char* name);
	float   getFloatValue(const char* name);
	char*   getStringValue(const char* name);
	bool    setIntValue(const char* name, int value);
	bool    setLongValue(const char* name, long value);
	bool    setFloatValue(const char* name, float value);
	bool    setStringValue(const char* name, const char* value);
	bool    setByteBuffer(const char* name, unsigned char* bytes, const long inLen);
	jobject getByteBuffer(const char* name);
	// only for debug 
	bool    dump();
	// will be used to set the different filed value, since all the fields have the same type java.lang.String
	/*
	{"width" interger
	 "height"  interger
	 "mime"    string
	 "max-input-size" int
	 "frame-rate"     int / float
	 "durationUs"     long
	 "color-format"   int
	 "channel-count"  int
	 "sample-rate"    int
	 "csd-0"          ByteBuffer
	*/
protected:
	bool onInit();
private:
	jmethodID         mCreateVideoFormat;
	jmethodID         mCreateAudioFormat;
	jmethodID         mToString; // only for debug
	jmethodID         mGetInteger;
	jmethodID         mGetLong;
	jmethodID         mGetFloat;
	jmethodID         mGetString;
	jmethodID         mSetInteger;
	jmethodID         mSetLong;
	jmethodID         mSetFloat;
	jmethodID         mSetString;
	jmethodID         mSetByteBuffer;
	jmethodID         mGetByteBuffer;
	MFInfo*           mInfo;
};
#endif // __JC_MEDIA_FORMAT__H
