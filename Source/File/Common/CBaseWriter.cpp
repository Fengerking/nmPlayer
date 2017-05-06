/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CBaseWriter.cpp

Contains:	CBaseWriter class file

Written by:	Lin Jun

Change History (most recent first):
2010-05-19		Lin Jun			Create file

*******************************************************************************/
#include "CBaseWriter.h"
#include "CBaseDumper.h"
#include "CBaseAssembler.h"
#include "cmnMemory.h"
#include "fMacros.h"
#include "CDumper.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CBaseWriter::CBaseWriter(void)
:m_bIsFirstVideo(VO_FALSE)
,m_bIsFirstAudio(VO_FALSE)
,m_nTimeOffset(0)
,m_pDumper(VO_NULL)
,m_pAssembler(VO_NULL)
,m_pAssemblerInitInfo(VO_NULL)
{
}

CBaseWriter::~CBaseWriter(void)
{
	Close();
}

VO_U32	CBaseWriter::Open(VO_FILE_SOURCE* pFileSource , VO_SINK_OPENPARAM * pParam)
{
	if(pParam == NULL || pFileSource == NULL)
		return VO_ERR_FAILED;

	m_pDumper = doCreateDumper((VO_FILE_OPERATOR *)pParam->pSinkOP);
	
	VO_U32 nRet = m_pDumper->Open(pFileSource);

	if(nRet != VO_ERR_NONE)
		return nRet;

	m_pAssemblerInitInfo = new ASSEMBLERINITINFO;
	cmnMemSet(0, m_pAssemblerInitInfo, 0, sizeof(ASSEMBLERINITINFO));
	m_pAssemblerInitInfo->codec_count	= 0;
	m_pAssemblerInitInfo->dumper		= m_pDumper;

	if (pParam->nAudioCoding != 0)
	{
		m_pAssemblerInitInfo->codec_list[0] = pParam->nAudioCoding;
		m_pAssemblerInitInfo->codec_count++;
	}


	if (pParam->nVideoCoding != 0)
	{
		m_pAssemblerInitInfo->codec_list[1] = pParam->nVideoCoding;
		m_pAssemblerInitInfo->codec_count++;
	}

	m_pAssembler = doCreateAssembler();
	m_pAssembler->Open(m_pAssemblerInitInfo);

	return VO_ERR_NONE;
}

CBaseDumper* CBaseWriter::doCreateDumper(VO_FILE_OPERATOR* op)
{
	return new CBaseDumper(op);
}

VO_U32	CBaseWriter::Close()
{
	m_bIsFirstVideo	= VO_FALSE;
	m_bIsFirstAudio	= VO_FALSE;
	m_nTimeOffset	= 0;

	if (m_pDumper)
	{
		m_pDumper->Close();

		delete m_pDumper;
		m_pDumper = VO_NULL;
	}

	if (m_pAssembler)
	{
		m_pAssembler->Close();
		delete m_pAssembler;
		m_pAssembler = VO_NULL;
	}

	if (m_pAssemblerInitInfo)
	{
		delete m_pAssemblerInitInfo;
		m_pAssemblerInitInfo = VO_NULL;
	}

	return VO_ERR_NONE;
}

VO_U32	CBaseWriter::AddSample( VO_SINK_SAMPLE * pSample)
{
	voCAutoLock locker(&m_Mutex);

	// pre-process
	if(!m_bIsFirstAudio && !m_bIsFirstVideo)
	{
		m_nTimeOffset = pSample->Time;

		m_bIsFirstAudio = VO_TRUE;
	}

	pSample->Time -= m_nTimeOffset;

	if(pSample->Time < 0)
		pSample->Time = 0;

	VO_BOOL  bIsSync = VO_FALSE;

	if(pSample->Size & 0x80000000)
	{
		bIsSync = VO_TRUE;
	}

	pSample->Size = pSample->Size & 0x7FFFFFFF;


	if (m_pAssembler)
	{
		m_pAssembler->AddSample(bIsSync, pSample);
	}

	return VO_ERR_NONE;
}

VO_U32	CBaseWriter::SetParam(VO_U32 uID, VO_PTR pParam)
{
	if (VO_PID_SINK_EXTDATA == uID)
	{
		VO_SINK_EXTENSION* pExtData = (VO_SINK_EXTENSION*)pParam;
		if (m_pAssembler)
		{
			m_pAssembler->AddExtData(pExtData->nAV, pExtData);
		}
		return VO_ERR_NONE;
	}
	else if (VO_PID_SINK_FLUSH == uID)
	{
		return VO_ERR_NONE;
	}
	else if (VO_PID_COMMON_LOGFUNC == uID)
	{
	//	VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pParam;
	//	vologInit (pVologCB->pUserData, pVologCB->fCallBack);
	}

	return VO_ERR_FAILED;
}

VO_U32	CBaseWriter::GetParam(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_FAILED;
}



