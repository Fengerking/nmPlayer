/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CJniEnvUtil.h

	Contains:	CMobiPlayer header file

	Written by:	Wenwei Tao

	Change History (most recent first):
	2011-02-22		JBF			Create file

*******************************************************************************/
#ifndef __CJniEnvUtil_H__
#define __CJniEnvUtil_H__

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CJniEnvUtil
{
protected:
	bool 				m_fNeedDetach;
	JavaVM 			*mJavaVM;
	JNIEnv 			*m_pEnv;
	
	CJniEnvUtil(const CJniEnvUtil&);
	CJniEnvUtil& operator=(const CJniEnvUtil&);
public:
	CJniEnvUtil(JavaVM *pvm);
	~CJniEnvUtil();
	
	JNIEnv* getEnv() { return m_pEnv; } 
};

#ifdef _VONAMESPACE
}
#endif


#endif
