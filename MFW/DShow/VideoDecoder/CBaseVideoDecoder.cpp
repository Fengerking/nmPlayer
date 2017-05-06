#include "CBaseVideoDecoder.h"
#include "streams.h"


typedef VO_S32 (VO_API * VOGETVIDEODECAPI) (VO_VIDEO_DECAPI * pDecHandle);

CBaseVideoDecoder::CBaseVideoDecoder(VO_MEM_OPERATOR *pMemOpt)
: CMemoryOpt(pMemOpt , VO_INDEX_FILTER_VIDEODECODER)
//, mpExtData(NULL)
//, mnExtDataLen(0)
 , mpVideoMemOp(NULL)
{
}

CBaseVideoDecoder::~CBaseVideoDecoder(void)
{
}

HRESULT CBaseVideoDecoder::InitDecoder(VO_VIDEO_CODINGTYPE nCodec , BYTE *pExtData , int nExtDataLen)
{
	//if(mpExtData != NULL)
	//{
	//	Free(mpExtData);
	//	mpExtData = NULL;
	//}

	//mnExtDataLen = nExtDataLen;
	//if(mnExtDataLen > 0)
	//{
	//	mpExtData = Alloc(mnExtDataLen);
	//	Copy(mpExtData , pExtData , mnExtDataLen);
	//}

	mnCodec = nCodec;

	HRESULT  hr = LoadDLL(g_hInst);
	if(hr != S_OK)
		return hr;

	VO_CODEC_INIT_USERDATA	initInfo;
	Set(&initInfo, 0, sizeof (VO_CODEC_INIT_USERDATA));
	initInfo.memflag = VO_IMF_USERMEMOPERATOR;
	initInfo.memData = mpMemOpt;

	VO_U32 nRC = mfunDec.Init(&mhDec , mnCodec , &initInfo);
	if(nRC != VO_ERR_NONE)
		return E_FAIL;



	return E_NOTIMPL;
}

HRESULT CBaseVideoDecoder::SetInput(BYTE *pBuf , int nBufLen , LONGLONG llStartTime)
{
	return E_NOTIMPL;
}

HRESULT CBaseVideoDecoder::GetOutput(BYTE **pBuf , int &nBufLen , LONGLONG &llStartTime )
{
	return E_NOTIMPL;
}

HRESULT CBaseVideoDecoder::SetParam(long nParamID , void *pData)
{
	if (nParamID == VO_PID_VIDEO_VIDEOMEMOP)
		mpVideoMemOp = (VO_MEM_VIDEO_OPERATOR *) pData;

	if (mhDec == 0)
		return VO_ERR_WRONG_STATUS;
	return E_NOTIMPL;
}

HRESULT CBaseVideoDecoder::GetParam(long nParamID , void *pData)
{
	return E_NOTIMPL;
}

HRESULT CBaseVideoDecoder::UpdateParam()
{


	if (mpVideoMemOp != NULL)
		SetParam (VO_PID_VIDEO_VIDEOMEMOP, mpVideoMemOp);

	if (mnCodec == VO_VIDEO_CodingH264)
	{
		VO_H264FILEFORMAT nStreamFormat = VO_H264_ANNEXB;

		if(memcmp((char*)&m_nFourCC, (char*)"1CVA", 4) == 0)
			nStreamFormat = VO_H264_AVC;
		else if(memcmp((char*)&m_nFourCC, (char*)"0CVA", 4) == 0)
			nStreamFormat = VO_H264_14496_15;
		else
			nStreamFormat = VO_H264_ANNEXB;

		//		VOLOGI ("H264 Stream Type %d", nStreamFormat);

		nRC = SetParam (VO_ID_H264_STREAMFORMAT, &nStreamFormat);
	}


	//mfunDec.SetParam (mhDec, VO_PID_VIDEO_FORMAT, &m_OutputInfo);

	return 0;
}

HRESULT CBaseVideoDecoder::SetHeadData(BYTE *pData , int nDataLen)
{

}

HRESULT	CBaseVideoDecoder::LoadDLL (VO_HANDLE hInst)
{
	
	if (mnCodec== VO_VIDEO_CodingMPEG4)
	{
		vostrcpy (m_szDllFile, _T("voMPEG4Dec"));
		vostrcpy (m_szAPIName, _T("voGetMPEG4DecAPI"));
	}
	else if (mnCodec== VO_VIDEO_CodingH263)
	{
		vostrcpy (m_szDllFile, _T("voMPEG4Dec"));
		vostrcpy (m_szAPIName, _T("voGetMPEG4DecAPI"));
	}
	else if (mnCodec== VO_VIDEO_CodingS263)
	{
		vostrcpy (m_szDllFile, _T("voMPEG4Dec"));
		vostrcpy (m_szAPIName, _T("voGetMPEG4DecAPI"));
	}
	else if (mnCodec== VO_VIDEO_CodingH264)
	{
		vostrcpy (m_szDllFile, _T("voH264Dec"));
		vostrcpy (m_szAPIName, _T("voGetH264DecAPI"));
	}
	else if (mnCodec== VO_VIDEO_CodingWMV)
	{
		vostrcpy (m_szDllFile, _T("voWMVDec"));
		vostrcpy (m_szAPIName, _T("voGetWMV9DecAPI"));
	}
	else if (mnCodec== VO_VIDEO_CodingRV)
	{
		vostrcpy (m_szDllFile, _T("voRealVideoDec"));
		vostrcpy (m_szAPIName, _T("voGetRVDecAPI"));
	}
	else if (mnCodec== VO_VIDEO_CodingMPEG2)
	{
		vostrcpy (m_szDllFile, _T("voMPEG2Dec"));
		vostrcpy (m_szAPIName, _T("voGetMPEG2DecAPI"));
	}
	else if (mnCodec== VO_VIDEO_CodingMJPEG)
	{
		vostrcpy (m_szDllFile, _T("voMJPEGDec"));
		vostrcpy (m_szAPIName, _T("voGetMJPEGDecAPI"));
	}
	else if (mnCodec== VO_VIDEO_CodingDIVX)
	{
		vostrcpy (m_szDllFile, _T("voDIVX3Dec"));
		vostrcpy (m_szAPIName, _T("voGetDIVX3DecAPI"));
	}
	else if (mnCodec== VO_VIDEO_CodingVP6)
	{
		vostrcpy (m_szDllFile, _T("voVP6Dec"));
		vostrcpy (m_szAPIName, _T("voGetVP6DecAPI"));
	}


	vostrcat (m_szDllFile, _T(".Dll"));

	if (LoadLib (g_hInst) == 0)
	{
		return E_NOTIMPL;
	}

	VOGETVIDEODECAPI pAPI = (VOGETVIDEODECAPI) m_pAPIEntry;
	pAPI (&mfunDec);

	return 1;
}