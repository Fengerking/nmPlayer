/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

/*******************************************************************************
File:		CAviParser.cpp

Contains:	provides the interfaces to parse an avi file according to the avi specification 

Written by:	East

Reference:	OpenDML AVI File Format Extensions

Change History (most recent first):
2006-09-20		East			Create file

*******************************************************************************/
#include "CAviParser.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

#define avi_obj_data_len(obj)	((obj.size + 1) & (~1))
#define avi_obj_total_len(obj)	(sizeof(AviObject) + avi_obj_data_len(obj))

#define read_avi_object(obj)	ptr_read_pointer(&obj, sizeof(AviObject))
#define skip_dword()			ptr_skip(sizeof(VO_U32))
#define skip_chunk()			ptr_skip(avi_obj_data_len(ao))

const bool AVI_FOURCC_CHAR[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //0-31
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, //32-63
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, //64-95
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, //96-127
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //128-159
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //160-191
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //192-223
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	//224-255
};

CAviHeaderParser::CAviHeaderParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
	: CvoFileHeaderParser(pFileChunk, pMemOp)
	, m_pStrdContent(VO_NULL)
	, m_dwStrdLength(0)
	, m_ullMediaDataFilePos(0)
	, m_ullMediaDataSize(0)
	, m_btStreamCount(0)
	, m_paTrackInfo(VO_NULL)
	, m_btIndexFlags(0)
	, m_paProposedIndexInfo(VO_NULL)
	, m_ullIdx1FilePos(0)
	, m_dwIdx1Size(0)
{
}

CAviHeaderParser::~CAviHeaderParser()
{
	ReleaseTmpInfo();

	if(m_paProposedIndexInfo)
	{
		PAviProposedIndexInfo pCur = m_paProposedIndexInfo;
		PAviProposedIndexInfo pDel;
		while(pCur)
		{
			pDel = pCur;
			pCur = pCur->next;
			MemFree(pDel->pullFilePos);
			MemFree(pDel);
		}
		m_paProposedIndexInfo = VO_NULL;
	}

	SAFE_MEM_FREE(m_pStrdContent);
}

VO_BOOL CAviHeaderParser::ReadFromFile()
{
	m_ullMediaDataFilePos = 0;
	m_ullMediaDataSize = 0;

	AviObject ao;
	read_avi_object(ao);

	if(FOURCC_RIFF != ao.fcc)
		return VO_FALSE;

	skip_dword();

	if(!ReadHdrl())
		return VO_FALSE;

	VO_U32 nRes = 1;
	while(1 == nRes)
		nRes = ReadDataB();

	return VO_TRUE;
}

VO_BOOL CAviHeaderParser::ReadHdrl()
{
	VO_U32 dwValue = 0, dwLeave = 0;

	ptr_read_dword(dwValue);

	//some file will has padding data before 'hdrl'!!
	//we should skip them!!
	while(FOURCC_JUNK == dwValue || FOURCC_JUNQ == dwValue)
	{
		ptr_read_dword(dwLeave);
		ptr_skip((dwLeave + 1) & (~1));

		ptr_read_dword(dwValue);
	}

	if(FOURCC_LIST != dwValue)
		return VO_FALSE;

	ptr_read_dword(dwLeave);
	ptr_read_dword(dwValue);
	if(FOURCC_hdrl != dwValue)
	{
		ptr_skip(dwLeave - 4);
		
		ptr_read_dword(dwValue);

		ptr_read_dword(dwLeave);
		ptr_read_dword(dwValue);
	}
	
	if(FOURCC_hdrl != dwValue)
		return VO_FALSE;

	VO_S32 nLeave = dwLeave - 4;
	VO_U32 dwReaded = 0;
	while(nLeave > 0)
	{
		dwReaded = ReadHdrlB();
		if(!dwReaded)
			return VO_FALSE;

		nLeave -= dwReaded;
	}

	return (nLeave == 0 || nLeave == -1) ? VO_TRUE : VO_FALSE;
}

VO_U32 CAviHeaderParser::ReadHdrlB()
{
	AviObject ao;
	read_avi_object(ao);
	switch(ao.fcc)
	{
	case FOURCC_avih:
		skip_chunk();
		break;

	case FOURCC_LIST:
		{
			VO_U32 dwValue = 0;
			ptr_read_dword(dwValue);
			switch(dwValue)
			{
			case FOURCC_strl:
				{
					VO_S32 nLeave = ao.size - 4;
					VO_U32 dwReaded = 0;
					while(nLeave > 0)
					{
						dwReaded = ReadStrlB();
						if(!dwReaded)
							return 0;

						nLeave -= dwReaded;
					}
				}
				break;

			case FOURCC_INFO:
				{
					VO_S32 nLeave = ao.size - 4;
					VO_U32 dwReaded = 0;
					while(nLeave > 0)
					{
						dwReaded = ReadInfoB();
						if(!dwReaded)
							return 0;

						nLeave -= dwReaded;
					}
				}
				break;

			case FOURCC_odml:
				{
					VO_S32 nLeave = ao.size - 4;
					VO_U32 dwReaded = 0;
					while(nLeave > 0) 
					{
						dwReaded = ReadOdmlB();
						if(!dwReaded)
							return 0;

						nLeave -= dwReaded;
					}
				}
				break;

			default:
				break;
			}
		}
		break;

	case FOURCC_IDIT:
	case FOURCC_JUNK:
	default:
		skip_chunk();
		break;
	}

	return avi_obj_total_len(ao);
}

VO_U32 CAviHeaderParser::ReadStrlB()
{
	AviObject ao;
	read_avi_object(ao);

	VO_U32 dwRet = avi_obj_total_len(ao);
	switch(ao.fcc)
	{
	case FOURCC_strh:
		{
			PAviTrackInfo pNewInfo = NEW_OBJ(AviTrackInfo);
			pNewInfo->btStreamNum = m_btStreamCount;
			pNewInfo->next = VO_NULL;

			ADD_TO_CHAIN(AviTrackInfo, m_paTrackInfo, pNewInfo);

			ptr_read_pointer(&pNewInfo->StreamHeader, sizeof(AviStreamHeader));
			ptr_skip(ao.size - sizeof(AviStreamHeader));

			read_avi_object(ao);
			if(FOURCC_strf != ao.fcc)
				return 0;

			pNewInfo->dwPropBufferSize = ao.size;
			pNewInfo->pPropBuffer = NEW_BUFFER(pNewInfo->dwPropBufferSize);

			ptr_read_pointer(pNewInfo->pPropBuffer, ao.size);
			if (0x73647561/*auds*/ == pNewInfo->StreamHeader.fccStreamType && ao.size >= VO_WAVEFORMATEX_STRUCTLEN) //#14701
				( (VO_WAVEFORMATEX*)pNewInfo->pPropBuffer )->cbSize = ao.size - VO_WAVEFORMATEX_STRUCTLEN;

			ptr_skip(ao.size & 1);

			dwRet += avi_obj_total_len(ao);
			m_btStreamCount++;
		}
		break;

	case FOURCC_indx:
		{
			m_btIndexFlags |= FLAG_INDX;

			PAviProposedIndexInfo pNewInfo = NEW_OBJ(AviProposedIndexInfo);
			pNewInfo->next = VO_NULL;
			pNewInfo->pullFilePos = VO_NULL;

			ADD_TO_CHAIN(AviProposedIndexInfo, m_paProposedIndexInfo, pNewInfo);

			//skip wLongsPerEntry, bIndexSubType, bIndexType!!
			ptr_skip(4);
			ptr_read_dword(pNewInfo->dwEntriesInUse);
			ptr_read_dword(pNewInfo->dwChunkId);
			//skip dwReserved[3]!!
			ptr_skip(12);

			pNewInfo->pullFilePos = NEW_OBJS(VO_U64, pNewInfo->dwEntriesInUse);

			//super index entries!! some AviSuperIndexEntry
			for(VO_U32 j = 0; j < pNewInfo->dwEntriesInUse; j++)
			{
				ptr_read_qword(pNewInfo->pullFilePos[j]);
				//skip size and duration!!
				ptr_skip(8);
			}

			ptr_skip(ao.size - sizeof(AviSuperIndexChunk) - pNewInfo->dwEntriesInUse * sizeof(AviSuperIndexEntry));
		}
		break;

	case FOURCC_strd:
		{
			SAFE_MEM_FREE(m_pStrdContent);

			m_dwStrdLength = ao.size;
			m_pStrdContent = NEW_BUFFER(m_dwStrdLength);
			if(!m_pStrdContent)
				return VO_FALSE;

			ptr_read_pointer(m_pStrdContent, m_dwStrdLength);

			if(ao.size & 0x1)
				ptr_skip(1);
		}
		break;

	case FOURCC_strn:
	case FOURCC_JUNK:
	case FOURCC_vprp:
	default:
		skip_chunk();
		break;
	}

	return dwRet;
}

VO_U32 CAviHeaderParser::ReadInfoB()
{
	AviObject ao;
	read_avi_object(ao);

	switch(ao.fcc)
	{
	case FOURCC_Info_IARL:
	case FOURCC_Info_IART:
	case FOURCC_Info_ICMS:
	case FOURCC_Info_ICMT:
	case FOURCC_Info_ICOP:
	case FOURCC_Info_ICRD:
	case FOURCC_Info_ICRP:
	case FOURCC_Info_IDIM:
	case FOURCC_Info_IDPI:
	case FOURCC_Info_IENG:
	case FOURCC_Info_IGNR:
	case FOURCC_Info_IKEY:
	case FOURCC_Info_ILGT:
	case FOURCC_Info_IMED:
	case FOURCC_Info_INAM:
	case FOURCC_Info_IPLT:
	case FOURCC_Info_IPRD:
	case FOURCC_Info_ISBJ:
	case FOURCC_Info_ISFT:
	case FOURCC_Info_ISHP:
	case FOURCC_Info_ISRC:
	case FOURCC_Info_ISRF:
	case FOURCC_Info_ITCH:
	case FOURCC_JUNK:
	default:
		skip_chunk();
		break;
	}

	return avi_obj_total_len(ao);
}

VO_U32 CAviHeaderParser::ReadOdmlB()
{
	AviObject ao;
	read_avi_object(ao);

	switch(ao.fcc)
	{
	case FOURCC_dmlh:
	case FOURCC_JUNK:
	default:
		skip_chunk();
		break;
	}

	return avi_obj_total_len(ao);
}

VO_U32 CAviHeaderParser::ReadDataB()
{
	AviObject ao;
	read_avi_object(ao);

	switch(ao.fcc)
	{
	case FOURCC_LIST:
		{
			VO_U32 fcc;
			ptr_read_dword(fcc);
			if(fcc == FOURCC_movi)
			{
				if(!m_ullMediaDataFilePos)
					m_ullMediaDataFilePos = m_pFileChunk->FGetFilePos() + sizeof(VO_U32);

				m_ullMediaDataSize += (avi_obj_data_len(ao) - sizeof(VO_U32));
			}
			ptr_skip(((ao.size + 1) & (~1)) - sizeof(VO_U32));
		}
		break;

	case FOURCC_idx1:
		{
			VO_U64 ullCurFilePos = m_pFileChunk->FGetFilePos();
			//skip_chunk();

			m_btIndexFlags |= FLAG_IDX1;

			m_ullIdx1FilePos = ullCurFilePos;
			m_dwIdx1Size = ao.size;

			if (m_ullMediaDataFilePos && m_ullMediaDataSize)
				return 0;
		}
		break;

	case FOURCC_RIFF:
		skip_dword();
		break;

	case FOURCC_JUNK:
	case FOURCC_JUNQ:
	default:
		skip_chunk();
		break;
	}
	return ao.size ? 1 : 0;
}

VO_VOID CAviHeaderParser::ReleaseTmpInfo()
{
	if(m_paTrackInfo)
	{
		PAviTrackInfo pCur = m_paTrackInfo;
		PAviTrackInfo pDel;
		while(pCur)
		{
			pDel = pCur;
			pCur = pCur->next;
			MemFree(pDel->pPropBuffer);
			MemFree(pDel);
		}
		m_paTrackInfo = VO_NULL;
	}
}

PAviProposedIndexInfo CAviHeaderParser::GetProposedIndexInfo(VO_U16 wStreamTwocc)
{
	PAviProposedIndexInfo pTmp = m_paProposedIndexInfo;
	while(pTmp)
	{
		if((pTmp->dwChunkId & 0xFFFF) == wStreamTwocc)
			return pTmp;

		pTmp = pTmp->next;
	}

	return VO_NULL;
}

VO_BOOL CAviHeaderParser::GetStrdContent(VO_PBYTE* ppStrdContent, VO_U32* pdwStrdLength)
{
	if(!m_pStrdContent)
		return VO_FALSE;

	if(ppStrdContent)
		*ppStrdContent = m_pStrdContent;

	if(pdwStrdLength)
		*pdwStrdLength = m_dwStrdLength;

	return VO_TRUE;
}

CAviDataParser::CAviDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
	: CvoFileDataParser(pFileChunk, pMemOp)
	, m_btStreamCount(0)
{
}

CAviDataParser::~CAviDataParser()
{
}

VO_VOID CAviDataParser::Init(VO_PTR pParam)
{
	PAviDataParserInitParam pInitParam = (PAviDataParserInitParam)pParam;

	m_btStreamCount = pInitParam->btStreamCount;
}

VO_BOOL CAviDataParser::SetStartFilePos(VO_U64 ullStartFilePos)
{
	return m_pFileChunk->FLocate(ullStartFilePos);
}

#define is_digit(d)		((d) >= '0' && (d) <= '9')
#define is_data(d)		(is_digit((d) & 0xFF) && is_digit(((d) >> 8) & 0xFF) && !is_digit(((d) >> 16) & 0xFF) && !is_digit(((d) >> 24) & 0xFF))
#define get_stream(d)	((((d) & 0xFF) - 0x30) * 10 + ((((d) & 0xFF00) >> 8) - 0x30))
VO_BOOL CAviDataParser::ReadAviObject(AviObject& ao)
{
	ao.fcc = 0;
	VO_U8 btValue = 0, btFccRead = 0, btStreamNum = 0;
	VO_U16 wTwocc = 0;
	VO_U32 dwReaded = 0;

	while(btFccRead < 32)
	{
		if(dwReaded > 0x200000)	// if can not find AVI object within 2M, exit to avoid ANR
			return VO_FALSE;

		ptr_read_byte(btValue);
		dwReaded++;
		if(AVI_FOURCC_CHAR[btValue])
		{
			ao.fcc |= (VO_U32(btValue) << btFccRead);
			btFccRead += 8;

			if(btFccRead == 32)
			{
				if(!is_data(ao.fcc))
				{
					btFccRead -= 8;
					ao.fcc >>= 8;
					continue;
				}

				btStreamNum = get_stream(ao.fcc);
				if(btStreamNum >= m_btStreamCount)
				{
					btFccRead -= 8;
					ao.fcc >>= 8;
					continue;
				}

				wTwocc = (ao.fcc >> 16);
				if(wTwocc != TWOCC_dd && wTwocc != TWOCC_dc && wTwocc != TWOCC_db && wTwocc != TWOCC_wb && 
					wTwocc != TWOCC_pc && wTwocc != TWOCC_ix)
				{
					btFccRead -= 8;
					ao.fcc >>= 8;
					continue;
				}
			}
		}
		else
		{
			ao.fcc = 0;
			btFccRead = 0;
		}
	}
	ptr_read_dword(ao.size);

	return VO_TRUE;
}

VO_BOOL CAviDataParser::StepB()
{
	AviObject ao;
	if(!ReadAviObject(ao))
		return VO_FALSE;

	VO_U16 wType = VO_U16(ao.fcc >> 16);
	VO_U8 btStreamNum = get_stream(ao.fcc);
	if((m_btBlockStream != btStreamNum) && (wType == TWOCC_dd || wType == TWOCC_dc || wType == TWOCC_db || wType == TWOCC_wb))
	{
		AviDataParserSample sample;
		sample.wStreamTwocc = (ao.fcc & 0xFFFF);
		sample.dwLen = (TWOCC_dd == wType) ? (ao.size | 0x80000000) : ao.size;
		sample.pFileChunk = m_pFileChunk;
		VO_U8 btRes = m_fODCallback(m_pODUser, m_pODUserData, &sample);
		
		if(!CBRT_IS_CONTINUABLE(btRes))
			return VO_FALSE;

		if(CBRT_IS_NEEDSKIP(btRes))
		{
			skip_chunk();
		}
		else
		{
			if(ao.size & 0x1)
				ptr_skip(1);
		}
	}
	else
		skip_chunk();

	return VO_TRUE;
}