	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXFileSource.h

	Contains:	voCOMXFileSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXFileSource_H__
#define __voCOMXFileSource_H__

#include "vomeFilePipe.h"
#include "voCOMXCompSource.h"
#include "voCOMXFileAudioPort.h"
#include "voCOMXFileVideoPort.h"
#include "voCOMXPortClock.h"
#include "CFileSource.h"

#define	TASK_LOAD_FILE	0X01000001

class voCOMXFileSource : public voCOMXCompSource
{
public:
	voCOMXFileSource(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXFileSource(void);

public:
	virtual OMX_ERRORTYPE GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nParamIndex,
										OMX_INOUT OMX_PTR pComponentParameterStructure);


	virtual OMX_ERRORTYPE SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_INOUT OMX_PTR pComponentConfigStructure);


	virtual OMX_ERRORTYPE SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE SendCommand (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_COMMANDTYPE Cmd,
										OMX_IN  OMX_U32 nParam1,
										OMX_IN  OMX_PTR pCmdData);

	virtual OMX_ERRORTYPE FillThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

	static int				voomxFileSourceStreamCallBack (long EventCode, long * EventParam1, long * userData);

protected:
	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);

	virtual OMX_ERRORTYPE	LoadFile (OMX_PTR pSource, OMX_U32 nType);
	virtual OMX_ERRORTYPE	CloseFile (void);
	virtual OMX_ERRORTYPE	ForceCloseSource (void);

	//make GetSample can return quickly
	//because PD will use much time, response will be very slowly
	virtual OMX_ERRORTYPE	QuickReturnBuffer();

	virtual OMX_BOOL		GetLineFromHeaders(OMX_STRING pKey, OMX_STRING pLine);
	virtual OMX_BOOL		GetValueFromHeaders(OMX_STRING pKey, OMX_STRING pValue);
	virtual OMX_BOOL		GetRawHeaders();

	virtual OMX_BOOL		CheckProxyIP(OMX_STRING strIP);
	virtual OMX_BOOL		CheckProxyPort(OMX_STRING strPort);

	virtual OMX_U32			DumpPerformanceInfo (OMX_STRING pFile);
	virtual OMX_U32			MessageHandle (COMP_MESSAGE_TYPE * pTask);

protected:
	voCOMXFileAudioPort *		m_pAudioPort;
	voCOMXFileVideoPort *		m_pVideoPort;
	voCOMXPortClock *			m_pClockPort;
	voCOMXThreadMutex			m_tmLoadSource;
	voCOMXThreadMutex			m_tmForceClose;

	OMX_U32						m_nURISize;
	OMX_S8 *					m_pURI;
	OMX_VO_FILE_OPERATOR *		m_pPipe;
	OMX_VO_SOURCEDRM_CALLBACK *	m_pDrm;
	OMX_S64						m_nOffset;
	OMX_S64						m_nLength;
	OMX_BOOL					m_bForceClose;

	OMX_STRING					m_pHeaders;
	OMX_STRING					m_pRawHeaders;
	OMX_STRING					m_pTmpHeaders;

	OMX_TIME_SEEKMODETYPE		m_nSeekMode;
	OMX_S64						m_llStartPos;

	CBaseSource *				m_pSource;
	OMX_S32						m_nAudioTrack;
	OMX_S32						m_nVideoTrack;

	OMX_U32						m_nWVC1Dec;

	VO_MEM_OPERATOR *			m_pMemOP;
	VO_FILE_OPERATOR *			m_pFileOP;
	VO_FILE_OPERATOR			m_FileOP;

	CBaseConfig *				m_pCfgSource;
	CBaseConfig *				m_pCfgCodec;

	OMX_BOOL					m_bBufferThread;

	VO_FILE_FORMAT				m_ffSource;
	VO_FILE_FORMAT				m_ffExternalSet;	//set externally

	VO_LIVESRC_FORMATTYPE		m_LiveSrcType;

	OMX_TICKS					m_llTmpRefTime;
	OMX_BOOL					m_bBlockUnknownCodec;
};

#endif //__voCOMXFileSource_H__
