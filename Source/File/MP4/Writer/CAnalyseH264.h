#pragma once
#include "CBaseAnalyseData.h"

enum StreamType {
	ST_AVC4 = 0x5354 , //'ST',  //4 bytes show packet size
	ST_AVC2,        //2 bytes show packet size
	ST_H264,       //start bytes is  00 00 00 01 
	ST_RAW,        //Only video packet data
};

class CAnalyseH264 :public CBaseAnalyseData
{
public:
	CAnalyseH264(void);
	virtual ~CAnalyseH264(void);

	virtual VO_BOOL	AnalyseData(VO_BYTE *pData , VO_U32 nDataLen);
	virtual VO_BOOL	IsKeyFrame(VO_BYTE *pData , VO_U32 nDataLen);

	virtual VO_U32 TrimSample(VO_BYTE **ppData , VO_U32 &nDataLen);

	virtual void   SetFormat(void *pFmt);
protected:
	VO_BOOL	IsKeyFrm(VO_BYTE *pData , VO_U32 nDataLen);

	void	STH264ToAVC(VO_BYTE **ppData , VO_U32 &nDataLen);
	void	WriteDataLen(VO_BYTE *pData , VO_U32 nDataLen);

	void    STRawToAVC(VO_BYTE **ppData , VO_U32 &nDataLen);

	void    DetectStreamType(VO_BYTE *pData , VO_U32 nDataLen);
	bool	AllocFrameBuf();
	bool	WriteRawDataToBuf(VO_BYTE *pData , VO_U32 nDataLen);

protected:
	VO_U32  mnSPSLen ;
	VO_U32  mnPPSLen ;

	VO_S32	mnStreamType;
	VO_BYTE * mpData;
	VO_U32  mnDataLen;
	VO_U32  mnWritedLen;

	VO_U32  mnNestDeep;
};
