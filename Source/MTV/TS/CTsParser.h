#pragma once

#include "voYYDef_TS.h"
#include "CBaseParser.h"
#include "CTsParseCtroller.h"

#define MAX_CACHE_SIZE 512*1024
#define MAX_CACHE_BUF_COUNT 256

#define TS_PARSER_UNKNOWN        0
#define TS_PARSER_VIDEO_ONLY     1
#define TS_PARSER_AUDIO_ONLY     2
#define TS_PARSER_AUDIO_VIDEO    3
#define TS_MAX_ROLLBACK_COUNT    128
#define TS_SET_WITHOUT_TIMESTAMP_OFFSET  1
#define TS_MAX_TRACK_COUNT_AUDIO_VIDEO   16


//#define SAVE_MEM_ADDR
class CheckTimestampCache
{
public:
	CheckTimestampCache(){pCache = NULL;Init();};
	~CheckTimestampCache(void){Uninit();};

    VO_U32              ulType;
    VO_U32              ulPID;
	VO_U64				nLastTimestamp;
	VO_U32				nBufCount;
	VO_MTV_FRAME_BUFFER CheckTimestampBuf[MAX_CACHE_BUF_COUNT];

	VO_BYTE*			pCache;
	VO_U32				nCurrCachePos;

	void Init()
	{
		Reset();
#ifndef SAVE_MEM_ADDR
		pCache = new VO_BYTE[MAX_CACHE_SIZE];
#endif
	}

	void Uninit()
	{
		if(pCache)
		{
			delete pCache;
			pCache = NULL;
		}

		Reset();
	}

	void SetPIDValue(VO_U32  ulPIDValue)
	{
		ulPID = ulPIDValue;
	}

	void Reset()
	{
		nLastTimestamp	= 0xFFFFFFFFFFFFFFFFLL;
		nBufCount		= 0;
		nCurrCachePos	= 0;
	};

	bool InsertFrame(VO_MTV_FRAME_BUFFER* pFrame)
	{
		if(nBufCount+1 >= MAX_CACHE_BUF_COUNT)
			return false;

		CheckTimestampBuf[nBufCount].nCodecType		= pFrame->nCodecType;
		CheckTimestampBuf[nBufCount].nEndTime		= pFrame->nEndTime;
		CheckTimestampBuf[nBufCount].nFrameType		= pFrame->nFrameType;
		CheckTimestampBuf[nBufCount].nPos			= pFrame->nPos;
		CheckTimestampBuf[nBufCount].nSize			= pFrame->nSize;
		CheckTimestampBuf[nBufCount].nStartTime		= pFrame->nStartTime;

#ifdef SAVE_MEM_ADDR  // not memory copy, just save memory addr
		CheckTimestampBuf[nBufCount].pData			= pFrame->pData;
#else
		if((nCurrCachePos+pFrame->nSize) <= MAX_CACHE_SIZE)
		{
			memcpy(pCache+nCurrCachePos, pFrame->pData, pFrame->nSize);
			CheckTimestampBuf[nBufCount].pData = pCache+nCurrCachePos;
			nCurrCachePos += pFrame->nSize;
		}
		else
		{
			// need process this case
			return false;
		}
#endif
		nLastTimestamp = pFrame->nStartTime;
		nBufCount++;


		return true;
	}

	VO_U64 CalculateAvgTS(VO_U64 nEndTS)
	{
		VO_BOOL   bBigThanLastTime = VO_TRUE;
		VO_U64 avg = 0;
		if(nEndTS>nLastTimestamp)
		{
		    avg = (nEndTS - nLastTimestamp)/(nBufCount+1);
			bBigThanLastTime = VO_TRUE;
		}
		else
		{
			avg = (nLastTimestamp - nEndTS)/(nBufCount+1);
			bBigThanLastTime = VO_FALSE;
		}

		for(VO_U32 n=0; n<nBufCount; n++)
		{
			if(bBigThanLastTime == VO_TRUE)
			{
			    CheckTimestampBuf[n].nStartTime = nLastTimestamp + avg*(n+1);
			}
			else
			{
				CheckTimestampBuf[n].nStartTime = nLastTimestamp - avg*(n+1);
			}
		}

		return avg;
	}
};

class CTsParser	:	public CBaseParser
{
public:
	CTsParser(VO_MEM_OPERATOR* pMemOp);
	virtual ~CTsParser(void);

public:
	virtual VO_U32 Open(VO_PARSER_INIT_INFO* pParam);
	virtual	VO_U32 Close();
	virtual	VO_U32 Process(VO_PARSER_INPUT_BUFFER* pBuffer);
	virtual VO_U32 SelectProgram(VO_U32 uStreamID);

protected:
	virtual VO_U32 doSetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32 doGetParam(VO_U32 uID, VO_PTR pParam);
    void    ResetAllAVCache();
    void    InitAllAVCache();
    void    ResetAllAVCacheData();
    int     GetCacheIndexCacheByPID(VO_U32  ulPID);
	void    CreateCacheByPID(VO_U32  ulPID, VO_U32 ulType);


private:
	//static void RecvParseResult(VO_PARSER_OUTPUT_BUFFER* pBuf)
	static void OnParsed(VO_PARSER_OUTPUT_BUFFER* pBuf);
	void HandleParseResult(VO_PARSER_OUTPUT_BUFFER* pBuf);

	VO_BOOL PreprocessTimestamp(VO_U32 nOutputType, VO_MTV_FRAME_BUFFER* pBuf, VO_U32 ulPID);
    VO_VOID ResetCheckRollbackContext();
    VO_VOID ResetForFirstTimeStampForRollback();
    VO_BOOL CheckNeedResetFirstTime();
    VO_VOID IssueAllCacheFrame();

	VO_VOID   InitAllFileDump();
	VO_VOID   ResetAllFileDump();
	VO_VOID   CreateDumpByStreamId(VO_U32 ulPID);
	VO_VOID   DumpFrame(VO_PARSER_OUTPUT_BUFFER* pBuf);
	VO_VOID   CloseAllFileDump();
private:
	ITsParseController* m_pTsCtrl;
	VO_U64				m_nFirstTimeStamp;
	VO_U32				program_time;
    VO_U32              m_ulVideoTimeStampRollbackCount;
    VO_U32              m_ulAudioTimeStampRollbackCount;
    VO_U32              m_ulCurrentWorkMode;

	CheckTimestampCache* m_pAVCheckTimestampCache[TS_MAX_TRACK_COUNT_AUDIO_VIDEO];
    VO_U32              m_ulCurrentCacheCount;
    VO_U32              m_ulCurrentVideoPID;
	VO_U32              m_ulCurrentAudioPID;
    VO_U32              m_ulCurrentTextPID;
    VO_U32              m_uFlagForTimeStampOffset;
    VO_PTR              m_pHCheck;
#ifdef _DUMP_FRAME
    FILE*               m_pFileFrameData[0x2000];
	FILE*               m_pFileFrameSize[0x2000];
	FILE*               m_pFileFrameTimeStamp[0x2000];
#endif
};
