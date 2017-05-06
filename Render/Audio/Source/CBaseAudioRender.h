	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseAudioRender.h

	Contains:	CBaseAudioRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CBaseAudioRender_H__
#define __CBaseAudioRender_H__

#include "voMem.h"
#include "voAudioRender.h"
#include "CvoBaseObject.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
    
class CBaseAudioRender : public CvoBaseObject
{
public:
	// Used to control the image drawing
	CBaseAudioRender (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CBaseAudioRender (void);

	virtual VO_U32		SetInputFormat(int nFormat);
	virtual VO_U32 		SetFormat (VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32 		Start (void);
	virtual VO_U32 		Pause (void);
	virtual VO_U32 		Stop (void);
	virtual VO_U32 		Render (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait);
	virtual VO_U32 		Flush (void);
	virtual VO_U32 		GetPlayingTime (VO_S64	* pPlayingTime);
	virtual VO_U32 		GetBufferTime (VO_S32	* pBufferTime);
	virtual VO_U32 		SetCallBack (VOAUDIOCALLBACKPROC pCallBack, VO_PTR pUserData);
	virtual VO_U32 		SetParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32 		GetParam (VO_U32 nID, VO_PTR pValue);

    virtual VO_U32      SetVolume(float leftVolume, float rightVolume);
    
    virtual VO_U32      AudioInterruption(VO_U32 inInterruptionState);
    
protected:
	VO_PTR				m_hInst;
	VO_MEM_OPERATOR *	m_pMemOP;

	VOAUDIOCALLBACKPROC	m_fCallBack;
	VO_PTR				m_pUserData;

	VO_LIB_OPERATOR *	m_pLibOP;
	VO_PTR				m_hCheck;

};
    
#ifdef _VONAMESPACE
}
#endif

#endif // __CBaseAudioRender_H__
