
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		AudioEncoderCtrl.cpp

Contains:	AudioEncoderCtrl class file

Written by:   Leon Huang
Change History (most recent first):
2011-01-13		Leon		 Create file
*******************************************************************************/
#include "AudioEncoderCtrl.h"

#define AUDIO_BUFFER_LEN 1024*8

CAudioEncoderCtrl::CAudioEncoderCtrl(VO_LIB_OPERATOR* pLibOp,void* pUserData,VOEDTENCODERCALLBACKPROC proc)
:CEncoderBaseCtrl(pLibOp,pUserData,proc)
,m_audioEncoder(NULL)
{
		m_OutData.Buffer = (unsigned char *)malloc(AUDIO_BUFFER_LEN);
		m_OutData.Length = AUDIO_BUFFER_LEN;
}

CAudioEncoderCtrl::~CAudioEncoderCtrl(void)
{
	ReleaseCtrl();
}
VO_VOID CAudioEncoderCtrl::ReleaseCtrl()
{
	if(m_audioEncoder) delete m_audioEncoder;
	m_audioEncoder = NULL;
	m_nPreviousTime = 0;
}
VO_S32 CAudioEncoderCtrl::SetEncoderParam(void *param)
{	
	ReleaseCtrl();
	VO_S32 nRC = 0;
	VOEDT_OUTPUT_AUDIO_PARAM *encParam = (VOEDT_OUTPUT_AUDIO_PARAM*)param;
	VOEDT_AUDIO_CODINGTYPE nCodecType = encParam->nCodecType;
	m_audioEncoder = new CAudioEncoder(NULL,nCodecType,NULL);
	m_audioEncoder->SetLibOperator(m_pLibOp);
	nRC = m_audioEncoder->SetEncParam(encParam);
	nRC = m_audioEncoder->Init();
	if( nRC == VO_ERR_NONE)
	{
		return nRC;
	}
	return nRC;
}

VO_S32  CAudioEncoderCtrl::ToEncoder(void *InData)
{
	if(!m_audioEncoder) return VO_ERR_FAILED;

	VO_AUDIO_OUTPUTINFO outInfo;
	VOMP_BUFFERTYPE * buf = (VOMP_BUFFERTYPE*)InData;
	VOEDT_ENCODER_SAMPLE sample;
	VO_CODECBUFFER inData;
	inData.Buffer = buf->pBuffer;
	inData.Length = buf->nSize;
	inData.Time	  = buf->llTime;
	VO_U32 nRc = m_audioEncoder->SetInputData(&inData);
	if(nRc == VO_ERR_NONE) 
	{
		do
		{
			m_OutData.Length = AUDIO_BUFFER_LEN;
			
			nRc = m_audioEncoder->GetOutputData(&m_OutData,&outInfo);
			if(nRc != VO_ERR_NONE) 
				break;
			
			VO_U32 lltime = (buf->llTime - m_nPreviousTime) * outInfo.InputUsed * 1.0/ buf->nSize ;

			sample.nSampleType = 7;
			sample.buffer = &m_OutData;
			sample.buffer->Time = lltime +  m_nPreviousTime;//buf->llTime;
			//sample.buffer->Time =  buf->llTime;
			//sample.Duration =  buf->llTime;
			VOEDT_PLAYER_CALLBACK_BUFFER pcb;
			pcb.pUserData = m_pUserData;
			pcb.pData = &sample;
			if(m_OutData.Length > 0)	m_proc(-1,&pcb);
		}while(!m_bExit && nRc != (VO_ERR_INPUT_BUFFER_SMALL));
	}
	m_nPreviousTime = buf->llTime;

	return nRc;
}
