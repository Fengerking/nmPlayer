
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCMixDataSource.h

	Contains:	voCMixDataSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __voCMixDataSource_H__
#define __voCMixDataSource_H__

#include "voCDataSource.h"
#include "voParser.h"

class voCLivePlayer;

class voCMixDataSource : public voCDataSource
{
public:
	voCMixDataSource(VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB);
	~voCMixDataSource(void);

	virtual VO_U32			LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);
	virtual VO_U32			CloseSource (void);

	virtual VO_U32			AddBuffer (int nSSType, VOMP_BUFFERTYPE * pBuffer);

	virtual VO_U32			Start(void);
	virtual VO_U32			Stop(void);

	virtual void			SetLivePlayer (voCLivePlayer * pPlayer) {m_pLivePlayer = pPlayer;}

	virtual VO_U32			LoadLib (VO_HANDLE hInst);

protected:
	VO_PARSER_API			m_sParser;
	VO_PARSER_INIT_INFO		m_sInitInfo;
	VO_PTR					m_hParser;

	VO_U32					m_nProcRC;
	VO_PARSER_INPUT_BUFFER	m_sBuffer;

	VOMP_BUFFERTYPE			m_sVideoBuff;
	VOMP_BUFFERTYPE			m_sAudioBuff;

	bool					m_bStopped;

	voCLivePlayer *			m_pLivePlayer;

	VO_S64						m_nStartSendSysTime;
	VO_S64						m_nStartSendBufTime;

public:
	static	void			ParserCB (VO_PARSER_OUTPUT_BUFFER* pData);
	virtual void			ParseData (VO_PARSER_OUTPUT_BUFFER* pData);
};


#endif // __voCMixDataSource_H__
