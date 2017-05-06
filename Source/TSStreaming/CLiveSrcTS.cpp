	/************************************************************************
	 *																		*
	 *		VisualOn, Inc. Confidential and Proprietary, 2003 ~				*
	 *																		*
	 ************************************************************************/
/*******************************************************************************
 File:		CLiveSrcTS.h
 
 Contains:	CLiveSrcTS header file
 
 Written by:	Jeff Huang
 
 Change History (most recent first):
 2011-07-22		hjf			Create file
 
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "CLiveSrcTS.h"
#include "CLiveParserTs.h"
#include "voLog.h"
#include "voLiveSource.h"
#include "voTSParser.h"


CLiveSrcTS::CLiveSrcTS(void)
: CLiveSrcBase ()
,m_nDiscontinuousCount(0)
,m_nLossPackageCount(0)
{
}

CLiveSrcTS::~CLiveSrcTS(void)
{
}

VO_U32 CLiveSrcTS::doInitParser()
{	
	m_pParser = new CLiveParserBase(VO_LIVESRC_FT_TS);
	
	return VO_ERR_LIVESRC_OK;
}

VO_U32 CLiveSrcTS::doOpenDevice()
{
    VO_PARSER_INIT_INFO info;
	memset(&info, 0, sizeof(VO_PARSER_INIT_INFO));
	info.pProc		= OnParsedDataProc;
	info.pUserData	= this;
	info.pMemOP		= NULL;
	
	return m_pParser->Open(&info);
}

VO_U32 CLiveSrcTS::Stop()
{
	int nRet = CLiveSrcBase::Stop();
	m_nDiscontinuousCount	= 0;
	m_nLossPackageCount		= 0;
	
	return nRet;
}

VO_BOOL CLiveSrcTS::doOnParsedData(VO_PARSER_OUTPUT_BUFFER * pData)
{
	VO_BOOL bRet = VO_FALSE;
	
	switch (pData->nType )
	{
		case VO_PARSER_OT_TS_PACKET_LOSS:
		{
			VO_TS_PACKET_LOST_INFO	* ptr_info = ( VO_TS_PACKET_LOST_INFO * )pData->pOutputData;
			
			if( ptr_info->nErrorCode == MTV_PARSER_ERROR_CODE_NO_CONTINUITY) 
			{
				m_nDiscontinuousCount++;
			}
			else if( ptr_info->nErrorCode == MTV_PARSER_ERROR_CODE_PACKET_SIZE_ERROR) 
			{
				m_nLossPackageCount++;
			}
			bRet = VO_TRUE;
		}
			break;
		default:
			bRet = VO_FALSE;
	}
	
	return bRet;
}

VO_U32 CLiveSrcTS::SetParam(VO_U32 nParamID, VO_PTR pValue)
{
	VOLOGI("CLiveSrcTS::SetParam(0x%0.8X, 0x%0.8X)", nParamID, pValue);
	if (nParamID == VO_PID_LIVESRC_LIBOP)
	{
		VOLOGI("VO_PID_LIVESRC_LIBOP is coming");
		if (!m_pParser)
		{
			VOLOGI("m_pParser is NULL");
			return 0;
		}
		else
		{
			VOLOGI("Value is 0x%0.8X", pValue);
			m_pParser->SetLibOperator((VO_LIB_OPERATOR *)pValue);
		}
	}
	return 0;
}
