/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		VideoEncoderCtrl.cpp

Contains:	VideoEncoderCtrl class file

Written by:	Leon Huang

Change History (most recent first):
2011-01-18		Leon			Create file
*******************************************************************************/


#include "VideoEncoderCtrl.h"

CVideoEncoderCtrl::CVideoEncoderCtrl(VO_LIB_OPERATOR* pParam,void* pUserData,VOEDTENCODERCALLBACKPROC proc)
: CEncoderBaseCtrl (pParam,pUserData, proc)
,m_videoEncoder(NULL)
{
}

CVideoEncoderCtrl::~CVideoEncoderCtrl(void)
{
	ReleaseCtrl();
}

VO_VOID CVideoEncoderCtrl::ReleaseCtrl()
{
	if(m_videoEncoder)	delete m_videoEncoder;
	if(m_OutData.Buffer) delete m_OutData.Buffer;
	m_OutData.Buffer = NULL;
	m_videoEncoder = NULL;
}


VO_S32 CVideoEncoderCtrl::SetEncoderParam(void *param)
{
	VO_S32 nRC = 0;
	ReleaseCtrl();


	VOEDT_OUTPUT_VIDEO_PARAM *encParam = (VOEDT_OUTPUT_VIDEO_PARAM*)param;
	m_nCodecType = encParam->nCodecType;
	m_videoEncoder = new CVideoEncoder(NULL,m_nCodecType,NULL);
	m_videoEncoder->SetLibOperator(m_pLibOp);
	nRC = m_videoEncoder->SetEncParam(encParam);
	nRC = m_videoEncoder->Init();
	if( nRC == VO_ERR_NONE)
	{
		int nOutBufLen = 0;
		unsigned char * pOutBuf ;
		VO_S32 nPid;
		if(m_nCodecType == VO_VIDEO_CodingMPEG4 || m_nCodecType == VO_VIDEO_CodingH263)
			nPid = VO_PID_ENC_MPEG4_OUTBUF_SIZE;
		else
			nPid = VO_PID_ENC_H264_OUTBUF_SIZE;
		/* Malloc output buffer*/
		/* Get default output buffer length */
		nRC = m_videoEncoder->GetParam(nPid,&nOutBufLen);
		pOutBuf = (unsigned char *)malloc(nOutBufLen);
		if (!pOutBuf) nRC = VOEDT_ERR_BUFFER_INIT_FAILED;
		m_OutData.Buffer = pOutBuf;
	}
	return nRC;
}
VO_S32  CVideoEncoderCtrl::ToEncoder(void  *InData)
{
	if(!m_videoEncoder) return VO_ERR_FAILED;
	VO_VIDEO_BUFFER * buf = (VO_VIDEO_BUFFER*)InData;
	VOEDT_ENCODER_SAMPLE sample;
	VO_S32 nRc = m_videoEncoder->Process(buf,&m_OutData,&sample.pType);
	if(nRc == VO_ERR_NONE)
	{
		//temporary write
		//because H264 Encoder returned pType is different from voVideo.h declared
		//He defined that VO_VIDEO_FRAME_P == 0; VO_VIDEO_FRAME_I == 1
		//in voVideo.h that VO_VIDEO_FRAME_I == 1; VO_VIDEO_FRAME_P == 0
		//if(m_nCodecType == VO_VIDEO_CodingH264 && sample.pType == VO_VIDEO_FRAME_P ) sample.pType = VO_VIDEO_FRAME_I;
		//else if(m_nCodecType == VO_VIDEO_CodingH264 && sample.pType == VO_VIDEO_FRAME_I ) sample.pType = VO_VIDEO_FRAME_P;

		if(sample.pType == VO_VIDEO_FRAME_I ) m_OutData.Length |= 0x80000000;
		sample.nSampleType = 9;
		sample.buffer = &m_OutData;
		sample.buffer->Time = buf->Time;
		//sample.Duration = 40;
		VOEDT_PLAYER_CALLBACK_BUFFER pcb;
		pcb.pUserData = m_pUserData;
		pcb.pData = &sample;
		m_proc(-1,&pcb);
	}
	return nRc;
}
