#ifndef __JC_MEDIA_CRYPTO__H
#define __JC_MEDIA_CRYPTO__H
#include "JCObject.h"

class JCUUID;

class JCMediaCrypto : public JCObject
{
public:
	JCMediaCrypto(JNIEnv* env);
	virtual ~ JCMediaCrypto();
public:
	bool createMediaCrypto(JCUUID& pID, const unsigned char* data, int nLen);
	bool isCryptoSchemeSupported(JCUUID& uuid);
	bool requiresSecureDecoderComponent(const char* pMime);
	void release();
protected:
	bool onInit();

private:
	jmethodID    mCons;
	jmethodID    mIsCryptoSchemeSupported;
	jmethodID    mRelease;
	jmethodID    mRequiresSecureDecoderComponent;
};
#endif // __JC_MEDIA_CRYPTO__H
