#pragma once

#include "voVideo.h"
#include "CDllLoad.h"
#include "CMemoryOpt.h"
#include "voMem.h"

class CBaseVideoDecoder : public CDllLoad , public CMemoryOpt
{
public:
	CBaseVideoDecoder(VO_MEM_OPERATOR *pMemOpt );
	virtual ~CBaseVideoDecoder(void);

	virtual HRESULT InitDecoder(VO_VIDEO_CODINGTYPE nCodec , BYTE *pExtData , int nExtDataLen);
	virtual HRESULT SetInput(BYTE *pBuf , int nBufLen , LONGLONG llStartTime);
	virtual HRESULT GetOutput(BYTE **pBuf , int &nBufLen , LONGLONG &llStartTime );

	virtual HRESULT SetParam(long nParamID , void *pData);
	virtual HRESULT GetParam(long nParamID , void *pData);
protected:
	HRESULT	LoadDLL (VO_HANDLE hInst);

	virtual HRESULT UpdateParam();
	virtual HRESULT SetHeadData(BYTE *pData , int nDataLen);

protected:
	VO_VIDEO_DECAPI				mfunDec;
	VO_HANDLE					mhDec;

	VO_VIDEO_CODINGTYPE			mnCodec;

	//BYTE *						mpExtData;
	//int							mnExtDataLen;

	VO_MEM_VIDEO_OPERATOR		mpVideoMemOp;
};
