	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFileFormatCheck.cpp

	Contains:	CFileFormatCheck class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-11-22		JBF			Create file

*******************************************************************************/
#include "voString.h"
#include "voFormatChecker.h"
#include "../../File/ID3/ID3v2DataStruct.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#include "CFileFormatCheck.h"
#include "voBufferIO.h"
#include "voSource2.h"
#include "voDSType.h"
#include "voLog.h"

voFormatChecker::voFormatChecker()
:m_pBuffer(NULL)
,m_uBufferSize(0)
,m_uUsedSize(0)
,m_nFileFormat(VO_FILE_FFUNKNOWN)
{
	m_pBuffer = new VO_BYTE[ 1024 * 20 ];
	memset( m_pBuffer, 0x00, sizeof(VO_BYTE) * 1024 * 20);
	m_uBufferSize = 1024 *20;
}

voFormatChecker::~voFormatChecker()
{
	if( m_pBuffer )
		delete []m_pBuffer;
	m_pBuffer = NULL;
	m_uUsedSize = 0;
	m_uBufferSize = 0;
}
VO_VOID voFormatChecker::Reset()
{
	if(m_nFileFormat != VO_FILE_FFUNKNOWN)
	{
		memset(m_pBuffer, 0x00, sizeof(VO_BYTE) * m_uBufferSize);
		m_uUsedSize = 0;
		m_nFileFormat = VO_FILE_FFUNKNOWN;
	}
}
VO_U32 voFormatChecker::FormatCheckStart(VO_PBYTE *ppBuffer, VO_U32 *pSize, VO_FILE_FORMAT *pFileFormat )
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	Reset();

	VO_U32 uTmpSize = m_uUsedSize + *pSize;
	
	if( m_uBufferSize < uTmpSize)
	{
		VO_BYTE *pTmpBuff1 = new VO_BYTE[ uTmpSize + 100 ];
		if(!pTmpBuff1)
		{
			VOLOGW("new tmp buffer failed");
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
		memset( pTmpBuff1, 0x00, sizeof(VO_BYTE) * ( uTmpSize + 100) );
		if( m_uUsedSize > 0)
			memcpy( pTmpBuff1, m_pBuffer, sizeof(VO_BYTE) * m_uUsedSize);
		delete []m_pBuffer;
		m_pBuffer = pTmpBuff1;
		m_uBufferSize = uTmpSize + 100;
	}
	memcpy( m_pBuffer + m_uUsedSize, *ppBuffer, *pSize);
	m_uUsedSize += *pSize;


	VO_FILE_OPERATOR fop;
	memset( &fop , 0 , sizeof( VO_FILE_OPERATOR ) );
	voGetBufferFileOperatorAPI( &fop , 0 );

	VO_FILE_SOURCE source;
	source.nLength = m_uUsedSize;
	source.pSource = m_pBuffer;
	CFileFormatCheck ffcheck(&fop, NULL);
	m_nFileFormat = VO_FILE_FFUNKNOWN;
	m_nFileFormat = ffcheck.GetFileFormat(&source,FLAG_CHECK_ALL, ~FLAG_CHECK_AUDIOREADER );
	if( m_nFileFormat == VO_FILE_FFUNKNOWN )
	{
		m_nFileFormat = ffcheck.GetFileFormat(&source, FLAG_CHECK_ALL);
	}

	if( m_nFileFormat == VO_FILE_FFUNKNOWN )
	{
		if( m_uUsedSize < _VODS_SIZE_1M )
		{
			VOLOGW("Check FileFormat. VO_RET_SOURCE2_INPUTDATASMALL");
			ret =  VO_RET_SOURCE2_INPUTDATASMALL;
		}
		else 
		{
			VOLOGW("Check FileFormat. VO_RET_SOURCE2_FORMATUNSUPPORT");
			ret = VO_RET_SOURCE2_FORMATUNSUPPORT;
		}
	}
	else
	{
		*ppBuffer = m_pBuffer;
		*pSize = m_uUsedSize;
		*pFileFormat = m_nFileFormat;
		ret = VO_RET_SOURCE2_OK;
	}
	return ret;
}

VO_U32 voFormatChecker::GetInternalBuffer(VO_BYTE **ppBuffer, VO_U32 *pSize )
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	if( !m_pBuffer || m_uBufferSize == 0 || m_uUsedSize == 0)
		ret = VO_RET_SOURCE2_FAIL;
	*ppBuffer = m_pBuffer;
	*pSize = m_uUsedSize;
	return ret;
}
