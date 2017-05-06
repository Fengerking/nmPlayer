	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CSEIInfo.cpp

	Contains:	CSEIInfo class file


	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "CSEIInfo.h"

#define LOG_TAG "CSEIInfo"
#include "voLog.h"
#ifdef _IOS
#include "voLoadLibControl.h"
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CSEIInfo::CSEIInfo (int nCount)
	: m_nSEIType(0)
	, m_nSETCount(nCount)
	, m_pSEIData(NULL)
	, m_nSeiUserDataCount(0)
	, m_pSeiUserDataSet(NULL)
{
}

CSEIInfo::~CSEIInfo ()
{
	Uninit ();

	if(m_pSEIData)
	{
		delete m_pSEIData;
		m_pSEIData = NULL;
	}

	if (m_pSeiUserDataSet)
	{
		for (int i = 0; i < m_nSeiUserDataCount; ++i)
		{
			delete m_pSeiUserDataSet[i].m_pSeiUserData->buffer;
			delete m_pSeiUserDataSet[i].m_pSeiUserData;
		}
		delete m_pSeiUserDataSet;
	}
}

VO_U32 CSEIInfo::Init (unsigned char* pBuffer, int nLength, int nType)
{
	VO_U32 nRC = VO_ERR_NONE;
	
	m_nSEIType = nType;

	LoadLib(0);

	if (m_nSEIType & VO_SEI_PIC_TIMING )
	{
		m_pSEIData = new voH264SEIDataBuffer(m_nSETCount);
	}
	else if (m_nSEIType & VO_SEI_USER_DATA_UNREGISTERED )
	{
		if (m_pSeiUserDataSet)
			delete m_pSeiUserDataSet;

		m_pSeiUserDataSet = new SEI_USER_DATA_UNREGST[ m_nSETCount ];
		m_nSeiUserDataCount = 0;
	}

	return nRC;
}

VO_U32 CSEIInfo::Uninit (void)
{
	voCAutoLock lock( &m_Lock );
	if (m_nSEIType & VO_SEI_PIC_TIMING )
	{
		m_pSEIData->Flush();
	}
	else if (m_nSEIType & VO_SEI_USER_DATA_UNREGISTERED )
	{
		for (int i = 0; i < m_nSeiUserDataCount; ++i)
		{
			delete m_pSeiUserDataSet[i].m_pSeiUserData->buffer;
			delete m_pSeiUserDataSet[i].m_pSeiUserData;
		}
		delete m_pSeiUserDataSet;
		m_pSeiUserDataSet = NULL;
		m_nSeiUserDataCount = 0;
	}

	return 0;
}

VO_U32 CSEIInfo::AddSEIData (void* pBuffer)
{
	int nRC = VO_ERR_RETRY;
	if(m_nSEIType == 0)
		return nRC;

	voCAutoLock lock( &m_Lock );

	if (m_nSEIType & VO_SEI_PIC_TIMING )
	{
		nRC = m_pSEIData->AddBuffer( (VO_PARSER_SEI_INFO*)pBuffer);
	}
	else if (m_nSEIType & VO_SEI_USER_DATA_UNREGISTERED )
	{
		VO_U32 nDataSize = 0;
		VOOSMP_SEI_INFO* pParam = (VOOSMP_SEI_INFO*)pBuffer;
		VO_H264_USERDATA_Params* pTmpUserData = new VO_H264_USERDATA_Params;
		memcpy (pTmpUserData, pParam->pInfo, sizeof (VO_H264_USERDATA_Params));
		for (VO_U32 i = 0; i < pTmpUserData->count; ++i)
		{
			nDataSize = nDataSize + pTmpUserData->size[i];
		}
		VO_U8* pBuff = new VO_U8[nDataSize];
		memcpy (pBuff, pTmpUserData->buffer, nDataSize);
		pTmpUserData->buffer = pBuff;

		if (m_nSeiUserDataCount >= m_nSETCount ) 
		{
			delete m_pSeiUserDataSet[0].m_pSeiUserData->buffer;
			delete m_pSeiUserDataSet[0].m_pSeiUserData;
			for (int i = 0; i < m_nSeiUserDataCount-1; ++i)
			{
				m_pSeiUserDataSet[i] = m_pSeiUserDataSet[i+1];
			}
			m_pSeiUserDataSet[m_nSeiUserDataCount-1].m_nTime = (int)(pParam->llTime);
			m_pSeiUserDataSet[m_nSeiUserDataCount-1].m_pSeiUserData = pTmpUserData;
		}
		else
		{
			m_pSeiUserDataSet[m_nSeiUserDataCount].m_nTime = (int)(pParam->llTime);
			m_pSeiUserDataSet[m_nSeiUserDataCount].m_pSeiUserData = pTmpUserData;
			m_nSeiUserDataCount ++;
		}

		nRC = VO_ERR_NONE;
	}

	return nRC;
}

VO_U32 CSEIInfo::GetSEIData (void *pBuffer)
{
	int nRC = VO_ERR_RETRY;
	voCAutoLock lock( &m_Lock );

	if(m_nSEIType == 0)
		return nRC;

	if (m_nSEIType & VO_SEI_PIC_TIMING )
	{
		nRC = m_pSEIData->GetBuffer( (VO_PARSER_SEI_INFO*)pBuffer);
	}
	else if (m_nSEIType & VO_SEI_USER_DATA_UNREGISTERED )
	{
		if (m_nSeiUserDataCount <= 0)
		{
			return nRC;
		}
		else if (m_nSeiUserDataCount == 1)
		{
			VOOSMP_SEI_INFO* pParam = (VOOSMP_SEI_INFO*)pBuffer;
			//memcpy (pParam->pInfo, m_pSeiUserDataSet[0].m_pSeiUserData, sizeof(VO_H264_USERDATA_Params));
			pParam->pInfo = m_pSeiUserDataSet[0].m_pSeiUserData;
			pParam->llTime =  m_pSeiUserDataSet[0].m_nTime;
			nRC = VO_ERR_NONE;
		}
		else
		{
			VOOSMP_SEI_INFO* pParam = (VOOSMP_SEI_INFO*)pBuffer;
			int nTime = (int)(pParam->llTime);
			for (int i = m_nSeiUserDataCount; i > 1; --i)
			{
				int nTime1 = m_pSeiUserDataSet[i-1].m_nTime;
				int nTime2 = m_pSeiUserDataSet[i].m_nTime;

				if (nTime == 0)
				{
					pParam->pInfo = m_pSeiUserDataSet[0].m_pSeiUserData;
					pParam->llTime =  m_pSeiUserDataSet[i-1].m_nTime;
					nRC = VO_ERR_NONE;
					break;
				}

				if ( nTime < nTime1 )
				{
					continue;
				}
				else
				{
					int diff1 = nTime - nTime1;
					int diff2 = nTime2 - nTime;
					if (diff2 < 0) diff2 = 0 - diff2;

					if (diff1 < diff2)
					{
						//memcpy (pBuffer, m_pSeiUserDataSet[i-1].m_pSeiUserData, sizeof(VO_H264_USERDATA_Params));
						pParam->pInfo = m_pSeiUserDataSet[i-1].m_pSeiUserData;
						pParam->llTime =  m_pSeiUserDataSet[i-1].m_nTime;
					}
					else
					{
						//memcpy (pBuffer, m_pSeiUserDataSet[i].m_pSeiUserData, sizeof(VO_H264_USERDATA_Params));
						pParam->pInfo = m_pSeiUserDataSet[i].m_pSeiUserData;
						pParam->llTime =  m_pSeiUserDataSet[i].m_nTime;
					}
					nRC = VO_ERR_NONE;
					break;
				}
			}
		}
	}

	return nRC;
}

VO_U32 CSEIInfo::SetPos (int nCurPos)
{
	int nRC = VO_ERR_RETRY;
	voCAutoLock lock( &m_Lock );

	if (m_nSEIType & VO_SEI_PIC_TIMING )
	{
		nRC = m_pSEIData->RemoveTo(nCurPos);
	}
	else
	{
		/* Flus buffer and recreat */
		nRC = Flush();
	}

	return nRC;
}

void* CSEIInfo::GetSEISample (void *pSample, int nSEIType)
{
	if(pSample == 0)
		return NULL;

	voCAutoLock lock( &m_Lock );

	void *pRet = NULL;
	VO_PARSER_SEI_INFO* pTmpSample = (VO_PARSER_SEI_INFO*)pSample;
	if (m_nSEIType & VO_SEI_PIC_TIMING )
	{
		if(pTmpSample->buffercount)
		{
			int i = 0; 
			unsigned char *buffer = pTmpSample->buffer;
			
			for(i=0; i< (int)pTmpSample->buffercount; i++)
			{
				if(pTmpSample->data[i].flag&nSEIType)
				{
					return (buffer + pTmpSample->data[i].buffer_lenth);
				}
				else
				{
					buffer += pTmpSample->data[i].buffer_lenth + pTmpSample->data[i].has_struct;
				}
			}
		}
	}

	return pRet;
}

VO_U32 CSEIInfo::Flush (void)
{
	int nRC = VO_ERR_RETRY;
	voCAutoLock lock( &m_Lock );

	if (m_nSEIType & VO_SEI_PIC_TIMING )
	{
		nRC = m_pSEIData->Flush();
	}
	else if (m_nSEIType & VO_SEI_USER_DATA_UNREGISTERED )
	{
		for (int i = 0; i < m_nSeiUserDataCount; ++i)
		{
			delete m_pSeiUserDataSet[i].m_pSeiUserData->buffer;
			delete m_pSeiUserDataSet[i].m_pSeiUserData;
		}
		m_nSeiUserDataCount = 0;
		nRC = VO_ERR_NONE;
	}

	return nRC;
}

VO_U32 CSEIInfo::Enable (int nEnable)
{
	if(nEnable == 0)
		Flush();

	m_nSEIType = nEnable;

	Init(0, 0, nEnable);

	return 0;
}

VO_U32 CSEIInfo::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	int nRC = VO_ERR_RETRY;
	voCAutoLock lock( &m_Lock );


	return 	nRC;
}

VO_U32 CSEIInfo::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	int nRC = VO_ERR_RETRY;

	return nRC;	
}

VO_U32 CSEIInfo::LoadLib (VO_HANDLE hInst)
{
	return 1;
}
