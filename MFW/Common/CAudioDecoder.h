	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAudioDecoder.h

	Contains:	CAudioDecoder header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CAudioDecoder_H__
#define __CAudioDecoder_H__

#include "voAudio.h"
#include "CBaseNode.h"

class CAudioDecoder : public CBaseNode
{
public:
	CAudioDecoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP);
	virtual ~CAudioDecoder (void);

	virtual void		SetFourCC (VO_U32 nFourCC) {m_nFourCC = nFourCC;}
	virtual void		SetAACPlus (VO_U32 bAACPlus) { m_bDisableAACP = (bAACPlus == 0 ? VO_FALSE : VO_TRUE);}

	virtual VO_U32		SetAudioLibFile (VO_U32 nAudioType, VO_PTCHAR pFileName);
	virtual VO_U32		SetAudioAPIName (VO_U32 nAudioType, VO_PTCHAR pAPIName);

	virtual VO_U32		Init (VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32		Uninit (void);

	virtual VO_U32		Start(void);
	virtual VO_U32		Pause(void);
	virtual VO_U32		Stop(void);

	virtual VO_U32		SetInputData (VO_CODECBUFFER * pInput);
	virtual VO_U32		GetOutputData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat);
	virtual VO_U32		Flush (void);

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);

	virtual VO_U32		GetFormat (VO_AUDIO_FORMAT * pFormat);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);
	bool                setSampleDrmInfo(const bool isDrmAppended);
	inline VO_AUDIO_CODINGTYPE getOutputType() {return mOutputType;}
protected:
	virtual VO_U32		UpdateParam (void);
	virtual VO_U32		SetHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize);
	virtual VO_U32		HandleFirstSample (VO_CODECBUFFER * pInput);

	virtual VO_U32		DownMixBuffer (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat);

protected:
	VO_AUDIO_CODECAPI	m_funDec;
	VO_HANDLE			m_hDec;

	VO_U32				m_nCoding;
	VO_U32				m_nFourCC;
	VO_BOOL				m_bFirstSample;

	VO_CODECBUFFER *	m_pInputData;
	VO_U32				m_nInputUsed;

	VO_BOOL				m_bSetHeadData;

	VO_AUDIO_FORMAT		m_fmtAudio;
	VO_AUDIO_OUTPUTINFO	m_OutputInfo;

	VO_BOOL				m_bDisableAACP;

	FILE *				m_hDumpFile;

	VO_U32				m_nSetAudioNameType;
	VO_U32				m_nSetAudioAPIType;
	VO_TCHAR			m_sSetAudioName[256];
	VO_TCHAR			m_sSetAudioAPI[256];
	bool                mIsDrmDataAppended;
	VO_AUDIO_CODINGTYPE mOutputType;

	VO_TCHAR			m_sWorkPath[1024];
	bool				m_bSpptMltChanl;  //if audio render support multi-channel
};

#endif // __CAudioDecoder_H__
