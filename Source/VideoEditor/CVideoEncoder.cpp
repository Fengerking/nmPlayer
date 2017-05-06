/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CVideoEditorEngine.cpp

Contains:	CVideoEditorEngine class file

Written by:	Leon Huang

Change History (most recent first):
2011-01-06		Leon			Create file
*******************************************************************************/
#include "voMMPlay.h"
#include "voOSFunc.h"

#include "CVideoEncoder.h"
#include "voMPEG4.h"

#include "voLog.h"

typedef VO_S32 (VO_API * VOGETVIDEOENCAPI) (VO_VIDEO_ENCAPI * pDecHandle);

CVideoEncoder::CVideoEncoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP)
	: CBaseNode (hInst, pMemOP)
	, m_hEnc (NULL)
{
}

CVideoEncoder::~CVideoEncoder ()
{
	Uninit ();
}

VO_U32 CVideoEncoder::Init ()
{
	VO_S32 nCodecType = m_outputParam.nCodecType;
	int nWidth = m_outputParam.nWidth;
	int nHeight =m_outputParam.nHeight;

	if (nCodecType > VO_VIDEO_CodingDIVX || nCodecType < VO_VIDEO_CodingMPEG2)
	{
		if (m_pError != NULL)
			vostrcpy (m_pError, _T("The video format is unknown!"));
		return VO_ERR_INVALID_ARG;
	}	

	VO_U32 nRC = VO_ERR_NOT_IMPLEMENT;
	while (nRC != VO_ERR_NONE)
	{
		Uninit ();
		VOLOGI("Before LoadLib (m_hInst) .");
		if (LoadLib (m_hInst) == 0)
		{
			VOLOGE ("LoadLib (m_hInst) Failed.");
			return VO_ERR_WRONG_STATUS;
		}
		VOLOGI("After LoadLib (m_hInst) .");
		VO_CODEC_INIT_USERDATA	*initInfo = NULL;
		/*memset (&initInfo, 0, sizeof (VO_CODEC_INIT_USERDATA));
		initInfo.memflag = VO_IMF_USERMEMOPERATOR;
		initInfo.memData = m_pMemOP;*/
/*
		if (m_pLibOP != NULL)
		{
			initInfo.memflag |= 0X10;
			initInfo.libOperator = m_pLibOP;
		}
*/
		VOLOGI("Before m_funEnc.Init codecType %d.",nCodecType);
		nRC = m_funEnc.Init (&m_hEnc, (VO_VIDEO_CODINGTYPE)nCodecType, initInfo);
		if (nRC != VO_ERR_NONE || m_hEnc == NULL)
		{
			VOLOGE ("m_funEnc.Init Failed. 0X%08X", nRC);
			return VO_ERR_NOT_IMPLEMENT;
		}
		/*VO_VIDEO_FORMAT vf;
		vf.Height =nHeight;
		vf.Width = nWidth;
		m_funEnc.SetParam (m_hEnc, VO_PID_VIDEO_FORMAT, &vf);*/
		VOLOGI("After m_funEnc.Init .");

		VOLOGI("Before UpdateParam.");
		UpdateParam ();
		VOLOGI("After UpdateParam.");
		if (nRC == VO_ERR_NONE)
			break;

		if (m_bVOUsed)
			break;

		m_bVOUsed = VO_TRUE;
	}

	return nRC;
}
VO_S32  CVideoEncoder::SetEncParam(VOEDT_OUTPUT_VIDEO_PARAM *param)
{
	memcpy(&m_outputParam,param,sizeof(VOEDT_OUTPUT_VIDEO_PARAM));
	return VO_ERR_NONE;
}
VO_U32 CVideoEncoder::UpdateParam (void)
{

	VO_U32 nRC = 0;
//	nRC = SetParam (VO_PID_COMMON_LIBOP, m_pLibOP);

	//float  fFrameRate		= 15.00f;
	//VO_U32 nKeyFrmInterval	= 15;
	//VO_U32 nRotateType		= VO_RT_DISABLE;

	double ratio = 4.0; //quality_normal
	if (m_outputParam.nEncQuality == VO_ENC_LOW_QUALITY) //good
		ratio = 3.0;
	else if (m_outputParam.nEncQuality  == VO_ENC_HIGH_QUALITY) //best
		ratio = 6.0;

	VO_U32 nBitrate = m_outputParam.nBitRate;
	
//	m_nBitrate = (int)(ratio * m_outputParam.nWidth * m_outputParam.nHeight);
//	m_nBitrate *= 1.2;

//	m_funEnc.SetParam (m_hEnc, VO_PID_VIDEO_QUALITY, &m_nQuality);
//	m_funEnc.SetParam (m_hEnc, VO_PID_VIDEO_BITRATE, &m_nBitrate);
	float framerate = m_outputParam.nFrameRate / 100.;
	if (m_outputParam.nCodecType == VO_VIDEO_CodingH264)
	{
		nRC |= SetParam(VO_PID_ENC_H264_BITRATE,			    &nBitrate);
		nRC |= SetParam(VO_PID_ENC_H264_FRAMERATE,				&framerate);
		nRC |= SetParam(VO_PID_ENC_H264_WIDTH,					&m_outputParam.nWidth);
		nRC |= SetParam(VO_PID_ENC_H264_HEIGHT,			    &m_outputParam.nHeight);
		nRC |= SetParam(VO_PID_ENC_H264_KEYFRAME_INTERVAL,	    &m_outputParam.nKeyFrmInterval);
		nRC |= SetParam(VO_PID_ENC_H264_VIDEO_QUALITY,				&m_outputParam.nEncQuality);
	}
	else if (m_outputParam.nCodecType == VO_VIDEO_CodingH263 || m_outputParam.nCodecType == VO_VIDEO_CodingMPEG4)
	{
		nRC = SetParam (VO_PID_ENC_MPEG4_WIDTH,					&m_outputParam.nWidth);
		nRC = SetParam (VO_PID_ENC_MPEG4_HEIGHT,			    &m_outputParam.nHeight);
		nRC = SetParam (VO_PID_ENC_MPEG4_BITRATE,			    &nBitrate);
		nRC = SetParam (VO_PID_ENC_MPEG4_VIDEO_QUALITY,			&m_outputParam.nEncQuality);
		nRC = SetParam (VO_PID_ENC_MPEG4_FRAMERATE,				&framerate);
		nRC = SetParam (VO_PID_ENC_MPEG4_KEY_FRAME_INTERVAL,	&m_outputParam.nKeyFrmInterval);
		nRC = SetParam (VO_PID_ENC_MPEG4_INPUT_ROTATION,		&m_outputParam.nRotateType);

	}

	return 0;
}

VO_U32 CVideoEncoder::Uninit (void)
{

	if (m_hEnc != NULL)
	{
		m_funEnc.Uninit (m_hEnc);
		m_hEnc = NULL;
		memset(&m_outputParam,0x00,sizeof(m_outputParam));
	}

	FreeLib ();

	return 0;
}

VO_U32 CVideoEncoder::Start(void)
{

	VO_U32	nValue = 0;
	return 	SetParam (VO_PID_COMMON_START, &nValue);
}

VO_U32 CVideoEncoder::Pause(void)
{

	VO_U32	nValue = 0;
	return 	SetParam (VO_PID_COMMON_PAUSE, &nValue);
}

VO_U32 CVideoEncoder::Stop(void)
{

	VO_U32	nValue = 0;
	return 	SetParam (VO_PID_COMMON_STOP, &nValue);
}

VO_U32 CVideoEncoder::Process (VO_VIDEO_BUFFER * pInput, VO_CODECBUFFER * pOutput, VO_VIDEO_FRAMETYPE * pType)
{

	if (m_hEnc == NULL)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	VO_U32 nRC = m_funEnc.Process (m_hEnc, pInput, pOutput, pType);

	return nRC;
}

VO_U32 CVideoEncoder::SetParam (VO_S32 uParamID, VO_PTR pData)
{

	if (m_hEnc == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	return 	m_funEnc.SetParam (m_hEnc, uParamID, pData);
}

VO_U32 CVideoEncoder::GetParam (VO_S32 uParamID, VO_PTR pData)
{

	if (m_hEnc == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	return 	m_funEnc.GetParam (m_hEnc, uParamID, pData);
}

VO_U32 CVideoEncoder::LoadLib (VO_HANDLE hInst)
{

	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;
	switch(m_outputParam.nCodecType)
	{
	case VO_VIDEO_CodingMPEG4:
		strcpy (m_szCfgItem, "Video_Enc_MPEG4");
		vostrcpy (m_szDllFile, _T("voMPEG4Enc"));
		vostrcpy (m_szAPIName, _T("voGetMPEG4EncAPI"));
		break;
	case VO_VIDEO_CodingH263:
		strcpy (m_szCfgItem, "Video_Enc_H263");
		vostrcpy (m_szDllFile, _T("voMPEG4Enc"));
		vostrcpy (m_szAPIName, _T("voGetMPEG4EncAPI"));
		break;
	case VO_VIDEO_CodingH264:
		strcpy (m_szCfgItem, "Video_Enc_H264");
		vostrcpy (m_szDllFile, _T("voH264Enc"));
		vostrcpy (m_szAPIName, _T("voGetH264EncAPI"));
	default:
		break;
	}

	if (m_pConfig != NULL)
	{
		pDllFile = m_pConfig->GetItemText (m_szCfgItem, (char*)"File");
		pApiName = m_pConfig->GetItemText (m_szCfgItem, (char*)"Api");
	}
	if (pDllFile == NULL)
	{
		m_bVOUsed = VO_TRUE;
	}

#if defined _WIN32
	if (pDllFile != NULL && !m_bVOUsed)
	{
		TCHAR voDllFile[256];
		_tcscpy (voDllFile, m_szDllFile);

		memset (m_szDllFile, 0, sizeof (m_szDllFile));
		MultiByteToWideChar (CP_ACP, 0, pDllFile, -1, m_szDllFile, sizeof (m_szDllFile));

		if (!_tcscmp (voDllFile, m_szDllFile))
			m_bVOUsed = VO_TRUE;
	}
	vostrcat (m_szDllFile, _T(".Dll"));

	if (pApiName != NULL && !m_bVOUsed)
	{
		memset (m_szAPIName, 0, sizeof (m_szAPIName));
		MultiByteToWideChar (CP_ACP, 0, pApiName, -1, m_szAPIName, sizeof (m_szAPIName));
	}
#elif defined LINUX
	if (pDllFile != NULL && !m_bVOUsed)
	{
		if (!vostrcmp (m_szDllFile, pDllFile))
			m_bVOUsed = VO_TRUE;

		vostrcpy (m_szDllFile, pDllFile);
	}
	vostrcat (m_szDllFile, _T(".so"));

	if (pApiName != NULL && !m_bVOUsed)
		vostrcpy (m_szAPIName, pApiName);
#endif

	if (CBaseNode::LoadLib (m_hInst) == 0)
	{
		VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}
	if(m_pAPIEntry== NULL) VOLOGE ("LoadLib (m_hInst) Failed.");

	VOGETVIDEOENCAPI pAPI = (VOGETVIDEOENCAPI) m_pAPIEntry;
	pAPI (&m_funEnc);

	return 1;
}
