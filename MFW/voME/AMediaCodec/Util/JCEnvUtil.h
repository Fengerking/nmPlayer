/*
  Limitation : Using this class from Heap: new JCEnvUtil
 */
#ifndef __JCEnvUtil_H__
#define __JCEnvUtil_H__

#include <pthread.h>


class JCEnvUtil
{
protected:
	bool 			m_fNeedDetach;
	JavaVM 			*mJavaVM;
	JNIEnv 			*m_pEnv;
	bool            mIsAttachThread;
	JCEnvUtil(const JCEnvUtil&);
	JCEnvUtil& operator=(const JCEnvUtil&);
private:
	static pthread_once_t   mCKeyThread;
	static pthread_key_t 	mEnvUtilKey;
	static void             createKey(void);
	static void             onThreadExit(void* pParam);
	
public:
	JCEnvUtil(JavaVM *pvm, bool isAttachThread=true);
	~JCEnvUtil();
	
	JNIEnv* getEnv();
};

#endif
