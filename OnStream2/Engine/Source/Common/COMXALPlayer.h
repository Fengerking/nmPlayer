/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
 *																		*
 ************************************************************************/
/*******************************************************************************
 File:		COMXALPlayer.h
 
 Contains:	VisualOn OpenMAX AL player header file
 
 Written by:	Jim Lin
 
 Change History (most recent first):
 2012-03-24		Jim			Create file
 *******************************************************************************/


#ifndef _COMXAL_PLAYER_H_
#define _COMXAL_PLAYER_H_

#include "voOnStreamType.h"
#include "voOnStreamEngine.h"
#include "COMXALEngine.h"
#include "COSBasePlayer.h"
#include "voType.h"

#ifndef _IOS
#include <jni.h>
#endif

class COMXALPlayer : public COSBasePlayer
{
public:
	COMXALPlayer();
	virtual ~COMXALPlayer();
	
	virtual int		Init();
	virtual int		Uninit();
	
	virtual int		SetDataSource(void * pSource, int nFlag);
	
	virtual int		Run(void);
	virtual int		Pause(void); 
	virtual int		Stop(void);
    virtual int     Close();
	virtual int		Flush(void);
	virtual int		GetStatus(int* pStatus);
	virtual int		GetDuration(int* pDuration);
	
	virtual int		SetView(void* pView);
#ifndef _IOS
	virtual	int		SetJavaVM(void *pJavaVM, void* obj);
	virtual void*	GetJavaObj(){ return (void*) m_jOSmpEng;	};
#endif
    
	virtual int		GetAudioData (unsigned char * pData, unsigned int nSize, int * pFilled);
	
	virtual int		GetPos(int* pCurPos);
	virtual int		SetPos(int nCurPos);
	
	virtual int		GetParam(int nID, void* pValue);
	virtual int		SetParam(int nID, void* pValue);

	virtual int		ChangEventIDEvent (unsigned int nID, void * pParam1, void * pParam2);
	virtual int		HandleEvent(int nID, void * pParam1, void * pParam2);	
private:
	static int		OnNPReadAudio(void* pUserData, VONP_BUFFERTYPE* pBuffer);
	virtual int		doNPReadAudio(VONP_BUFFERTYPE* pBuffer);
	
	static int		OnNPReadVideo(void * pUserData, VONP_BUFFERTYPE * pBuffer);
	virtual int		doNPReadVideo(VONP_BUFFERTYPE* pBuffer);
	
	static int		OnOSMPSendData(void * pUserData, int nSSType, VOOSMP_BUFFERTYPE* pBuffer);
	virtual int		doOSMPSendData(int nSSType, VOOSMP_BUFFERTYPE* pBuffer);
	
	static int		OnNPListener(void * pUserData, int nID, void * pParam1, void * pParam2);


	static void*		vomtGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag);
	static int			vomtFreeLib (void * pUserData, void * hLib, int nFlag);
	static void*		vomtLoadLib (void * pUserData, char * pLibName, int nFlag);
	
protected:
	int		ReturnCode_OSMP2NP(unsigned int nRC);
	int		ReturnCode_NP2OSMP(unsigned int nRC);
	int		BufferFlag_OSMP2NP(int nFlag);
	int     StreamType_OSMP2NP(int nType);
	int		Buffer_OSMP2NP(VOOSMP_BUFFERTYPE* pOSMP, VONP_BUFFERTYPE* pNP, int nSSType);
	int     LibOP_OSMP2NP(VOOSMP_LIB_FUNC* pOSMP, VONP_LIB_FUNC* pNP);
	int		Listener_OSMP2NP(VOOSMP_LISTENERINFO* pOSMP, VONP_LISTENERINFO* pNP);
	
	int		Load_library();
		
protected:
	COMXALEngine*			m_pEngn;
	VONP_LIB_FUNC		m_sLibFunc;
	
	VOOSMP_LISTENERINFO*	m_pOSMPLListener;
	
	VOOSMP_LISTENERINFO     m_NPListener;
	VONP_READBUFFER_FUNC m_NP_ReadBufFunc;
	VONP_SENDBUFFER_FUNC m_NP_SendBufFunc;
	

	
	VONP_BUFFER_FORMAT	m_sAudioTrackInfo;
	VONP_BUFFER_FORMAT	m_sVideoTrackInfo;
};

#endif // end _COMXAL_PLAYER_H_
