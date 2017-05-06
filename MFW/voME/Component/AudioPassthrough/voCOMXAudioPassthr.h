	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXAudioPassthr.h

	Contains:	voCOMXAudioPassthr header file

	Written by:	Rogine Xu

	Change History (most recent first):
	2012-05-09	XRJ			Create file

*******************************************************************************/

#ifndef __voCOMXAudioPassthr_H__
#define __voCOMXAudioPassthr_H__

#include "voOMX_Index.h"
#include "voCOMXCompFilter.h"

static const int MAX_FRAME_SIZE = 48000;

static const int halfrate_tbl[12] = 
{ 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3
};
static const int lfe_mask[] = 
{ 
	16, 16, 4, 4, 4, 1, 4, 1
};
static const int bitrate_tbl[] = 
{ 
	32,  40,  48,  56,  64,  80,  96, 112,
	128, 160, 192, 224, 256, 320, 384, 448,
	512, 576, 640 
};
/*static const unsigned short frame_size_code_tbl[][] =
{
{  64,   69,   96},
{  64,   70,   96},
{  80,   87,  120},
{  80,   88,  120},
{  96,  104,  144},
{  96,  105,  144},
{ 112,  121,  168},
{ 112,  122,  168},
{ 128,  139,  192},
{ 128,  140,  192},
{ 160,  174,  240},
{ 160,  175,  240},
{ 192,  208,  288},
{ 192,  209.  288},
{ 224,  243,  336},
{ 224,  244,  336},
{ 256,  278,  384},
{ 256,  279,  384},
{ 320,  348,  480},
{ 320,  349,  480},
{ 384,  417,  576},
{ 384,  418,  576},
{ 448,  487,  672},
{ 448,  488,  672},
{ 512,  557,  768},
{ 512,  558,  768},
{ 640,  696,  960},
{ 640,  697,  960},
{ 768,  835, 1152},
{ 768,  836, 1152},
{ 896,  975, 1344},
{ 896,  976, 1344},
{1024, 1114, 1536},
{1024, 1115, 1536},
{1152, 1253, 1728},
{1152, 1254, 1728},
{1280, 1393, 1920},
{1280, 1394, 1920}
};*/
static const int acmod2channels_tbl[] = 
{
	2, 1, 2, 3, 3, 4, 4, 5
};

class voCOMXAudioPassthr : public voCOMXCompFilter
{
public:
	voCOMXAudioPassthr(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXAudioPassthr(void);

	virtual OMX_ERRORTYPE	EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											 OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE	GetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nParamIndex,  
										OMX_INOUT OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

protected:
	virtual OMX_ERRORTYPE	InitPortType (void);

	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	Flush (OMX_U32	nPort);

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled);

	virtual VO_U32			GetSingleSample(OMX_BUFFERHEADERTYPE * pObj, VO_CODECBUFFER * pDst, OMX_BOOL assemble);

	virtual VO_U32			PassThroughDataProc (VO_CODECBUFFER * pInput, VO_CODECBUFFER * pOutput);

	virtual bool            IsBigEndian(VO_CODECBUFFER * pInput);

	virtual VO_U32          FindSyncword(OMX_BUFFERHEADERTYPE * pInput);

	virtual VO_U32          ParseHeader(unsigned char *hdr, int &frameSize, int &sampleRate, 
										int &numChannels, int &bitRate, int &bsmod, bool &isBigEndian);

	virtual VO_U32          GetSampleLenDTS(unsigned char *hdr, int &frameSize);

protected:
	OMX_AUDIO_CODINGTYPE			m_nCoding;
	OMX_PTR							m_pFormatData;
	OMX_U32							m_nExtSize;
	OMX_S8 *						m_pExtData;

	OMX_AUDIO_PARAM_PCMMODETYPE		m_pcmType;

	OMX_U32							m_nSampleRate;
	OMX_U32							m_nChannels;
	OMX_U32							m_nBits;
	OMX_U32							m_nStereoMode;

	OMX_BOOL						m_bSetThreadPriority;

	OMX_U32							m_nPassableError;
	OMX_U32							mnSourceType;

protected:
	VO_MEM_OPERATOR *				m_pMemOP;
	VO_U32							m_nCodec;

	VO_CODECBUFFER					m_inBuffer;
	VO_CODECBUFFER					m_outBuffer;

	VO_U32							m_nStepSize;
	VO_S64							m_nOutputSize;
	VO_BOOL							m_bNofityError;

protected:
	OMX_U16						    m_Header[4];  //passthrough header
	OMX_BOOL						m_isAssbBuffer;	
	OMX_U32							m_nFrameSize;
	OMX_U32							m_nReadOffset;
	OMX_U32							m_nInputNum;
	OMX_U32							m_nFillSampleNum;
	OMX_U32							m_nPassthrDTSLen;

	OMX_BOOL						m_bPass;
	OMX_U32							m_nRoute;

	VO_CODECBUFFER                  m_assbBuffer;
	OMX_U8*							m_pAccumBuffer;
	OMX_U32							m_nAccumLength;
};

#endif //__voCOMXAudioPassthr_H__
