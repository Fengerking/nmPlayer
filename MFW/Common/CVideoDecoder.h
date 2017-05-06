	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoDecoder.h

	Contains:	CVideoDecoder header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CVideoDecoder_H__
#define __CVideoDecoder_H__

#include "voVideo.h"
#include "CBaseNode.h"
#include "viMem.h"
#include "voCheck.h"
#include "vompType.h"

class CVideoDecoder : public CBaseNode
{
public:
	CVideoDecoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP);
	virtual ~CVideoDecoder (void);

	virtual void		SetFourCC (VO_U32 nFourCC) {m_nFourCC = nFourCC;}
	virtual VO_U32		SetVideoLibFile (VO_U32 nVideoType, VO_PTCHAR pFileName);
	virtual VO_U32		SetVideoAPIName (VO_U32 nVideoType, VO_PTCHAR pAPIName);

	virtual VO_U32		Init (VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_VIDEO_FORMAT * pFormat);
	virtual VO_U32		Uninit (void);

	virtual VO_U32		Start(void);
	virtual VO_U32		Pause(void);
	virtual VO_U32		Stop(void);

	virtual void		SetDelayTime (VO_S32 nDelayTime) {m_nDelayTime = nDelayTime;}
	virtual VO_U32		SetInputData (VO_CODECBUFFER * pInput);
	virtual VO_U32		GetOutputData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat, VO_BOOL * pbContinuous);
	virtual VO_U32		Flush (void);

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);

	virtual VO_U32		GetFormat (VO_VIDEO_FORMAT * pFormat);

	virtual VO_VIDEO_FRAMETYPE GetFrameType (VO_CODECBUFFER * pBuffer);
	virtual VO_VIDEO_FRAMETYPE GetFrameType (VO_PBYTE pBuffer, VO_U32 nBuffSize);
	bool				IsRefFrame(VO_CODECBUFFER * pBuffer); //If B frame is reference frame

	virtual int			GetDeblock( ) {return (m_nDisableDeblock ? 0 : 1);};

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

	void				SetSourceType(VO_U32 nST){mnSourceType = nST;}
	void				SetSourcePtr(VO_PTR psrc) { m_psrc = psrc; } // added by David @ 2011/11/30
	void				SetListenerInfo(VO_PTR pListenerInfo);  // david @ 2011/12/2
	void				SetVideoCount(VO_U32 nVideoCount) {m_nVideoCount = nVideoCount;};  // jacky @ 2011/12/2
	int					GetMemShared() {return m_nMemShared;};  // jacky @ 2011/12/2

	void				ResetDecoder(VO_PBYTE pHeadData = NULL, VO_U32 nHeadSize = 0);
	virtual VO_BOOL		IsIOMXWorking();
	// added by gtxia
	bool                setSampleDrmInfo(const bool isDrmAppended);
protected:
	virtual VO_U32		UpdateParam (void);
	virtual VO_U32		SetHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize);
	virtual VO_U32		HandleFirstSample (VO_CODECBUFFER * pInput);

	VO_VIDEO_FRAMETYPE	GetH264FrameType(unsigned char * buffer , int size);
	

protected:
	VO_VIDEO_DECAPI				m_funDec;
	VO_HANDLE					m_hDec;

	VO_U32						m_nCoding;
	VO_U32						m_nFourCC;
	VO_BOOL						m_bSetHeadData;
	VO_BOOL						m_bFirstSample;
	VO_U32						m_nVideoWidth;
	VO_U32						m_nVideoHeight;

	VO_S64						m_llPrevTime;
	VO_U32						m_nFrameRate;

	VO_S32						m_nDelayTime;
	VO_CODECBUFFER *			m_pInputBuffer;
	VO_U32						m_nInputUsed;

	VO_U32						m_nVideoCount;
	VO_U32						m_nMemShared;

	VO_VIDEO_OUTPUTINFO			m_OutputInfo;
	VO_S32						m_nDecFrames;
	VO_S32						m_nVideoArtifact;

	VO_S32						m_nPassCodec;
	VO_PBYTE					m_pYUVPassData;
	VO_U32						m_nPassCount;

	VO_MEM_VIDEO_OPERATOR *		m_pVMOP;
	VO_MEM_OPERATOR				m_memVideo;

	VO_PTR						m_hCheck;

	VO_CODECBUFFER				mHeadData;
	VO_U32						mnSourceType;  // 0, Live, 1 File	

	VO_U32						m_nDisableDeblock;
	VO_TCHAR					m_sWorkPath[1024];

	// added by david @ 1011/11/30 for OMXCompWrap@h264
	VO_PTR						m_psrc; 
	VOMP_LISTENERINFO			m_listenerinfo;

	VO_U32						m_nSetVideoNameType;
	VO_U32						m_nSetVideoAPIType;
	VO_TCHAR					m_sSetVideoName[256];	
	VO_TCHAR					m_sSetVideoAPI[256];
	bool                        mIsDrmDataAppended;
protected:
	static VO_U32	voVideoMemAlloc (VO_S32 uID,  VO_MEM_INFO * pMemInfo);
	static VO_U32	voVideoMemFree (VO_S32 uID, VO_PTR pBuffer);
	static VO_U32	voVideoMemSet (VO_S32 uID, VO_PTR pBuff, VO_U8 uValue, VO_U32 uSize);
	static VO_U32	voVideoMemCopy (VO_S32 uID, VO_PTR pDest, VO_PTR pSource, VO_U32 uSize);
	static VO_U32	voVideoMemCheck (VO_S32 uID, VO_PTR pBuffer, VO_U32 uSize);
	static VO_S32	voVideoMemCompare (VO_S32 uID, VO_PTR pBuffer1, VO_PTR pBuffer2, VO_U32 uSize);
	static VO_U32	voVideoMemMove (VO_S32 uID, VO_PTR pDest, VO_PTR pSource, VO_U32 uSize);
};

#endif // __CVideoDecoder_H__
