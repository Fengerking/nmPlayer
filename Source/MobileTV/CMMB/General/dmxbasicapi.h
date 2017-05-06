#ifndef _DMX_BASIC_API_H_
#define _DMX_BASIC_API_H_

#include "dmxbasicdef.h"
//#include <tchar.h>


/***********************************************
************************************************
*				bit operation                  *
************************************************
************************************************/
inline DWORD	GET32(const LPBYTE p)	{return (p[0]<<24) + (p[1] <<16) + (p[2]<<8) +  p[3];}
inline WORD		GET16(const LPBYTE p)	{return (p[0]<<8) + p[1];}
inline BYTE		GET8(const LPBYTE p)	{return p[0];}

inline DWORD STEP32(LPBYTE& rP)
{
	DWORD ret = (rP[0]<<24) + (rP[1] <<16) + (rP[2]<<8) +  rP[3];
	rP+=4;
	return ret;
}

inline DWORD STEP24(LPBYTE& rP)
{
	DWORD ret = (rP[0] <<16) + (rP[1]<<8) +  rP[2];
	rP+=3;
	return ret;
}

inline WORD STEP16(LPBYTE& rP)
{
	WORD ret = (rP[0]<<8) + rP[1];
	rP+=2;
	return ret;
}

inline BYTE STEP8(LPBYTE& rP)
{
	BYTE ret = rP[0];
	rP++;
	return ret;
}

inline void STEP_S(LPBYTE const dst, LPBYTE& p, size_t size)
{
	if(dst)
	{
		size_t i;
		for(i=0;i<size;i++)
		{
			dst[i] = p[i];
		}
	}
	p+=size;
}

/***********************************************
************************************************
*				CRC check                  *
************************************************
************************************************/
void BuildCrcTable();
unsigned char CalcCRC8(const LPBYTE pbuf, size_t len) ;
unsigned long CalcCRC32(const LPBYTE inData, size_t inLen);
bool CheckCRC32(const LPBYTE lpIn, size_t len);


/***********************************************
************************************************
*				parse operation                  *
************************************************
************************************************/

int ParseNIT(BYTE* pData, int nLen, NetworkInfoTable* pInfo);
int ParseCMCT(BYTE* pData, int nLen, ContinueServiceMultiControlTable* pCMCT);
int ParseCSCT(BYTE* pData, int nLen, ContinueServiceControlTable* pCSCT);
int ParseEBDT(BYTE* pData, int nLen, ESgBasicDescription* pEsg);
int ParseEADesc(BYTE* pData, int nLen, EncryptionAuthorizationDescTable* pDesc);
int ParseEB(BYTE* pData, int nLen, EmegencyBroadcast* pEB);
int ParseMFHeader(BYTE* pData, MultiplexFrameHeader* pHeader);
int ParseMSFHeader(BYTE* pData, MultiSubFrameHeader* pHeader);
int ParseVideoSegmentHeader(LPBYTE pData, VideoSegmentHeader* pHeader);
FRAMEPOS CalFramePos(BYTE nStartFlag, BYTE nEndFlag);
int ParseFragmentMode2Header(BYTE* pData, Mode2FragmentHeader* pHeader);
int _ParseMode2VideoFragment(BYTE* pData, Mode2FragmentHeader* pHeader, FRAMEPOS nPos, VideoUnitBuf* pFrame);
int ParseMode2AudioFragment(BYTE* pData, Mode2FragmentHeader* pHeader, AudioUnitBuf* pUnit);
int _ParseFragmentMode2(BYTE* pData, Mode2FragmentHeader* pHeader, void* pOut);
int ParseDraFragmentMode2(BYTE* pData, Mode2FragmentHeader* pHeader, void* pOut);
int ParseAudioSegmentHeader(LPBYTE pData, AudioSegmentHeader* pHeader);
int ParseDataSegmentHeader(LPBYTE pData, DataSegmentHeader* pHeader);
int ParseProgramGuideInfo(BYTE* pData, ProgramGuideInfo* pInfo);
int ParseEsgDataSegmentHeader(BYTE* pData, EsgDataSegmentHeader* pHeader);
int ParseEsgDataSegment(BYTE* pData, EsgDataSegment* pSeg);




#endif //_DMX_BASIC_API_H_