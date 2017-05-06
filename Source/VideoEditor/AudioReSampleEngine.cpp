/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		CAudioReSampleEngine.cpp
Written by:   Leon Huang

*******************************************************************************/
#include "AudioReSampleEngine.h"
#include "voLog.h"
#include "cmnMemory.h"
#define  BUFFER_SIZE 1024* 100
CAudioReSampleEngine::CAudioReSampleEngine ()
{
	//LoadDll ();
	//m_ImgBuffer.ColorType = VO_COLOR_YUV_PLANAR420;
	m_sourceImgBuffer.pBuffer = NULL;
	m_curImgBuffer.pBuffer = NULL;
	m_tmpImgBuffer.pBuffer = NULL;

	m_OutData.Buffer =new VO_BYTE[BUFFER_SIZE];
	m_OutData.Length = BUFFER_SIZE;
}

CAudioReSampleEngine::~CAudioReSampleEngine ()
{
	Uninit();
	RelaeseBuffer();
}
VO_U32 CAudioReSampleEngine::Init ()
{
	VO_U32 nRC = VO_ERR_NOT_IMPLEMENT;

	if (LoadLib (NULL) == 0)
	{
		VOLOGE ("LoadLib was failed!");
		return VO_ERR_FAILED;
	}

	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);
	nRC = m_AudioResampleAPI.Init(&m_pResHandle, VO_AUDIO_CodingUnused, &useData);
	CHECK_FAIL(nRC);

	return nRC;
}
VO_VOID CAudioReSampleEngine::RelaeseBuffer()
{
	delete []m_OutData.Buffer;
	m_OutData.Buffer = NULL;
}
VO_U32 CAudioReSampleEngine::Uninit (void)
{
	VOLOGF ();
	
	//RelaeseBuffer();

// 	if (m_pResHandle != NULL)
// 	{
// 		m_AudioResampleAPI.Uninit (m_pResHandle);
// 		m_pResHandle = NULL;
// 	}
	FreeLib ();
	return 0;
}


VO_U32 CAudioReSampleEngine::Process(VOMP_BUFFERTYPE *inputData )
{
	VOLOGI("+ Process. SampleSize:%d",inputData->nSize);
	VO_U32 nRc = VO_ERR_NONE;
	if (m_pResHandle == NULL|| inputData->nSize <=0)
		return VOMP_ERR_Pointer;

	VO_CODECBUFFER inData;
	inData.Buffer = inputData->pBuffer;
	inData.Length = inputData->nSize;
	inData.Time = inputData->llTime;
	nRc = m_AudioResampleAPI.SetInputData(m_pResHandle,&inData);
	m_OutData.Length = BUFFER_SIZE;
	nRc = m_AudioResampleAPI.GetOutputData(m_pResHandle,&m_OutData, &m_OutInfo);
	inputData->nSize = m_OutData.Length;
	memcpy(inputData->pBuffer, m_OutData.Buffer, m_OutData.Length);
	VOLOGI("- Process");
	return nRc;
}

int CAudioReSampleEngine::GetParam (int nID, void * pValue)
{
	VO_U32 nRc = VOEDT_ERR_NONE;
	switch(nID)
	{
	case VOEDT_PID_AUDIO_FORMAT:
		{
			break;
		}
	default:
		nRc = VOEDT_ERR_PID_NOTFOUND;
		break;
	}
	return nRc;
}

int CAudioReSampleEngine::SetParam (int nID, void * pValue)
{

	VO_U32 nRc = VOEDT_ERR_NONE;
	switch(nID)
	{
	case VOEDT_PID_PLAYER_AUDIO_FORMAT_TARGENT:
		{
			//Now the sample bits only support 16bit
			VOEDT_AUDIO_FORMAT *af = (VOEDT_AUDIO_FORMAT *)pValue;
			nRc = m_AudioResampleAPI.SetParam(m_pResHandle, VO_PID_RESAMPLE_OUTRATE, &af->SampleRate);
			nRc |=m_AudioResampleAPI.SetParam(m_pResHandle, VO_PID_RESAMPLE_OUTCHAN, &af->Channels);   //downmix channel.
			VOLOGI("nRc: 0x%08x, ResampleOut:%d, %d, %d", nRc, af->Channels, af->SampleBits, af->SampleRate)
			return nRc;
		}
	case VOEDT_PID_PLAYER_AUDIO_FORMAT_ORIGINAL:
		{
			
			VOEDT_AUDIO_FORMAT *af = (VOEDT_AUDIO_FORMAT *)pValue;
			nRc =m_AudioResampleAPI.SetParam(m_pResHandle, VO_PID_RESAMPLE_INRATE, &af->SampleRate);	
			VO_RESAMPLE_LEVEL level = (VO_RESAMPLE_LEVEL)2;
			nRc |=m_AudioResampleAPI.SetParam(m_pResHandle, VO_PID_RESAMPLE_LEVEL, &level);
			nRc |=m_AudioResampleAPI.SetParam(m_pResHandle, VO_PID_RESAMPLE_INCHAN, &af->Channels);   //downmix channel.
			VOLOGI("nRc: 0x%08x, ResampleIn:%d, %d, %d", nRc, af->Channels, af->SampleBits, af->SampleRate)
			return nRc;
		}
	}
	return m_AudioResampleAPI.SetParam (m_pResHandle, nID, pValue);
}


VO_U32 CAudioReSampleEngine::LoadLib (VO_HANDLE hInst)
{
	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;
	
	vostrcpy (m_szDllFile, _T("voResample"));
	vostrcpy (m_szAPIName, _T("voGetResampleAPI"));

#if defined _WIN32
	if (pDllFile != NULL)
	{
		memset (m_szDllFile, 0, sizeof (m_szDllFile));
		MultiByteToWideChar (CP_ACP, 0, pDllFile, -1, m_szDllFile, sizeof (m_szDllFile));
	}
	vostrcat (m_szDllFile, _T(".Dll"));

	if (pApiName != NULL)
	{
		memset (m_szAPIName, 0, sizeof (m_szAPIName));
		MultiByteToWideChar (CP_ACP, 0, pApiName, -1, m_szAPIName, sizeof (m_szAPIName));
	}
#elif defined LINUX
	if (pDllFile != NULL)
		vostrcpy (m_szDllFile, pDllFile);
	vostrcat (m_szDllFile, _T(".so"));

	if (pApiName != NULL)
		vostrcpy (m_szAPIName, pApiName);
#endif

	if (CDllLoad::LoadLib (hInst) == 0)
	{
		VOLOGE ("CDllLoad::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}

	VOGETRESAMPLEAPI pAPI = (VOGETRESAMPLEAPI) m_pAPIEntry;
	pAPI (&m_AudioResampleAPI);


	return 1;
}
