#ifndef __JC_CRYPTO_INFO__H
#define __JC_CRYPTO_INFO__H

#include "JCObject.h"
class JCCryptoInfo : public JCObject
{
public:
	JCCryptoInfo(JNIEnv* env);
	virtual ~JCCryptoInfo();
public:
	bool     createCryptoInfo(const unsigned char* iv, int ivLen, const unsigned char* key, int keyLen, int* numBytesOfCD, int* numBytesOfED, int nums, int nMode);
protected:
	bool onInit();
private:	
	jmethodID   mSet;
	jmethodID   mCons;
};
#endif // __JC_CRYPTO_INFO__H
