#pragma once
#include "CBaseAnalyseData.h"


class CAnalyseH265 :public CBaseAnalyseData
{
public:
	CAnalyseH265(void);
	virtual ~CAnalyseH265(void);

	virtual VO_BOOL	AnalyseData(VO_BYTE *pData , VO_U32 nDataLen);
	virtual VO_BOOL	IsKeyFrame(VO_BYTE *pData , VO_U32 nDataLen);

	virtual VO_U32 TrimSample(VO_BYTE **ppData , VO_U32 &nDataLen);

	virtual void   SetFormat(void *pFmt);
protected:
	VO_BOOL	IsKeyFrm(VO_BYTE *pData , VO_U32 nDataLen);

	void	STH265ToAVC(VO_BYTE **ppData , VO_U32 &nDataLen);
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
