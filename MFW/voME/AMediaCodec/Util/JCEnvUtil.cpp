#include <jni.h>
#include <assert.h>
#include "JCEnvUtil.h"

//#define __DEBUG

#define  LOG_TAG    "JCEnvUtil"
#include "JCObject.h"

pthread_once_t JCEnvUtil::mCKeyThread = PTHREAD_ONCE_INIT;
pthread_key_t  JCEnvUtil::mEnvUtilKey = NULL;

JCEnvUtil::JCEnvUtil(JavaVM *pvm, bool isAttachThread):mIsAttachThread(isAttachThread)
													  , m_fNeedDetach(false)
													  , mJavaVM(pvm)
													  , m_pEnv(NULL)
{
	switch (mJavaVM->GetEnv((void**)&m_pEnv, JNI_VERSION_1_6)) { 
		case JNI_OK: 
			break; 
		case JNI_EDETACHED: 
			m_fNeedDetach = true;
			__D("Thread id = %d\n", pthread_self());
			if (mJavaVM->AttachCurrentThread(&m_pEnv, NULL) != 0) { 
				LOGE("callback_handler: failed to attach current thread");
				break;
			} 			
			break; 
		case JNI_EVERSION: 
			LOGE("Invalid java version"); 
			break;
		}
}

JCEnvUtil::~JCEnvUtil()
{
	if (m_fNeedDetach) 
		 mJavaVM->DetachCurrentThread(); 
}
 
JNIEnv* JCEnvUtil::getEnv()
{
	int nOk = -1;

	if(mIsAttachThread)
	{
		nOk = pthread_once(&mCKeyThread, createKey);
		assert(nOk == 0);
		void *pTr = NULL;
		__D("ThreadId = %d\n", pthread_self());
		if((pTr = pthread_getspecific(mEnvUtilKey)) == NULL)
		{
			pthread_setspecific(mEnvUtilKey, this);	
			return m_pEnv;
		}
		return ((JCEnvUtil*)pTr)->m_pEnv; 
	}
	
	return m_pEnv;
} 

void JCEnvUtil::createKey()
{
	int nOk = -1;
	nOk = pthread_key_create(&mEnvUtilKey, onThreadExit);
	if(0 != nOk)
	{
		__D("Can not create the mEnvUtilKey\n");
	}
}
void JCEnvUtil::onThreadExit(void* pParam)
{
	if(pParam)
	{
		JCEnvUtil* pUtil = (JCEnvUtil*) pParam;	
		__D("pUtil = %p ThreadId = %d\n", pUtil, pthread_self());
		if(pUtil->mJavaVM && pUtil->m_fNeedDetach)
		{
			__D("Thread = %d\n", pthread_self());
			pUtil->mJavaVM->DetachCurrentThread(); 
			__D();
			pUtil->m_fNeedDetach = false;
			__D();
		}
	}
		
}

