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
#include "CVideoEditorEngine.h"

#define LOG_TAG "CVideoEditorEngine"
#include "voLog.h"

//#define _DUMP

CVideoEditorEngine::CVideoEditorEngine()
	:m_vompCurPlayer ( NULL)
	,m_nIndex(-1)
	,m_bExitEDITOR(false)
	,out_file_encoder(NULL)
	,out_file_pcm(NULL)
	,out_file_yuv(NULL)
	,out_file_ARGB32(NULL)
	,m_sampleTime_video(0)
	,m_sampleTime_audio(0)
	,m_nVideoSample_nums(0)
#ifndef _MULTITHREAD
	,m_hThreadListPlay(NULL)
	,m_bPlayListLoopOK(VO_TRUE)
#else
	,m_bPlayListLoopOK_Video(VO_TRUE)
	,m_bPlayListLoopOK_Audio(VO_TRUE)
	,m_hThreadListPlay_Audio(NULL)
	,m_hThreadListPlay_Video(NULL)
	,m_hThreadSystemTimeLoop(NULL)
#endif
	,m_pCallbackFunc(NULL)
	,m_bPlayerEditor(VO_FALSE)
	,m_nActionMode(0)
	,m_nAudioSize_count(0)
	,m_bSetAudioOutputParam(VO_FALSE)
	,m_bSetVideoOutputParam(VO_FALSE)
	,m_bSetSinkOutputParam(VO_FALSE)
	,m_nStartPosition(0)
	,m_pLibOp(NULL)
	,m_nFrameRate(100)
{


	VOEDT_VIDEO_FORMAT tf;
	tf.ColorType = VO_COLOR_YUV_PLANAR420;
	tf.nHeight = 120;
	tf.nWidth = 160;
	//SetThumbNailFormat(&tf);
	
	m_sVideoFormat.ColorType = VO_COLOR_ARGB32_PACKED;
	m_sVideoFormat.nHeight = 480;
	m_sVideoFormat.nWidth = 720;

	m_TrimCtrl = new CTrimCtrl(this,(VOEDTENCODERCALLBACKPROC)CVideoEditorEngine::EncoderCallback);

#ifdef _USE_YUV
	m_Transfer_buffer.Buffer[0] = NULL;
	m_video_encoderbuffer.Buffer[0] = NULL;
	m_ThumbnailBuffer.pBuffer = NULL;
#endif

	ReSetBuffer();

	memset(&m_outputParam, 0, sizeof(m_outputParam));
	memset(&m_outputParam.VideoParam, 0, sizeof(m_outputParam.VideoParam));

	m_videoOutputBuffer.pBuffer = NULL;

	memset(m_cWorkingPath,0x00,sizeof(m_cWorkingPath));
}
void CVideoEditorEngine::InitVideoEditorEngine(VOEDT_INIT_INFO *pParam)
{	

	m_pUserData = pParam->pUserData;
	m_pCallbackFunc = pParam->pCallback;
	m_pLibOp =(VO_LIB_OPERATOR* ) pParam->pLibOP;
	VOLOGI("Lib OP: 0x%08x", m_pLibOp);
	memset(m_cWorkingPath,0x00,sizeof(m_cWorkingPath));
#ifdef _WIN32
	MultiByteToWideChar (CP_ACP, 0, (VO_CHAR*)pParam->pWorkingPath, -1, m_cWorkingPath, sizeof (m_cWorkingPath));
#else
	memcpy(m_cWorkingPath, (VO_CHAR*)pParam->pWorkingPath, sizeof(m_cWorkingPath));
#endif
	VOLOGI("WorkingPath:%s",m_cWorkingPath);
	
	m_encoderCtrl.SetParam(VOEDT_PID_PLAYER_LIB_OPERATOR, m_pLibOp);
	m_EffectionCtrl.SetLibOp(m_pLibOp);

}

CVideoEditorEngine::~CVideoEditorEngine(void)
{
	m_bExitEDITOR = true;
	m_bStopPlay = true;
#ifndef _MULTITHREAD
	while(!m_bPlayListLoopOK){voOS_Sleep(10);}
#else
	while(!m_bPlayListLoopOK_Video){voOS_Sleep(10);}
	while(!m_bPlayListLoopOK_Audio){voOS_Sleep(10);}
#endif
	ReleaseList(m_listCPlayer_Base);
	
	voOS_Sleep( 100);

	UnloadAllCtrl();
	ReSetBuffer();
	
}

void CVideoEditorEngine::ReleaseList(CObjectList<CPlayerBaseCtrl> &listCPlayer)
{	
	POSITION pos = listCPlayer.GetHeadPosition ();
	while (pos != NULL)
	{
		CPlayerBaseCtrl *tmp = listCPlayer.GetNext (pos);
		tmp->Stop();
		delete tmp;
	}
	listCPlayer.RemoveAll();
}
VO_S32 CVideoEditorEngine::CallBackFunc(VO_S32 nPid, VOEDT_CALLBACK_BUFFER *callback)
{
	if(nPid == VOEDT_CB_EDITOR_POSITION || nPid == VOEDT_CB_CLIP_POSITION) 
	{
		VOLOGI("CallBack Information : ID 0x%08x,Position:%d", nPid,*(int*)callback->pData);
	}
	else
	{
		VOLOGI("CallBack Information : ID 0x%08x", nPid);
	}
	callback->pUserData = m_pUserData;
	if(m_pCallbackFunc)
		m_pCallbackFunc(nPid, callback);
	
	return VOEDT_ERR_NONE;
}
CClipPlayer* CVideoEditorEngine::CreateAClip(void* pSource,VO_U32 sourceType)
{

	CClipPlayer *vompPlayer= new CClipPlayer ( m_pLibOp);

	VO_S32 nRc = vompPlayer->SetDataSource ((void *)pSource, 0, 3,sourceType);
	if(nRc != VOEDT_ERR_NONE) {delete vompPlayer; return NULL;}
	nRc = vompPlayer->InitTrimCtrl(this, (VOEDTENCODERCALLBACKPROC)CVideoEditorEngine::EncoderCallback);

	return vompPlayer;
}

VO_S32 CVideoEditorEngine::AddImgClip(VO_S32 nPid, VO_VOID* pSource,VO_U32 nFlag,VOEDT_CLIP_HANDLE **ppOutClip /*= NULL*/)
{
	VO_S32 nRc = VOEDT_ERR_FAILED;
	CImgPlayer *player = new CImgPlayer(m_pLibOp);
	nRc = player->SetDataSource(pSource,nFlag);
	m_listCPlayer_Base.AddTail((CPlayerBaseCtrl*)player);

	VOEDT_CALLBACK_BUFFER edt;
	nPid = VOEDT_CB_ADD_IMG;
	edt.pData = (void*)player;
	edt.pClipHandle = (VOEDT_CLIP_HANDLE*)player;//->GetClipInfo();
	CallBackFunc(nPid,&edt);

	VO_U32 value = 1;
	SetClipParam((VOEDT_CLIP_HANDLE*)player,VOEDT_PID_DROP_BLACK_FRAME,&value);
	GetClipThumbNail(edt.pClipHandle  ,0);

	GetClipDuration((VOEDT_CLIP_HANDLE*)player, NULL);
	/*return editor duration*/
	GetEditorDuration(NULL);
	if(ppOutClip) *ppOutClip = (VOEDT_CLIP_HANDLE*)player;
	return nRc ;
}
VO_S32 CVideoEditorEngine::AddTextClip(VO_S32 nPid, VO_VOID* pSource,VO_U32 nFlag,VOEDT_CLIP_HANDLE **ppOutClip)
{
	VO_S32 nRc = VOEDT_ERR_NONE;
	CTextPlayer *player = new CTextPlayer(m_pLibOp);
	nRc = player->SetDataSource(pSource,nFlag);
	m_listCPlayer_Base.AddTail((CPlayerBaseCtrl*)player);

	VOEDT_CALLBACK_BUFFER edt;
	nPid = VOEDT_CB_ADD_TEXT;
	edt.pData = (void*)player;
	edt.pClipHandle = (VOEDT_CLIP_HANDLE*)player;//->GetClipInfo();
	CallBackFunc(nPid,&edt);
	

	if(ppOutClip) *ppOutClip = (VOEDT_CLIP_HANDLE*)player;
	return nRc ;
}

VO_S32 CVideoEditorEngine::AddMediaClip(VO_S32 nPid, VO_VOID* pSource,VO_U32 nFlag,VOEDT_CLIP_HANDLE **pOutClip)
{

	CClipPlayer *player = CreateAClip(pSource, nPid);
	if(player == NULL )
	{
		VOLOGE("AddMediaClip %s Fail",(VO_CHAR*)pSource);
		return VOEDT_ERR_OPEN_FAIL;
	}
// 	if(player->m_nClipType & VOEDT_PID_CLIP_TYPE_VIDEO)
// 		player->SetClipParam(VOEDT_PID_PLAYER_OUTPUT_VIDEO_FORMAT, &m_sVideoFormat);

	VO_S32 index = -1;
	VOEDT_CLIP_PARAM * clip = NULL;
	//player->GetClipParam(VOEDT_PID_PLAYER_CLIP_PARAM,&clip);
	GetClipParam((VOEDT_CLIP_HANDLE*)player,VOEDT_PID_PLAYER_CLIP_PARAM,&clip);
	CPlayerBaseCtrl *pxp = (CPlayerBaseCtrl*)player;
	m_listCPlayer_Base.AddTail(pxp);
	ReSortEditor(m_listCPlayer_Base);
	VOEDT_CALLBACK_BUFFER edt;

		
	VO_S32 position = 0;
	nPid & VOEDT_PID_CLIP_TYPE_VIDEO ? nPid = VOEDT_CB_ADD_VIDEO : nPid = VOEDT_CB_ADD_AUDIO;

	edt.pData = (void*)player;
	edt.pClipHandle = (VOEDT_CLIP_HANDLE*)player;//->GetClipInfo();
	CallBackFunc(nPid,&edt);

	VO_U32 value = 1;
	SetClipParam((VOEDT_CLIP_HANDLE*)player,VOEDT_PID_DROP_BLACK_FRAME,&value);
	GetClipThumbNail(edt.pClipHandle  ,0);

	GetClipDuration((VOEDT_CLIP_HANDLE*)player, NULL);
	/*return editor duration*/
	GetEditorDuration(NULL);
	
	if(pOutClip)  *pOutClip = (VOEDT_CLIP_HANDLE*)player;
	return VOEDT_ERR_NONE;
}
VO_S32 CVideoEditorEngine::GetOutputParam(void* pValue)
{
	memcpy((VOEDT_OUTPUT_PARAM*)pValue,&m_outputParam,sizeof(VOEDT_OUTPUT_PARAM ));
	return VOEDT_ERR_NONE;
	
}


VO_S32 CVideoEditorEngine::GetAClipPosition(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *pPos)
{
	CPlayerBaseCtrl* pPlayer = (CPlayerBaseCtrl*)pClipHandle;
	VO_S32 nRC = pPlayer->GetPosition(pPos);
	return nRC;
}

VO_S32 CVideoEditorEngine::GetEditorDurationTime(VO_U32 *pDuration)
{
	*pDuration = 0;
	CPlayerBaseCtrl *player = NULL;
	VOEDT_CLIP_PARAM clip ;
	CObjectList<CPlayerBaseCtrl> *listCPlayer = &m_listCPlayer_Base;
	POSITION pos = listCPlayer->GetHeadPosition ();
	VO_S32 n = 1;
	while (pos != NULL)
	{
		player = listCPlayer->GetNext (pos);
		if(player->m_nClipType == VOEDT_PID_CLIP_TYPE_TEXT)
			continue;

		VOEDT_CLIP_PARAM *pParam = NULL;
		GetClipParam((VOEDT_CLIP_HANDLE*)player,VOEDT_PID_PLAYER_CLIP_PARAM,&pParam);
		if(pParam->nPlayMode & VOEDT_PID_PLAYMODE_VIDEO)
		{
			*pDuration += pParam->point_out - pParam->point_in ;

			VOLOGI("play duration: %d", pParam->point_out - pParam->point_in);
			if(pParam->videoTransition && n!= 1)
				*pDuration -= pParam->videoTransition->nDuration;
			n++;
		}
	}
	VOLOGI("Duration:%d",*pDuration);
	return VOEDT_ERR_NONE;
}

VO_S32 CVideoEditorEngine::GetAClipDurationTime(VOEDT_CLIP_HANDLE *player,VO_U32 *pPos)
{
	if(!player) return VOEDT_ERR_CLIP_NOTFOUND;
	((CPlayerBaseCtrl*)player)->GetDuration(pPos);
	return VOEDT_ERR_NONE;
}

VO_S32	CVideoEditorEngine::StopEditor()
{
	m_bStopPlay = true;
	m_nActionMode = 0;
	m_bPlayerEditor = VO_FALSE;
	m_nStartPosition = 0;
	return VOEDT_ERR_NONE;
}
void CVideoEditorEngine::CheckAndInitList()
{
//	CClipPlayer *player = NULL;
//	POSITION pos = m_listCPlayer_Base.GetHeadPosition();
//	VO_S32	timeline = 0;
//	while(pos!= NULL)
//	{
//		player = m_listCPlayer_Base.GetNext(pos);
//		if(player) 
//		{	
//			VOEDT_CLIP_PARAM *clip = &player->m_sVideoEditorClip;
//			if(clip->beginAtTimeline == -1)
//			{	
//				clip->beginAtTimeline = timeline;
//			
//			}
//			timeline += clip->point_out - clip->point_in;
//			if(clip->videoTransition )
//				timeline -= clip->videoTransition->nDuration;
//			//now the clip's target Height & Width equal to Output's Height and Width;
//			//PosX & PosY are set to zero
//			/* these will be supported in the future*/
//			/*clip->targetVFormat.nHeight = m_outputParam.VideoParam.nHeight;
//			clip->targetVFormat.nWidth = m_outputParam.VideoParam.nWidth;
//			clip->targetVFormat.nLeft = 0;
//			clip->targetVFormat.nTop = 0;*/
//
//		}
//	}
//
}
VO_S32  CVideoEditorEngine::PlayEditor()
{
	VOLOGI("in PlayEditor. PlayMode :0x%08x", m_nActionMode);
	VO_U32 start = voOS_GetSysTime();

	VO_S32 nRc ;

	ReSetBuffer();

	VO_AUDIO_FORMAT af;
	VO_VIDEO_FORMAT vf;
	af.Channels = m_outputParam.AudioParam.nChannels;
	af.SampleBits = m_outputParam.AudioParam.nSampleBits;
	af.SampleRate = m_outputParam.AudioParam.nSampleRate;
	vf.Height =  m_outputParam.VideoParam.nHeight;
	vf.Width  =  m_outputParam.VideoParam.nWidth;
	if(m_nActionMode & VOEDT_ACTIONMODE_TRANSCODE)
	{
		VOLOGI("1");

		if(!(m_bSetVideoOutputParam && m_bSetAudioOutputParam&& m_bSetSinkOutputParam))
			return VOEDT_ERR_NOT_SETEDITORPARAM;
		m_nActionMode |= VOEDT_ACTIONMODE_TRIM;

		VOLOGI("2");
		CPlayerBaseCtrl *(pPlayer[VOEDT_MAX_CLIP_COUNT]);
		VO_S32 num = 0;
		nRc = FindEditor(pPlayer,&num,VOEDT_PID_CLIP_TYPE_VIDEO);
		for(VO_S32 i = 0;i< num; ++i)
		{	
			VOEDT_VIDEO_FORMAT vft;
			pPlayer[i]->GetClipParam(VOEDT_PID_VIDEO_FORMAT, &vft);
			if(vft.nHeight != vf.Height || vft.nWidth != vf.Width) m_nActionMode &= ~VOEDT_ACTIONMODE_TRIM;
			VO_S32 codec;
			pPlayer[i]->GetClipParam(VOEDT_PID_VIDEO_CODEC, &codec);
			if((VOEDT_VIDEO_CODINGTYPE)codec != m_outputParam.VideoParam.nCodecType) m_nActionMode &= ~VOEDT_ACTIONMODE_TRIM;
		}
		VOLOGI("3");
		num = 0;
		nRc = FindEditor(pPlayer,&num,VOEDT_PID_CLIP_TYPE_AUDIO);
		for(VO_S32 i = 0;i< num; ++i)
		{	
			VOEDT_AUDIO_FORMAT aft;
			pPlayer[i]->GetClipParam(VOEDT_PID_AUDIO_FORMAT, &aft);
			if(aft.Channels != af.Channels || aft.SampleBits != af.SampleBits || aft.SampleRate != af.SampleRate) 
			{
					m_nActionMode &= ~VOEDT_ACTIONMODE_TRIM;
			}//m_nActionMode &= ~VOEDT_ACTIONMODE_TRIM;
			VO_S32 codec;
			pPlayer[i]->GetClipParam(VOEDT_PID_AUDIO_CODEC, &codec);
			if((VOEDT_AUDIO_CODINGTYPE)codec != m_outputParam.AudioParam.nCodecType) m_nActionMode &= ~VOEDT_ACTIONMODE_TRIM;
			VOLOGI("4");
		}
	}	

	m_pVideoFormat_PresentSelected = &m_sVideoFormat;

	nRc = RefreshClipsVideoFormat(m_pVideoFormat_PresentSelected);
	CHECK_FAIL(nRc);
	nRc = RefreshClipsAudioFormat(&m_sAudioFormat);
	CHECK_FAIL(nRc);

	ResetEachClip();

	ReSortEditor(m_listCPlayer_Base);
	m_nVideoSample_nums = 0;
	m_nAudioSize_count = 0;

	m_bPlayerEditor = VO_TRUE;
	
	m_bStopPlay = false;
	VO_U32 nID;
	m_nSyncSystemTime_Start = voOS_GetSysTime();
	m_sampleTime_audio = m_nStartPosition;
	m_sampleTime_video = m_nStartPosition;

	VOLOGI("Start position:audio:%d video:%d", m_sampleTime_audio, m_sampleTime_video);
	if(m_nStartPosition != 0)	SeekEditor(m_nStartPosition);
#ifndef _MULTITHREAD
	if(m_hThreadListPlay ) voThreadClose(m_hThreadListPlay,0);
	nRc = voThreadCreate (&m_hThreadListPlay, &nID, PlayListProc, this, 3);
#else
	if(m_hThreadListPlay_Video ) voThreadClose(m_hThreadListPlay_Video,0);
	if(m_hThreadListPlay_Audio ) voThreadClose(m_hThreadListPlay_Audio,0);
	if(m_hThreadSystemTimeLoop ) voThreadClose(m_hThreadSystemTimeLoop,0);

 	if(m_nActionMode & VOEDT_ACTIONMODE_TRANSCODE || m_nActionMode & VOEDT_ACTIONMODE_PREVIEW_AUDIO)
 	{
		nRc = voThreadCreate (&m_hThreadListPlay_Audio, &nID, PlayListProc_Audio, this, 0);
		m_syncTime = &m_sampleTime_audio;
 	}
 	else{
 		//nRc = voThreadCreate (&m_hThreadSystemTimeLoop, &nID, PlaySyncProc_SystemTime, this, 0);
 		m_syncTime = &(m_nSyncSystemTime = m_nStartPosition);
	}
	if(m_nActionMode & VOEDT_ACTIONMODE_TRANSCODE || m_nActionMode & VOEDT_ACTIONMODE_PREVIEW_VIDEO)
		nRc = voThreadCreate (&m_hThreadListPlay_Video, &nID, PlayListProc_Video, this, 0);
#endif

	VOLOGI("out PlayEditor");
	VO_U32 end = voOS_GetSysTime();		
	VOLOGE("PlayEditor Operation Cost Time ! %f Seconds! \n" , ( end - start ) / 1000.);

	return VOEDT_ERR_NONE;
}

VO_VOID CVideoEditorEngine::ReSetBuffer()
{
#ifdef _USE_YUV
	if(m_Transfer_buffer.Buffer[0] ){ delete []m_Transfer_buffer.Buffer[0];m_Transfer_buffer.Buffer[0] = NULL;}
	if(m_video_encoderbuffer.Buffer[0] ){ delete []m_video_encoderbuffer.Buffer[0];m_video_encoderbuffer.Buffer[0] = NULL;}
#endif
}

void CVideoEditorEngine::ResetEachClip()
{
	POSITION pos = m_listCPlayer_Base.GetHeadPosition();
	while(pos )
	{
		CPlayerBaseCtrl *player = m_listCPlayer_Base.GetNext(pos);
 		if(player->m_nClipType != VOEDT_PID_CLIP_TYPE_TEXT)
 		{
			VOEDT_CLIP_PARAM * clip ;
			GetClipParam((VOEDT_CLIP_HANDLE*)player,VOEDT_PID_PLAYER_CLIP_PARAM,&clip);
			VO_U32 time = (clip->point_in);
			player->Flush();
			VOMP_BUFFERTYPE *pOutBuffer;
			player->ResetParam();
			player->Run();
			if(player->m_nClipType & VOEDT_PID_CLIP_TYPE_VIDEO)
				player->SetPosition(&pOutBuffer,&time, VOEDT_PID_VIDEO_SAMPLE);
			else
				player->SetPosition(&pOutBuffer,&time, VOEDT_PID_AUDIO_SAMPLE);

		}
	}
}

void CVideoEditorEngine::UnloadAllCtrl()
{
	if(!m_bPlayerEditor) return;

	m_bPlayerEditor = VO_FALSE;

	m_nStartPosition = 0;
	m_nActionMode = 0;

	m_encoderCtrl.UnInit(1);
	m_encoderCtrl.UnInit(0);
	m_FileSinkCtrl.Stop();
	
	m_bSetSinkOutputParam = VO_FALSE;
	m_bSetVideoOutputParam = VO_FALSE;
	m_bSetAudioOutputParam = VO_FALSE;

	if(out_file_encoder)fclose(out_file_encoder);
	out_file_encoder = NULL;
	if(out_file_pcm)fclose(out_file_pcm);
	out_file_pcm = NULL;
	if(out_file_yuv)fclose(out_file_yuv);
	out_file_yuv = NULL;
	if(out_file_ARGB32)fclose(out_file_ARGB32);
	out_file_ARGB32 = NULL;
}

#ifndef _MULTITHREAD
VO_S32 CVideoEditorEngine::DoLoop()
{
	CClipPlayer *vompPlayer = NULL;

	VOMP_BUFFERTYPE *pVideoBuf = NULL;
	VO_S32 editorTime = 0;	
	VO_U32  *time_v, *time_a,time_diff;
	m_sampleTime_video = 0;
	m_sampleTime_audio = 0;
	m_nVideoSample_nums = 0;
	m_nAudioSize_count =0;	
	time_v = &m_sampleTime_video;//&m_video_outputbuffer->a_sample->pBuffer->llTime;
	time_a = &m_sampleTime_audio;//&m_audio_outputbuffer->a_sample->pBuffer->llTime;

	m_syncTime = &m_sampleTime_audio;

	bool allover = false;

	VO_U32 duration = 0;
	GetEditorDurationTime(&duration);
	VOLOGI("Start Play Editor. Duration:%d SYNC:%d",duration,*m_syncTime);
	while (!m_bStopPlay && *m_syncTime < duration )
	{				

		VO_U32 start = voOS_GetSysTime();

		VOLOGI("SYNC Time %d %d",*m_syncTime,*time_v);
		if(*time_v < *m_syncTime)
		{
			//	m_TrimCtrl->GetSample(VOEDT_PID_VIDEO_SAMPLE);
			SetVideoPosition(&pVideoBuf,m_syncTime);
		}
		else
		{
			//*m_syncTime+=100;//
			SetAudioPosition(m_syncTime);
		}

		GetEditorPosition(NULL);

		voOS_Sleep(1);

		VO_U32 end = voOS_GetSysTime();
		VOLOGE("Operate one sample! Cost %f Seconds!\n" , ( end - start ) / 1000.);
	}

	UnloadAllCtrl();
	ReSetBuffer();	
	VOLOGI("Endof Play Editor. SYNC:%d",*m_syncTime);
	return 0;
}
#else
VO_S32 CVideoEditorEngine::DoLoop_Audio()
{
	VOLOGI("+DoLoop_Audio");

	voThreadSetPriority(m_hThreadListPlay_Audio,VO_THREAD_PRIORITY_ABOVE_NORMAL );

	CClipPlayer *vompPlayer = NULL;

	VOMP_BUFFERTYPE *pAudioBuf = NULL;
	VO_S32 editorTime = 0;	
	VO_U32  *time_v, *time_a,time_diff;

	m_nVideoSample_nums = 0;
	m_nAudioSize_count =0;	
	time_v = &m_sampleTime_video;//&m_video_outputbuffer->a_sample->pBuffer->llTime;
	time_a = &m_sampleTime_audio;//&m_audio_outputbuffer->a_sample->pBuffer->llTime;

	bool allover = false;

	VO_U32 duration = 0;
	GetEditorDurationTime(&duration);
	VOLOGI("Start Play Editor. Duration:%d SYNC:%d",duration,*m_syncTime);
	while (!m_bStopPlay && *m_syncTime < duration )
	{				
		
 		if(*time_v < *m_syncTime)
 		{
			;
 		}
		else
		{
			VOLOGI("SYNC Time %d %d",*m_syncTime,*time_v);	
			VO_U32 start = voOS_GetSysTime();
			SetAudioPosition(&pAudioBuf, (VO_U32*)m_syncTime);
			VO_U32 end = voOS_GetSysTime();
			VOLOGE("Operate one AudioSample! Cost %f Seconds! SyncTime:%d\n" , ( end - start ) / 1000., *m_syncTime);
			GetEditorPosition(NULL);
		}
		
		voOS_Sleep(1);

	}
	
	VOLOGI("Endof Play Editor. SYNC:%llu",*m_syncTime);
	while(!m_bPlayListLoopOK_Video){voOS_Sleep(10);}
	

	VOLOGI("-DoLoop_Audio");
	return VOEDT_ERR_NONE;
}
VO_S32 CVideoEditorEngine::DoLoop_Video()
{
	VOLOGI("+DoLoop_Video");
	voThreadSetPriority(m_hThreadListPlay_Video,VO_THREAD_PRIORITY_ABOVE_NORMAL );

	CClipPlayer *vompPlayer = NULL;

	VOMP_BUFFERTYPE *pVideoBuf = NULL;
	VO_S32 editorTime = 0;	
	VO_U32  *time_v, *time_a,time_diff;

	m_nVideoSample_nums = 0;
	m_nAudioSize_count =0;	
	time_v = &m_sampleTime_video;//&m_video_outputbuffer->a_sample->pBuffer->llTime;
	time_a = &m_sampleTime_audio;//&m_audio_outputbuffer->a_sample->pBuffer->llTime;

	bool allover = false;

	VO_U32 duration = 0;
	GetEditorDurationTime(&duration);
	VOLOGI("Start Play Editor. Duration:%d SYNC:%d",duration,*m_syncTime);
	while (!m_bStopPlay && *m_syncTime < duration )
	{				
		
		if(*time_v < *m_syncTime)
		{
			VO_U32 start = voOS_GetSysTime();

			SetVideoPosition(&pVideoBuf, time_v);

			VO_U32 end = voOS_GetSysTime();		

			VOLOGE("Operate one VideoSample! Cost %f Seconds! SampleTime:%d, SyncTime:%d\n" , ( end - start ) / 1000., *time_v, *m_syncTime);
		}
		m_nSyncSystemTime = voOS_GetSysTime() - m_nSyncSystemTime_Start + m_nStartPosition;
		//VOLOGI("Cur SystemSyncTime:%d",m_nSyncSystemTime);
		
		//GetEditorPosition(NULL);
		
		voOS_Sleep(1);

	
	}
	


	VOLOGI("-DoLoop_Video");
	return 0;
}
#endif

VO_U32 CVideoEditorEngine::PlaySyncProc_SystemTime (VO_PTR pParam)
{
	CVideoEditorEngine * pPlayer = (CVideoEditorEngine *) pParam;
	return pPlayer->PlaySyncLoop_SystemTime ();
}
VO_S32 CVideoEditorEngine::PlaySyncLoop_SystemTime()
{
	while(!m_bStopPlay && m_bPlayerEditor)
	{
		m_nSyncSystemTime = voOS_GetSysTime() - m_nSyncSystemTime;
		voOS_Sleep(10);
	}
	m_hThreadSystemTimeLoop  = NULL;
	return 0;
}
#ifndef _MULTITHREAD
VO_U32 CVideoEditorEngine::PlayListProc (VO_PTR pParam)
{
	CVideoEditorEngine * pPlayer = (CVideoEditorEngine *) pParam;
	
	return pPlayer->PlayListLoop ();
}
VO_S32 CVideoEditorEngine::PlayListLoop(void)
{
	m_bPlayListLoopOK = VO_FALSE;
	ResetEachClip();

	VO_U32 start = voOS_GetSysTime();
	DoLoop();	
	VO_U32 end = voOS_GetSysTime();
	VOLOGE("Play Editor End! Cost %f Seconds!\n" , ( end - start ) / 1000.);

	ResetEachClip();

	VOEDT_CALLBACK_BUFFER edt;
	VO_S32 nPid = VOEDT_CB_TRANSCODE_SUCCESS;
	edt.pClipHandle = NULL;
	edt.pData = NULL;
	CallBackFunc(nPid,&edt);

	m_bPlayListLoopOK = VO_TRUE;
	m_bPlayerEditor = VO_FALSE;

	m_nActionMode = VOEDT_ACTIONMODE_PREVIEW_VIDEO;
	return 0;
}
#else
VO_U32 CVideoEditorEngine::PlayListProc_Audio(VO_PTR pParam)
{
	CVideoEditorEngine * pPlayer = (CVideoEditorEngine *) pParam;
	return pPlayer->PlayListLoop_Audio();
}
VO_U32 CVideoEditorEngine::PlayListProc_Video(VO_PTR pParam)
{
	CVideoEditorEngine * pPlayer = (CVideoEditorEngine *) pParam;

	return pPlayer->PlayListLoop_Video();
}
VO_S32 CVideoEditorEngine::PlayListLoop_Audio(void)
{
	m_bPlayListLoopOK_Audio = VO_FALSE;
	VO_U32 start = voOS_GetSysTime();
	DoLoop_Audio();	
	VO_U32 end = voOS_GetSysTime();
	VOLOGE("Play Audio End! Cost %f Seconds!\n" , ( end - start ) / 1000.);

	VOEDT_CALLBACK_BUFFER edt;
	VO_S32 nPid = VOEDT_CB_TRANSCODE_SUCCESS;
	edt.pClipHandle = NULL;
	edt.pData = NULL;
	CallBackFunc(nPid,&edt);

	m_bPlayListLoopOK_Audio = VO_TRUE;

	UnloadAllCtrl();

	return 0;
}

VO_S32 CVideoEditorEngine::PlayListLoop_Video(void)
{
	m_bPlayListLoopOK_Video = VO_FALSE;

	VO_U32 start = voOS_GetSysTime();
	DoLoop_Video();	
	VO_U32 end = voOS_GetSysTime();
	VOLOGE("Play Video End! Cost %f Seconds!\n" , ( end - start ) / 1000.);

	VOEDT_CALLBACK_BUFFER edt;
	VO_S32 nPid = VOEDT_CB_TRANSCODE_SUCCESS;
	edt.pClipHandle = NULL;
	edt.pData = NULL;
	CallBackFunc(nPid,&edt);

	m_bPlayListLoopOK_Video = VO_TRUE;

	UnloadAllCtrl();

	return 0;
}
#endif

VO_S32 CVideoEditorEngine::UpdateEDITOR()
{
	
	return 0;
}

void CVideoEditorEngine::EncoderCallback(VO_S32 nPid,VOEDT_PLAYER_CALLBACK_BUFFER *pValue)
{
	CVideoEditorEngine *player = (CVideoEditorEngine*)pValue->pUserData;
	if(nPid == VO_PID_SINK_EXTDATA)
		player->m_FileSinkCtrl.SetParam(VO_PID_SINK_EXTDATA, pValue->pData);
	else
	{
		VOEDT_ENCODER_SAMPLE *encSample = ((VOEDT_ENCODER_SAMPLE*)pValue->pData);
		if(encSample->nSampleType == 9 &&  player->out_file_encoder)
		{
		
			VO_U32 size  = encSample->buffer->Length&0xFFFFFFF;
				VOLOGI("dump video %d",size);
		//	player->m_nAudioSize_count+= encSample->buffer->Length;
			VO_U32 writed = fwrite(encSample->buffer->Buffer, 1,  size, player->out_file_encoder) ;
			if(writed != size)
				VOLOGE("dump file error %d!=  %d" ,writed,size);
			//fwrite(encSample->buffer->Buffer, 1, encSample->buffer->Length, player->out_file_encoder);
		}		
		player->m_FileSinkCtrl.AddASample((VOEDT_ENCODER_SAMPLE*)pValue->pData);

		
	}

	
//	if(encSample->nSampleType == 9 &&  player->out_file)
	//	fwrite(encSample->buffer->Buffer, 1, 640*480*3/2, player->out_file);
}

VO_S32 CVideoEditorEngine::GetEditorPosition(VO_U32 *position)
{
	if(!m_syncTime) return VOEDT_ERR_FAILED;
	VO_U32 pp = 0;
	pp =m_sampleTime_video;// *m_syncTime;

	if( position == NULL)	
	{	
		VOEDT_CALLBACK_BUFFER edt;
		edt.pData = (void*)&pp;
		edt.pClipHandle = NULL;
		CallBackFunc(VOEDT_CB_EDITOR_POSITION,&edt);
	}
	else
		*position = pp;
	return VOEDT_ERR_NONE;
}

VO_S32 CVideoEditorEngine::GetClipPosition(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *position)
{
	VOLOGI("in GetClipPosition :%d", position ? *position : 0);
	VO_S32 nRc ;
	VO_U32 pp = 0;
	nRc= GetAClipPosition(pClipHandle,&pp);

	CHECK_FAIL(nRc);
	VOLOGI("Cur Position = %d, ClipNumbers = %d",pp,m_nVideoSample_nums);
	if( position == NULL)	
	{	
		VOEDT_CALLBACK_BUFFER edt;
		edt.pData = (void*)&pp;
		edt.pClipHandle = pClipHandle;
		CallBackFunc(VOEDT_CB_CLIP_POSITION,&edt);
	}
	else
		*position = pp;

	return nRc;
}

VO_S32 CVideoEditorEngine::GetEditorDuration(VO_U32 *duration)
{
	VO_S32 nRc ;
	VO_U32 dd = 0;
	nRc = GetEditorDurationTime(&dd);
	CHECK_FAIL(nRc);
	if( duration == NULL)	
	{
		VOEDT_CALLBACK_BUFFER edt;
		edt.pData = (void*)&dd;
		edt.pClipHandle = NULL;
		CallBackFunc(VOEDT_CB_EDITOR_DURATION,&edt);
	}
	else
		*duration = dd;
	return nRc;
}
VO_S32 CVideoEditorEngine::GetClipDuration(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *duration)
{
	VO_S32 nRc ;

	VO_U32 dd =0;
	nRc = GetAClipDurationTime(pClipHandle,&dd);

	CHECK_FAIL(nRc);
	if( duration == NULL)	
	{	
		
		VOEDT_CALLBACK_BUFFER edt;
		edt.pData = (void*)(int*)(&dd);
		edt.pClipHandle = pClipHandle;
		CallBackFunc(VOEDT_CB_CLIP_DURATION,&edt);
		VOLOGI("clip duration :%d",*(int*)edt.pData);
	}
	else
		*duration = dd;

	return nRc;
}

/*this is a special condition ,return all nCliptype clip*/
 VO_S32 CVideoEditorEngine::FindEditor(CPlayerBaseCtrl **ppPlayer,VO_S32 *nNum,VO_S32 nCliptype)
{
	POSITION pos = m_listCPlayer_Base.GetHeadPosition ();
	VO_S32 i = 0;
	VO_S32 num = *nNum;
	*nNum = 0;
	VO_U32 cliptype ;
	while (pos != NULL)
	{	
		//VOEDT_CLIP_PARAM **pParam = &ppParam[i];// &(*(ppParam + i));
		//VO_S32 *pPos = *(ppPos+i);//(*(ppPos +i));
		ppPlayer[i]  = m_listCPlayer_Base.GetNext (pos);
	//	VOEDT_CLIP_PARAM *pParam = ppPlayer[i]->GetVideoEditorClip();
		cliptype = ppPlayer[i]->m_nClipType;
		// ppPlayer[i]->GetClipParam(VOEDT_PID_PLAYER_CLIP_TYPE,&cliptype);
		//to distinction Video has Audio  and only Audio
		if(nCliptype == VOEDT_PID_CLIP_TYPE_AUDIO && (VOEDT_PID_CLIP_TYPE_VIDEO & cliptype))	
			continue;
		if(nCliptype & cliptype)
		{
			i++;
			(*nNum) ++;			
			if(i == num) break;
		}
	}
	if( *nNum > 0) return VOEDT_ERR_NONE;
	return VOEDT_ERR_FAILED;
}
VO_S32 CVideoEditorEngine::FindEditorPosition(CPlayerBaseCtrl **ppPlayer,VO_S32 ppPos[],bool ppTransition[],VO_S32 nInPos,VO_S32 *nNum,VO_S32 nCliptype)
{
	VOLOGI("+FindEditorPosition. Pos:%d, Num:%d, ClipType:%d",nInPos, *nNum, nCliptype )
	POSITION pos = m_listCPlayer_Base.GetHeadPosition ();
	VO_S32 i = 0;
	VO_S32 num = *nNum;
	*nNum = 0;

	VO_U32 cliptype;

	while (pos != NULL)
	{	
		ppPlayer[i]  = m_listCPlayer_Base.GetNext (pos);
		VOEDT_CLIP_PARAM *pParam;
		GetClipParam((VOEDT_CLIP_HANDLE*)ppPlayer[i],VOEDT_PID_PLAYER_CLIP_PARAM,&pParam);
		if(nCliptype == VOEDT_PID_CLIP_TYPE_VIDEO ){
			if( !(pParam->nPlayMode & VOEDT_PID_PLAYMODE_VIDEO) )continue;
		}
		else if(nCliptype == VOEDT_PID_CLIP_TYPE_AUDIO ) {
			if( !(pParam->nPlayMode & VOEDT_PID_PLAYMODE_AUDIO) )continue;
		}

		ppTransition[i] = false;
		cliptype = ppPlayer[i]->m_nClipType;
		if((nCliptype & cliptype) && ((pParam->beginAtTimeline + (pParam->point_out- pParam->point_in)) >= nInPos && pParam->beginAtTimeline <= nInPos))
		{
			
			ppPos[i] = nInPos - pParam->beginAtTimeline ; 
			if(ppPos[i] >= 0
				&& (
				   (nCliptype == VOEDT_PID_CLIP_TYPE_VIDEO && pParam->videoTransition && ppPos[i] <= pParam->videoTransition->nDuration ) 
				|| (nCliptype == VOEDT_PID_CLIP_TYPE_AUDIO && pParam->audioTransition && ppPos[i] <= pParam->audioTransition->nDuration )
				)
				)
			{
				ppTransition[i] = true;
			}
			ppPos[i] += pParam->point_in;
			i++;
			(*nNum) ++;

			if(num == 1) break;
			
		}
	}
	VOLOGI("-FindEditorPosition. Pos:%d, Num:%d, ClipType:%d",nInPos, *nNum, nCliptype )

	if( *nNum > 0) return VOEDT_ERR_NONE;
	return VOEDT_ERR_FAILED;
}
#ifndef _USE_YUV
VO_S32 CVideoEditorEngine::DoVideo(CPlayerBaseCtrl* pPlayer,VOMP_BUFFERTYPE** ppOutBuffer,bool isTransition,VO_U32 position,VO_S32 *pReserve)
{
	VO_S32 nRc = VOEDT_ERR_NONE;

	VOEDT_CLIP_PARAM *pParam = NULL;
	GetClipParam((VOEDT_CLIP_HANDLE*)pPlayer,VOEDT_PID_PLAYER_CLIP_PARAM,&pParam);
	if(!pParam ) VOLOGI("GetClipParam VOEDT_PID_PLAYER_CLIP_PARAM Error.");
	VOMP_BUFFERTYPE *buffer;

	VOEDT_CLIP_POSITION_FORMAT vofout;
	/*Video Transfor*/
	VOEDT_CLIP_POSITION_FORMAT vofin;
	VOEDT_VIDEO_FORMAT vf;
	pPlayer->GetClipParam(VOEDT_PID_PLAYER_OUTPUT_VIDEO_FORMAT,&vf);
	VOLOGI("VOEDT_PID_PLAYER_OUTPUT_VIDEO_FORMAT. (%d, %d)",vf.nWidth, vf.nHeight);
	vofin.nHeight = vf.nHeight;
	vofin.nWidth = vf.nWidth;
	vofin.nLeft = 0;
	vofin.nTop = 0;

	VOMP_BUFFERTYPE *pTmpBuffer = NULL;

	VOLOGI("Wanted Sample Position:%d", position);
	if(!m_bPlayerEditor)
	{
		nRc = pPlayer->SetPosition(&pTmpBuffer,&position,VOEDT_PID_VIDEO_SAMPLE);
	}
	else
	{
		nRc	= pPlayer->GetSample(&pTmpBuffer,position,pReserve,VOEDT_PID_VIDEO_SAMPLE);
	}
	CHECK_FAIL(nRc);

	if( *ppOutBuffer != NULL && pParam->videoTransition && isTransition && pParam->videoTransition->pUser)
 	{
 		VOLOGI("+ be to transition");
 		/*transition prepare*/
 		VOEDT_TRANSITION_BUFFER effect1;
 		effect1.effectBuffer = (VO_PBYTE )(*ppOutBuffer)->pBuffer;
 		VOEDT_TRANSITION_BUFFER effect2;
 		effect2.effectBuffer = (VO_PBYTE )pTmpBuffer->pBuffer;
 		effect2.fTransitionValue = pPlayer->GetVideoTransitionValue();
 		effect2.nType = pParam->videoTransition->nType;
 		VOLOGI("+ be to pSetSize");
 		nRc = pParam->videoTransition->pSetSize(pParam->videoTransition->pUser, vf.nWidth, vf.nHeight);
 		VOLOGI("- be to pSetSize");
 		VOLOGI("+ be to pEffectFunc");
 		nRc = pParam->videoTransition->pEffectFunc(pParam->videoTransition->pUser,&effect1,&effect2);
 		VOLOGI("- be to pEffectFunc");
 		VOLOGI("- be to transition");
 	}
 	else
 	{
 		VOLOGI("+ not to transition");
 		*ppOutBuffer = pTmpBuffer;
 		VOLOGI("- not to transition");
 	}


	return nRc;
}
#else
VO_S32 CVideoEditorEngine::DoVideo(CPlayerBaseCtrl* pPlayer,VO_VIDEO_BUFFER* pOutBuffer,bool isTransition,VO_U32 position,VO_S32 *pReserve)
{
	VO_S32 nRc ;

	nRc = SetClipParam((VOEDT_CLIP_HANDLE*)pPlayer, VOEDT_PID_PLAYER_OUTPUT_VIDEO_FORMAT, (VO_VOID*)&m_sVideoFormat);

	VOEDT_CLIP_PARAM *pParam;
	GetClipParam((VOEDT_CLIP_HANDLE*)pPlayer,VOEDT_PID_PLAYER_CLIP_PARAM,&pParam);
	VOMP_BUFFERTYPE *buffer;

	VOEDT_CLIP_POSITION_FORMAT vofout;
		/*Video Transfor*/
	VOEDT_CLIP_POSITION_FORMAT vofin;
	VOMP_VIDEO_FORMAT vf;
	pPlayer->GetClipParam(VOEDT_PID_VIDEO_FORMAT,&vf);
	
	vofin.nHeight = vf.Height;
	vofin.nWidth = vf.Width;
	vofin.nLeft = 0;
	vofin.nTop = 0;

	m_Transfer_buffer.ColorType =VO_COLOR_YUV_PLANAR420;
	VO_VIDEO_BUFFER *present = &m_Transfer_buffer;

	if(!m_bPlayerEditor)
	{
		nRc = pPlayer->SetPosition(&buffer,&position);
		vofout.nHeight = m_sVideoFormat.nHeight;
		vofout.nWidth = m_sVideoFormat.nWidth;
		vofout.nLeft = 0;
		vofout.nTop = 0;

	}
	else
	{
		nRc	= pPlayer->GetSample(&buffer,position,pReserve,VOMP_PID_VIDEO_SAMPLE);
		vofout.nHeight = m_outputParam.VideoParam.nHeight;
		vofout.nWidth =  m_outputParam.VideoParam.nWidth;
		vofout.nLeft = 0;
		vofout.nTop = 0;
	}
	VO_U32 start = voOS_GetSysTime();
	if(!present->Buffer[0] )EX_INIT_BUFFER(vofout.nWidth,vofout.nHeight,present);
	if(!pOutBuffer->Buffer[0] )EX_INIT_BUFFER(vofout.nWidth,vofout.nHeight,pOutBuffer);
	VO_U32 end = voOS_GetSysTime();
	VOLOGE("New Memory OK! Cost %f Seconds!\n" , ( end - start ) / 1000.);

	nRc = m_SampleTransformer.DoTransformer((VO_VIDEO_BUFFER*)buffer->pBuffer,present,vofin,vofout,buffer->llTime);

	if(pOutBuffer!= NULL && pParam->videoTransition && isTransition && pParam->videoTransition->pUser)
	{
		VOLOGI("+ be to transition");
		/*transition prepare*/
		VOEDT_TRANSITION_BUFFER effect1;
		effect1.effectBuffer = (VO_PBYTE )pOutBuffer;
		VOEDT_TRANSITION_BUFFER effect2;
		effect2.effectBuffer = (VO_PBYTE )present;
		effect2.fTransitionValue = pPlayer->GetVideoTransitionValue();
		effect2.nType = pParam->videoTransition->nType;
		VOLOGI("+ be to pSetSize");
		nRc = pParam->videoTransition->pSetSize(pParam->videoTransition->pUser,vofout.nWidth,vofout.nHeight);
		VOLOGI("- be to pSetSize");
		VOLOGI("+ be to pEffectFunc");
		nRc = pParam->videoTransition->pEffectFunc(pParam->videoTransition->pUser,&effect1,&effect2);
		VOLOGI("- be to pEffectFunc");
		VOLOGI("- be to transition");
	}
	else
	{
		//*pOutBuffer = present;
		VOLOGI("+ not to transition");
		nRc = m_SampleTransformer.DoTransformer((VO_VIDEO_BUFFER*)buffer->pBuffer,pOutBuffer,vofin,vofout,buffer->llTime);
		VOLOGI("- not to transition");
		/*VO_S32 len = present->Stride[0] * vofout.nHeight;
		memcpy(pOutBuffer->Buffer[0],present->Buffer[0],len);
		 len = present->Stride[1] * vofout.nHeight /2;
		memcpy(pOutBuffer->Buffer[1],present->Buffer[1],len);
		 len = present->Stride[2] * vofout.nHeight/2;
		 memcpy(pOutBuffer->Buffer[2],present->Buffer[2],len);
		 
		 pOutBuffer->Time = m_sampleTime_video;
		 m_encoderCtrl.ToEncoder(1,(void*)pOutBuffer);*/
	}

	
	return nRc;
}
#endif
VO_S32 CVideoEditorEngine::DoAudio(CClipPlayer* pClip,VOMP_BUFFERTYPE** pOutBuffer,bool isTransition,VO_U32 position,VO_S32 *pReserve)
{
	VOLOGI("+ DoAudio.IsTransition:%d", isTransition);
	CClipPlayer *pPlayer = pClip;
	VOMP_BUFFERTYPE *pPresent = NULL;
	VO_S32 nRc ;
	VOLOGI("1");
	if(!m_bPlayerEditor)
	{
		nRc = pPlayer->SetPosition(&pPresent,&position,VOEDT_PID_AUDIO_SAMPLE);
	}
	else
	{
		nRc	= pPlayer->GetSample(&pPresent,position,pReserve,VOEDT_PID_AUDIO_SAMPLE);
	}
	VOLOGI("nRc:0x%08x", nRc);
	VOEDT_CLIP_PARAM *pParam;
	GetClipParam((VOEDT_CLIP_HANDLE*)pPlayer,VOEDT_PID_PLAYER_CLIP_PARAM,&pParam);
VOLOGI("3");
	if(*pOutBuffer && (*pOutBuffer)->pBuffer&& pPresent && pPresent->pBuffer && pParam->audioTransition && isTransition && pParam->audioTransition->pUser)
	{
		/*transition prepare*/
		VOLOGI("4");	

		VOEDT_TRANSITION_BUFFER effect1;
		effect1.nSize = (*pOutBuffer)->nSize;
		effect1.effectBuffer = (VO_PBYTE )(*pOutBuffer)->pBuffer;
		VOEDT_TRANSITION_BUFFER effect2;
		effect2.effectBuffer = (VO_PBYTE )pPresent->pBuffer;
		effect2.fTransitionValue = pPlayer->GetAudioTransitionValue();
		effect2.nType = pParam->audioTransition->nType;
		effect2.nSize = pPresent->nSize;
		VOLOGI("Transition nType:%d");
		nRc = pParam->audioTransition->pEffectFunc(pParam->audioTransition->pUser,&effect1,&effect2);
		VOLOGI("5");	

	}
	else
		*pOutBuffer = pPresent;
	VOLOGI("- DoAudio");
	return nRc;
}
VO_S32 CVideoEditorEngine::SetAudioPosition(VOMP_BUFFERTYPE **ppOutBuffer,VO_U32 *pPos)
{
	VO_S32 nRc ;
	CPlayerBaseCtrl *(pPlayer[NUM_ARRAY]);
	VO_S32 pos[NUM_ARRAY]; 
	bool bTransition[NUM_ARRAY];
	VO_U32 nPosIn = *pPos;
	VOMP_BUFFERTYPE* pOutBuffer = *ppOutBuffer;// = m_AudioTransfer_buffer;
	VOMP_BUFFERTYPE* pOutBufferpp = NULL;
	
	bool bTrans = false;	
	VO_S32 num = -1;
	nRc = FindEditorPosition(pPlayer,pos,bTransition,*pPos,&num,VOEDT_PID_CLIP_TYPE_AUDIO);
	if(num == 0) *pPos +=100;
	if(nRc != VOEDT_ERR_NONE) return nRc;
	for(VO_S32 i = num -1 ;i >= 0;i--)
		bTrans |= bTransition[i];
	VO_S32 volume = 0;
	VO_S32 volumetmp = 0;
	if(m_nActionMode & VOEDT_ACTIONMODE_TRIM)
	{
		VO_U32 	*time =&m_sampleTime_audio;
		CClipPlayer *ppp = (CClipPlayer*)pPlayer[num-1];
		nRc =ppp->GetTrimSample(VOEDT_PID_AUDIO_SAMPLE, time);
		//m_sampleTime_audio =time;
	}
	else
	{
		for(VO_S32 i = 0 ;i < num;i++)
		{	
			VOLOGI("+%d",i);
			CClipPlayer *ppp = (CClipPlayer*)pPlayer[i];
			volume = VOEDT_AUDIO_MAX_VOLUME;//pPlayer[i]->GetAudioTransitionValue();
			nRc = DoAudio(ppp,&pOutBufferpp,bTransition[i],pos[i],&volume);	
			if(volume >= volumetmp){ pOutBuffer = pOutBufferpp; volumetmp = volume;}
			VOLOGI("-%d",i);
		}

		if( m_nActionMode & VOEDT_ACTIONMODE_THUMNAIL )
			pOutBuffer->llTime = nPosIn;
		else
			pOutBuffer->llTime = m_sampleTime_audio;

		VOLOGI("Audio TimeStamp:%lld",pOutBuffer->llTime);
 		m_sampleTime_audio = m_nStartPosition + (VO_S32) (1000.0 * m_nAudioSize_count /
 			(m_sAudioFormat.Channels * m_sAudioFormat.SampleBits/8 * m_sAudioFormat.SampleRate));
		VOLOGI("SampleTime_audio:%d, StartTime:%llu",m_sampleTime_audio,m_nStartPosition );
 		m_nAudioSize_count += pOutBuffer->nSize;

		if(m_nActionMode & VOEDT_ACTIONMODE_PREVIEW_AUDIO 
			|| m_nActionMode & VOEDT_ACTIONMODE_THUMNAIL)	{
	
			int nID = m_nActionMode & VOEDT_ACTIONMODE_PREVIEW_VIDEO ? VOEDT_CB_AUDIO_SAMPLE : VOEDT_CB_AUDIO_CHUNK;
			VO_U32 start = voOS_GetSysTime();
 			VOEDT_CALLBACK_BUFFER edt;
 			edt.pData = (void*)pOutBuffer;
 			edt.pClipHandle = NULL;
 			CallBackFunc(nID,&edt);
			VO_U32 end = voOS_GetSysTime();
			VOLOGE("Callback Cost %f Seconds! AudioTimeStamp:%d\n" , ( end - start ) / 1000., m_sampleTime_audio);
			
		}

		if(m_nActionMode & VOEDT_ACTIONMODE_TRANSCODE)	{
			if(out_file_pcm)	{	
				VOLOGI("before dump audio file");
				VO_U32 writed = fwrite(pOutBuffer->pBuffer, 1,  pOutBuffer->nSize, out_file_pcm) ;
				if(writed != pOutBuffer->nSize)
					VOLOGE("dump file error %d!=  %d" ,writed,pOutBuffer->nSize);
				VOLOGI("after dump audio file");
			}
			
			//fwrite(pOutBuffer->pBuffer, 1, pOutBuffer->nSize, out_file_pcm);
			m_encoderCtrl.ToEncoder(0,pOutBuffer);
		}

	}

	return nRc;
}
VO_S32 CVideoEditorEngine::SeekEditor(VO_U32 pos)
{
	VOLOGI("+ SeekEditor.Pos:%d",pos);

	CPlayerBaseCtrl *(pPlayer[NUM_ARRAY]);
	VO_S32 targetpos ;
	bool bTransition;
	VO_S32 nNum = -1;
	VOMP_BUFFERTYPE *pOutBuffer = NULL;
	VO_S32 nRc = FindEditorPosition(pPlayer,&targetpos,&bTransition,pos,&nNum,VOEDT_PID_CLIP_TYPE_VIDEO);
	if(nRc == VOEDT_ERR_NONE){
		for(int i= 0;i<nNum;i++)
			pPlayer[i]->SetPosition(&pOutBuffer, (VO_U32*)&targetpos, VOEDT_PID_VIDEO_SAMPLE);
	}
	nNum = -1;
	nRc = FindEditorPosition(pPlayer,&targetpos,&bTransition,pos,&nNum,VOEDT_PID_CLIP_TYPE_AUDIO);
	if(nRc == VOEDT_ERR_NONE) {
			for(int i= 0;i<nNum;i++)
				pPlayer[i]->SetPosition(&pOutBuffer, (VO_U32*)&targetpos, VOEDT_PID_AUDIO_SAMPLE);
	}
	//nRc = FindEditorPosition(&player2,&targetpos,&bTransition,pos,&nNum,VOEDT_PID_CLIP_TYPE_AUDIO);
	//if(nRc == VOEDT_ERR_NONE) player2->SetPosition(&pOutBuffer, (VO_U32*)&targetpos);
	VOLOGI("- SeekEditor");

	return VOEDT_ERR_NONE;
}
VO_S32 CVideoEditorEngine::SetVideoPosition(VOMP_BUFFERTYPE **ppOutBuffer,VO_U32 *pPos)
{
	VOLOGI("+ SetVideoPosition");
	VO_S32 nRc ;
	CPlayerBaseCtrl *(pPlayer[NUM_ARRAY]);
	VO_S32 pos[NUM_ARRAY]; 
	bool bTransition[NUM_ARRAY];
	VO_S32 relativeTime = 0;
	VO_S32 num = -1;
	VO_U32 nPosIn = *pPos;
	nRc = FindEditorPosition(pPlayer,pos,bTransition,*pPos,&num,VOEDT_PID_CLIP_TYPE_VIDEO);
	if(num <= 0) *pPos += 20;
	if(nRc != VOEDT_ERR_NONE)
	{
		VOLOGI("- SetVideoPosition1");
		return nRc;
	}
	VO_S32 nSize = 0;
	if(m_nActionMode & VOEDT_ACTIONMODE_TRIM)
	{	
		VO_U32 *nTime = &m_sampleTime_video;
		CClipPlayer *ppp = (CClipPlayer*)pPlayer[num-1];
		nRc =ppp->GetTrimSample(VOEDT_PID_VIDEO_SAMPLE, nTime);
	}
	else
	{
		VO_U32 start = voOS_GetSysTime();
		for(VO_S32 i =0 ;i<num;i++)
		{	
			CPlayerBaseCtrl *ppp = (CPlayerBaseCtrl*)pPlayer[i];
			nRc = DoVideo(ppp, ppOutBuffer,bTransition[i],pos[i],&relativeTime);
			GetClipPosition((VOEDT_CLIP_HANDLE*)pPlayer[i],NULL);
		}	

		VO_U32 end = voOS_GetSysTime();
		VOLOGE("DoVideo OK! Cost %f Seconds!\n" , ( end - start ) / 1000.);

		VO_VIDEO_FORMAT vf;
		vf.Height = m_pVideoFormat_PresentSelected->nHeight;
		vf.Width = m_pVideoFormat_PresentSelected->nWidth;
#if 1
		/*test blend*/
		num = -1;
		VO_S32 nRc_T = FindEditorPosition(pPlayer,pos,bTransition,*pPos,&num,VOEDT_PID_CLIP_TYPE_TEXT);
		if(nRc_T == VOEDT_ERR_NONE)
		{	
			for(VO_S32 i =0 ; i<num; i++)
			{	
				CTextPlayer * tp = (CTextPlayer*)pPlayer[i];
				tp->BlendText((VO_PBYTE)*ppOutBuffer,vf);
			}
		}
#endif
		VOLOGI("1");
	
		VOMP_BUFFERTYPE tmpBuf;	
		memset(&tmpBuf, 0x00 ,sizeof(VOMP_BUFFERTYPE));
		if(*ppOutBuffer)
		{
			memcpy(&tmpBuf, *ppOutBuffer, sizeof(VOMP_BUFFERTYPE));
			if( m_nActionMode & VOEDT_ACTIONMODE_POSITION 
				|| m_nActionMode & VOEDT_ACTIONMODE_THUMNAIL)
				tmpBuf.llTime = nPosIn;
			else
				tmpBuf.llTime = m_sampleTime_video;
		}
		VOLOGI("Video TimeStamp1:%lld.Sample size:%d",tmpBuf.llTime, tmpBuf.nSize);
		VOLOGI("2");
	
		if(m_nActionMode & VOEDT_ACTIONMODE_PREVIEW_VIDEO 
			|| m_nActionMode & VOEDT_ACTIONMODE_POSITION 
			|| m_nActionMode & VOEDT_ACTIONMODE_THUMNAIL)
		{
			
			VOLOGI("ActionMode : 0x%08x", m_nActionMode);
			{
#if 0
 				VO_VIDEO_FORMAT infv;
 				infv.Height = m_outputParam.VideoParam.nHeight;
 				infv.Width = m_outputParam.VideoParam.nWidth;
 				VOLOGI("Video Output:(%d * %d)",infv.Width,infv.Height);
 				m_SampleTransformer.DoTransformer2((VO_VIDEO_BUFFER*)pOutBuffer,(VO_VIDEO_BUFFER*)m_videoOutputBuffer.pBuffer,infv,infv,0);
#endif				
 				VOEDT_CALLBACK_BUFFER edt;
				edt.pData = (void*)&tmpBuf ;
 				edt.nReserve = tmpBuf.nSize;
 				edt.pClipHandle = NULL;
 				VOLOGI("before preview video callback");
 				VO_U32 start = voOS_GetSysTime();
				VO_U32 ID;
				(m_nActionMode & VOEDT_ACTIONMODE_THUMNAIL) ? ID = VOEDT_CB_VIDEO_THUMBNAIL : ID = VOEDT_CB_VIDEO_SAMPLE;
 				CallBackFunc(ID ,&edt);
				VOLOGI("Video TimeStamp2:%lld",tmpBuf.llTime);

 				VO_U32 end = voOS_GetSysTime();
 				VOLOGI("Video Sample Callback OK! Cost %f Seconds!\n" , ( end - start ) / 1000.);
 				VOLOGI("after preview video callback");
			}	
		}

		if(m_nActionMode & VOEDT_ACTIONMODE_TRANSCODE)
		{
			VOLOGI("VOEDT_ACTIONMODE_TRANSCODE");

			if(out_file_ARGB32){
				fwrite(tmpBuf.pBuffer, 1, tmpBuf.nSize, out_file_ARGB32);
			}
			VO_U32 start = voOS_GetSysTime();
		//	m_encoderCtrl.ToEncoder(1, m_videoOutputBuffer.pBuffer);	
			VO_VIDEO_BUFFER buf;
			memset(&buf, 0x00, sizeof(VO_VIDEO_BUFFER));
			buf.Buffer[0]= tmpBuf.pBuffer;
			buf.Stride[0] = m_outputParam.VideoParam.nWidth * 4;
			buf.ColorType =VO_COLOR_ARGB32_PACKED;
			buf.Time = tmpBuf.llTime;
			m_encoderCtrl.ToEncoder(1, &buf);	

			VO_U32 end = voOS_GetSysTime();
			VOLOGI("Video Encoder OK! Cost %f Seconds!\n" , ( end - start ) / 1000.);

		}	
		m_nVideoSample_nums ++;
		m_sampleTime_video =m_nStartPosition + m_nVideoSample_nums * AV_TIME_BASE  / (m_nFrameRate /100.);

	}
	VOLOGI("- SetVideoPosition2. Now VideoSampleTime:%d",m_sampleTime_video);

	return nRc ;
}
VO_S32 CVideoEditorEngine::CheckClip(VOEDT_CLIP_HANDLE *pClipHandle)
{
	//	if(pClipHandle == NULL) return VOEDT_ERR_NONE;
	CPlayerBaseCtrl *player = (CPlayerBaseCtrl*)pClipHandle;
	POSITION pos = m_listCPlayer_Base.Find(player);
	if(pos != NULL) return VOEDT_ERR_NONE;
	else return VOEDT_ERR_CLIP_NOTFOUND;
}
VO_S32 CVideoEditorEngine::SetClipPosition(VOEDT_CLIP_HANDLE *pClip, VO_U32 position)
{
	VO_S32 nRc = 0;
	CPlayerBaseCtrl *player = (CPlayerBaseCtrl*)pClip;
	player->SetClipParam(VOEDT_PID_PLAYER_OUTPUT_VIDEO_FORMAT, &m_sVideoFormat);
	//SetClipParam(pClip, VOEDT_PID_PLAYER_OUTPUT_VIDEO_FORMAT, &m_sVideoFormat);
	nRc= SetAClipPosition(player,&position );
	if(nRc == VOEDT_ERR_NONE)
	{	
		VOEDT_CALLBACK_BUFFER edt;
		edt.pData = (void*)&position;
		edt.pClipHandle = pClip;
		CallBackFunc(VOEDT_CB_CLIP_POSITION,&edt);
	}
	return nRc;
}
VO_S32 CVideoEditorEngine::SetEditorPosition(VO_U32 position)
{
	VO_S32 nRc = 0;
	m_pVideoFormat_PresentSelected = & m_sVideoFormat;
	nRc = RefreshClipsVideoFormat(m_pVideoFormat_PresentSelected);
	CHECK_FAIL(nRc);

	VOMP_BUFFERTYPE *ppOutBuffer = NULL;
	m_nActionMode |= VOEDT_ACTIONMODE_POSITION;
	nRc = SetVideoPosition(&ppOutBuffer,&position);
	m_nActionMode &= ~VOEDT_ACTIONMODE_POSITION;
	m_nStartPosition = position;
	VOLOGI("Now Editor Position:%lld", m_nStartPosition);
	
	CHECK_FAIL(nRc);

	return nRc;
}


VO_S32 CVideoEditorEngine::SetAClipPosition(CPlayerBaseCtrl *player,VO_U32 *pPos)
{
	VOLOGI("Set Position :%d", *pPos);
	int nRc = 0;
	nRc = RefreshClipsAudioFormat(&m_sAudioFormat);
	CHECK_FAIL(nRc);

	player->SetPosition(pPos);
	VOMP_BUFFERTYPE *buffer = NULL;

	if(!(player->m_nClipType & VOEDT_PID_CLIP_TYPE_TEXT ))
	{
		CPlayerBaseCtrl *clip = (CPlayerBaseCtrl*)player;
		clip->SetPosition(&buffer,pPos, 0);
	if(player->m_nClipType & VOEDT_PID_CLIP_TYPE_VIDEO )
	{		
		
#if 0			
		/* 11/03/21 add */
		if(m_videoOutputBuffer.pBuffer != NULL)
		{
			VO_VIDEO_FORMAT infv;	
			player->GetClipParam(VOEDT_PID_VIDEO_FORMAT,&infv);
			VO_VIDEO_FORMAT outfv;
			outfv.Height = m_outputParam.VideoParam.nHeight;
			outfv.Width = m_outputParam.VideoParam.nWidth;
			m_SampleTransformer.DoTransformer2((VO_VIDEO_BUFFER*)buffer->pBuffer,(VO_VIDEO_BUFFER*)m_videoOutputBuffer.pBuffer,infv,outfv,0);

			//VOMP_BUFFERTYPE vb;
			//vb.pBuffer = (unsigned char*)pOutBuffer;
			VOEDT_CALLBACK_BUFFER edt;
			m_videoOutputBuffer.llTime = *pPos;
			edt.pData = (void*)&m_videoOutputBuffer ;
			edt.pClipHandle = (VOEDT_CLIP_HANDLE*)player;
			CallBackFunc(VOEDT_CB_VIDEO_SAMPLE,&edt);
		}
		/* 11/03/21 add */
#endif
	}


	VOEDT_CALLBACK_BUFFER edt;
	edt.pData = (void*)buffer;
	edt.pClipHandle = (VOEDT_CLIP_HANDLE*)player;
	CallBackFunc(VOEDT_CB_VIDEO_SAMPLE,&edt);
		
	
	
	}
	return VOEDT_ERR_NONE;
}
VO_S32 CVideoEditorEngine::GetEditorAudioSample(VO_U32 position)
{
	VO_S32 nRc ;
	
	CPlayerBaseCtrl *pPlayer = NULL;

	VOMP_BUFFERTYPE *pOutBuffer = NULL;
	m_nActionMode |= VOEDT_ACTIONMODE_THUMNAIL;
	nRc = SetAudioPosition(&pOutBuffer, &position);
	m_nActionMode &= ~VOEDT_ACTIONMODE_THUMNAIL;

	CHECK_FAIL(nRc);

// 	VOEDT_CALLBACK_BUFFER edt;
// 	edt.pData = (void*)pOutBuffer;
// 	edt.pClipHandle = NULL;
// 	CallBackFunc(VOEDT_CB_AUDIO_SAMPLE,&edt);
	return nRc;
}
VO_S32 CVideoEditorEngine::GetEditorThumbNail(VO_U32 position)
{
	VO_S32 nRc ;
	VOLOGI("Position:%d", position);
	m_pVideoFormat_PresentSelected = & m_sVideoFormat_Thumbnail;
	nRc = RefreshClipsVideoFormat(m_pVideoFormat_PresentSelected);
	CHECK_FAIL(nRc);

	CPlayerBaseCtrl *pPlayer = NULL;

	VOMP_BUFFERTYPE *pOutBuf = NULL;
	m_nActionMode |= VOEDT_ACTIONMODE_THUMNAIL;

	nRc = SetVideoPosition(&pOutBuf, &position);
	m_nActionMode &= ~VOEDT_ACTIONMODE_THUMNAIL;

	CHECK_FAIL(nRc);

//  	VOEDT_CALLBACK_BUFFER edt;
//  	VO_S32 nPid = VOEDT_CB_VIDEO_THUMBNAIL;
//  	edt.pData = (void*)pOutBuf;
//  	edt.pClipHandle = NULL;
//  	edt.nReserve = pOutBuf->nSize;
//  	CallBackFunc(nPid,&edt);
	return nRc;
}
VO_S32 CVideoEditorEngine::GetClipThumbNail(VOEDT_CLIP_HANDLE *pClipHandle ,VO_S32 position)
{

	VOLOGI("position %d",position)
	VO_S32 nRc ;
	CPlayerBaseCtrl *pPlayer = (CPlayerBaseCtrl*)pClipHandle;

	if( !(pPlayer->m_nClipType & VOEDT_PID_CLIP_TYPE_VIDEO ) && pPlayer->m_nClipType != VOEDT_PID_CLIP_TYPE_IMAGE)
		return VOEDT_ERR_FAILED;
	
	VO_U32 buffersize = 0;
	
	VOEDT_VIDEO_FORMAT tf;
	GetClipParam((VOEDT_CLIP_HANDLE*)pPlayer,VOEDT_PID_THUMBNAIL_FORMAT,&tf);
	VOMP_VIDEO_FORMAT vtf ;
	vtf.Height = tf.nHeight;
	vtf.Type = tf.ColorType;
	vtf.Width = tf.nWidth;
	VOLOGI("ThumbNail Format （%d ,%d)",vtf.Width,vtf.Height);

	VOMP_BUFFERTYPE *pThumbnailBuffer = NULL;
	if(vtf.Height == 0 || vtf.Width == 0  )
		nRc = VOEDT_ERR_OUTPUT_FORMAT;
	else
		nRc= GetAClipThumbNail(pPlayer,&position, &pThumbnailBuffer);
	

	CHECK_FAIL(nRc);

	VOEDT_CALLBACK_BUFFER edt;
	VO_S32 nPid  =0; 
	VO_U32 value = 0;
	GetClipParam((VOEDT_CLIP_HANDLE*)pPlayer,VOEDT_PID_DROP_BLACK_FRAME,&value);
	value ==0 ? nPid = VOEDT_CB_VIDEO_THUMBNAIL : nPid = VOEDT_CB_VIDEO_NOBLACK_THUMBNAIL;
	edt.pData = (void*)pThumbnailBuffer;
	edt.pClipHandle = pClipHandle;
	edt.nReserve = pThumbnailBuffer->nSize;
	CallBackFunc(nPid,&edt);
	
	return nRc;
}


VO_S32 CVideoEditorEngine::GetAClipThumbNail(CPlayerBaseCtrl *player,VO_S32 *pPos,VOMP_BUFFERTYPE **outBuf,VO_S32 nFlag)
{
	VOLOGI("in GetAClipThumbNail");
	VO_S32 nRc = 0;
	VOEDT_CLIP_POSITION_FORMAT vofin;
	VOMP_BUFFERTYPE *buffer;
	SetClipParam((VOEDT_CLIP_HANDLE*)player, VOEDT_PID_PLAYER_OUTPUT_VIDEO_FORMAT, (VO_VOID*)&m_sVideoFormat_Thumbnail);

	nRc = player->SetPosition(outBuf,(VO_U32*)pPos,VOEDT_PID_VIDEO_SAMPLE);
	CHECK_FAIL(nRc);

	VOLOGI("out GetAClipThumbNail. BufferSize:%d", (*outBuf)->nSize);
	return nRc;
}
VO_S32 CVideoEditorEngine::GetEditorStatus(VO_U32* nStatus)
{
	*nStatus = m_bPlayerEditor;
	return VOEDT_ERR_NONE;
}

VO_S32 CVideoEditorEngine::SetThumbNailFormat(VOEDT_VIDEO_FORMAT *format)
{
	
	m_sVideoFormat_Thumbnail.ColorType = format->ColorType;
	CheckWidthHeight(format->nWidth,format->nHeight);
	m_sVideoFormat_Thumbnail.nHeight =format->nHeight ;
	m_sVideoFormat_Thumbnail.nWidth = format->nWidth;
	VOLOGI("ThumbNail Format:ct %d, w %d,h %d",m_sVideoFormat_Thumbnail.ColorType,m_sVideoFormat_Thumbnail.nWidth ,m_sVideoFormat_Thumbnail.nHeight );
	return VOEDT_ERR_NONE;
}
VO_S32 CVideoEditorEngine::GetThumbNailFormat(VOEDT_VIDEO_FORMAT *format)
{
	format->ColorType = m_sVideoFormat_Thumbnail.ColorType;
	format->nHeight = m_sVideoFormat_Thumbnail.nHeight;
	format->nWidth = m_sVideoFormat_Thumbnail.nWidth;
	//format = &m_Thumbnail_format;
	return VOEDT_ERR_NONE;
}
VO_S32 CVideoEditorEngine::RefreshClipsVideoFormat(VOEDT_VIDEO_FORMAT *format)
{
	VO_S32 nRc = 0;
	POSITION pos = m_listCPlayer_Base.GetHeadPosition ();
	if(format->nWidth == 0 || format->nHeight == 0 || format->ColorType == 0)
		return VOEDT_ERR_INVLAID_FORMAT;
	while (pos != NULL)
	{
		CPlayerBaseCtrl *tmpClip = m_listCPlayer_Base.GetNext (pos);
		if( tmpClip->m_nClipType & VOEDT_PID_CLIP_TYPE_VIDEO)
			tmpClip->SetClipParam(VOEDT_PID_PLAYER_OUTPUT_VIDEO_FORMAT, format);
	}
	return nRc;
}
VO_S32 CVideoEditorEngine::RefreshClipsAudioFormat(VOEDT_AUDIO_FORMAT *format)
{
	VO_S32 nRc = 0;
	POSITION pos = m_listCPlayer_Base.GetHeadPosition ();
	if(format->Channels == 0 || format->SampleBits == 0 || format->SampleRate == 0)
		return VOEDT_ERR_INVLAID_FORMAT;
	while (pos != NULL)
	{
		CPlayerBaseCtrl *tmpClip = m_listCPlayer_Base.GetNext (pos);
		if( tmpClip->m_nClipType & VOEDT_PID_CLIP_TYPE_AUDIO)
			tmpClip->SetClipParam(VOEDT_PID_PLAYER_AUDIO_FORMAT_TARGENT, format);
	}
	return nRc;
}
VO_S32 CVideoEditorEngine::SetAudioFormat(VOEDT_AUDIO_FORMAT *format)
{
	m_sAudioFormat.Channels = format->Channels;
	m_sAudioFormat.SampleBits = format->SampleBits;
	m_sAudioFormat.SampleRate = format->SampleRate;

	VOEDT_CALLBACK_BUFFER edt;
	edt.pData = (void*)&m_sAudioFormat;
	edt.pClipHandle = NULL;
	CallBackFunc(VOEDT_CB_AUDIO_FORMAT,&edt);

	return VOEDT_ERR_NONE;
}
VO_S32 CVideoEditorEngine::SetVideoFormat(VOEDT_VIDEO_FORMAT *format)
{
	CheckWidthHeight(format->nWidth,format->nHeight);
	m_sVideoFormat.ColorType = format->ColorType;
	m_sVideoFormat.nHeight = format->nHeight;
	m_sVideoFormat.nWidth = format->nWidth;

	VOEDT_CALLBACK_BUFFER edt;
	edt.pData = (void*)&m_sVideoFormat;
	edt.pClipHandle = NULL;
	CallBackFunc(VOEDT_CB_VIDEO_FORMAT,&edt);
	
	return VOEDT_ERR_NONE;
}
VO_S32 CVideoEditorEngine::GetOutputFormat(VOEDT_VIDEO_FORMAT *format)
{
	format->ColorType = m_sVideoFormat.ColorType ;
	format->nHeight = m_sVideoFormat.nHeight ;
	format->nWidth = m_sVideoFormat.nWidth ;
	return VOEDT_ERR_NONE;
}
VO_S32 CVideoEditorEngine::GetEditorVideoFormat(VOEDT_VIDEO_FORMAT *format)
{
	if(m_pVideoFormat_PresentSelected == NULL)
		memset(format, 0, sizeof(VOEDT_VIDEO_FORMAT));
	else
	{
		format->ColorType = m_pVideoFormat_PresentSelected->ColorType ;
		format->nHeight = m_pVideoFormat_PresentSelected->nHeight ;
		format->nWidth = m_pVideoFormat_PresentSelected->nWidth ;
	}
	
	return VOEDT_ERR_NONE;
}

VO_S32 CVideoEditorEngine::GetEditorAudioFormat(VOEDT_AUDIO_FORMAT *format)
{
		format->Channels = m_sAudioFormat.Channels;
		format->SampleBits = m_sAudioFormat.SampleBits;
		format->SampleRate = m_sAudioFormat.SampleRate;
		return VOEDT_ERR_NONE;
}
VO_S32 CVideoEditorEngine::UpdateEditorEverything()
{
	VO_S32 nRc ;
	VO_U32 duration;

	VOEDT_CALLBACK_BUFFER edt;
	VO_S32 nPid = VOEDT_CB_EDITOR_UPDATE;
	edt.pClipHandle = NULL;
	edt.pData = NULL;
	CallBackFunc(nPid,&edt);

	CPlayerBaseCtrl *(pPlayer[10]);

	VO_S32 num = 0;
	//callback video
	nRc = FindEditor(pPlayer,&num,VOEDT_PID_CLIP_TYPE_VIDEO);
	if(nRc == VOEDT_ERR_NONE)
	{
		for(VO_S32 i = 0 ;i< num ;i++)
		{	
			nPid = VOEDT_CB_ADD_VIDEO;
			edt.pClipHandle =	(VOEDT_CLIP_HANDLE*)	pPlayer[i];//->GetClipInfo();
			edt.pData = (void*)pPlayer[i];
			CallBackFunc(nPid,&edt);

			VO_U32 value = 1;
			SetClipParam((VOEDT_CLIP_HANDLE*)pPlayer[i],VOEDT_PID_DROP_BLACK_FRAME,&value);
			GetClipThumbNail(edt.pClipHandle  ,0);
		}
	}
	//callback audio
	num = 0;
	nRc = FindEditor(pPlayer,&num,VOEDT_PID_CLIP_TYPE_AUDIO);
	if(nRc == VOEDT_ERR_NONE)
	{
		for(VO_S32 i = 0 ;i< num ;i++)
		{		
			nPid = VOEDT_CB_ADD_AUDIO;
			edt.pClipHandle =	(VOEDT_CLIP_HANDLE*) pPlayer[i];//->GetClipInfo();
			edt.pData = (void*)pPlayer[i];
			CallBackFunc(nPid,&edt);
		}
	}
	//callback text
	nRc = FindEditor(pPlayer,&num,VOEDT_PID_CLIP_TYPE_TEXT);
	if(nRc == VOEDT_ERR_NONE)
	{
		for(VO_S32 i = 0 ;i< num ;i++)
		{	
			nPid = VOEDT_CB_ADD_TEXT;
			edt.pClipHandle  =			(VOEDT_CLIP_HANDLE*)pPlayer[i];//->GetClipInfo();
			edt.pData = (void*)pPlayer[i];
			CallBackFunc(nPid,&edt);
		}
	}
	GetEditorDuration(NULL);
	return nRc;
}



VO_S32 CVideoEditorEngine::FindListPos(CObjectList<CPlayerBaseCtrl> &listCPlayer,CPlayerBaseCtrl *player,POSITION &pos)
{
	POSITION postmp;
	 pos = postmp = listCPlayer.GetHeadPosition();
	if(player == listCPlayer.GetHead())
		return VOEDT_ERR_NONE;

	while(postmp)
	{
		if(player == listCPlayer.GetNext(postmp))
			return VOEDT_ERR_NONE;
		pos =postmp;
	}
	
	return VOEDT_ERR_CLIP_NOTFOUND;
}
VO_S32 CVideoEditorEngine::DeleteAllClip()
{
	VO_S32 nRc = 0;
	POSITION pos = m_listCPlayer_Base.GetHeadPosition ();
	while (pos != NULL)
	{
		CPlayerBaseCtrl *tmpClip = m_listCPlayer_Base.GetNext (pos);
		DeleteClip((VOEDT_CLIP_HANDLE *)tmpClip);
	}
	return nRc;
}
VO_S32 CVideoEditorEngine::DeleteClip(VOEDT_CLIP_HANDLE *pClipHandle)
{
	VO_S32 nRc = 0;
	CClipPlayer *player;
	POSITION pos;

	VO_U32 cliptype;
	int clipID = (int)pClipHandle;
	VOLOGI("Deleted ClipID:0x%08x, %d",pClipHandle, clipID);

	if(pClipHandle)
	{

		player = (CClipPlayer *)pClipHandle;
		player->GetClipParam(VOEDT_PID_CLIP_TYPE,&cliptype);

		m_listCPlayer_Base.Remove(player);
		player->Stop();
		delete player;
		pClipHandle = NULL;

		VOEDT_CALLBACK_BUFFER edt;
		VO_S32 nPid = VOEDT_CB_CLIP_DELETE;
		edt.pClipHandle = NULL;
		edt.pData = (void*)(int*)&clipID;
		CallBackFunc(nPid,&edt);
		
	}

	//if(pClipHandle == NULL) m_listCPlayer_Base.RemoveAll();
	
	ReSortEditor(m_listCPlayer_Base);
	

	
	//UpdateEditorEverything();

	return nRc;
}

/*the pParam must be sure not NULL*/

VO_S32 CVideoEditorEngine::JumpClipTo(VOEDT_CLIP_HANDLE *pClipHandle,VO_S32 nTargetPos)
{
	CPlayerBaseCtrl *player1 = (CPlayerBaseCtrl*)pClipHandle;
	CPlayerBaseCtrl *player2;
	//POSITION pos = m_listCPlayer_Base.GetHeadPosition();
	VO_S32 pos ;
	bool bTransition;
	VO_S32 nNum = 1;
	VO_S32 nRc = FindEditorPosition(&player2,&pos,&bTransition,nTargetPos,&nNum,VOEDT_PID_CLIP_TYPE_VIDEO);
	CHECK_FAIL(nRc);

	if(player2 != player1)
	{
		POSITION pos1,pos2;
		FindListPos(m_listCPlayer_Base,player2,pos2);
		m_listCPlayer_Base.Remove(player1);
		m_listCPlayer_Base.AddBefore(pos2,player1);
		ReSortEditor(m_listCPlayer_Base);

		UpdateEditorEverything();
	}


	return nRc;
}

VO_S32 CVideoEditorEngine::ReSortEditor(CObjectList<CPlayerBaseCtrl> &listCPlayer)
{
	VO_S32 nRc = VOEDT_ERR_NONE;
#if 0
	VO_U32 duration_a = 0;
	VO_U32 duration_v = 0;
	CPlayerBaseCtrl *player = NULL;
	VOEDT_CLIP_PARAM *pParam = NULL;
	POSITION pos = listCPlayer.GetHeadPosition ();
	VO_S32 n = 1;
	VO_U32 cliptype;
	
	while (pos)
	{
		player = listCPlayer.GetNext (pos);
		if(player->m_nClipType == VOEDT_PID_CLIP_TYPE_TEXT)
		{	
			;
		}
		else
		{
			CClipPlayer *ppp = (CClipPlayer*) player;
			ppp->GetClipParam(VOEDT_PID_CLIP_TYPE,&cliptype);
			VOEDT_CLIP_PARAM *pParam;
			GetClipParam((VOEDT_CLIP_HANDLE*)ppp,VOEDT_PID_PLAYER_CLIP_PARAM,&pParam);

			if(pParam->nPlayMode == VOEDT_PID_PLAYMODE_VIDEO & VOEDT_PID_PLAYMODE_AUDIO
				|| pParam->nPlayMode ==VOEDT_PID_PLAYMODE_VIDEO )
			{	
				if(pParam->videoTransition && n!= 1)
				{
					duration_v -= pParam->videoTransition->nDuration;
					n = 2;
					if(duration_v <= 0)duration_v =0;
				}
				pParam->beginAtTimeline = duration_v;
				duration_v += pParam->point_out - pParam->point_in ;
				if( pParam->nPlayMode == VOEDT_PID_PLAYMODE_VIDEO & VOEDT_PID_PLAYMODE_AUDIO)
					duration_a += pParam->point_out - pParam->point_in ;
			}
			else if (pParam->nPlayMode ==  VOEDT_PID_PLAYMODE_AUDIO)
			{
				pParam->beginAtTimeline = duration_a;
				duration_a += pParam->point_out - pParam->point_in ;
			}

		}
		n++;
	}
#endif
	GetEditorDuration(NULL);
	return nRc;
}
VO_S32 CVideoEditorEngine::SetActionMode(VO_S32 *nMode)
{	
	m_nActionMode = *nMode;
	VOLOGI("Now ActionMode:%d", m_nActionMode);
	return VOEDT_ERR_NONE;
}
VO_S32 CVideoEditorEngine::GetActionMode(VO_S32 *nMode)
{	
	*nMode = m_nActionMode;
	return VOEDT_ERR_NONE;
}

VO_S32 CVideoEditorEngine::GetAudioOutputParam(VOEDT_OUTPUT_AUDIO_PARAM* pValue)
{
	VO_S32 nRC = VOEDT_ERR_BASE;
	if(m_bSetAudioOutputParam)
	{
		memcpy(pValue,&m_outputParam.AudioParam,sizeof(VOEDT_OUTPUT_AUDIO_PARAM));
		nRC = VOEDT_ERR_NONE;
	}
	return nRC;
}

VO_S32 CVideoEditorEngine::GetVideoOutputParam(VOEDT_OUTPUT_VIDEO_PARAM* pValue)
{
	VO_S32 nRC = VOEDT_ERR_FAILED;
	//if(m_bSetVideoOutputParam)
	//{
		memcpy(pValue,&m_outputParam.VideoParam,sizeof(VOEDT_OUTPUT_VIDEO_PARAM));
		nRC = VOEDT_ERR_NONE;
	//}
	return nRC;
}
VO_S32 CVideoEditorEngine::GetSinkOutputParam(VOEDT_OUTPUT_SINK_PARAM* pValue)
{
	VO_S32 nRC = VOEDT_ERR_FAILED;
	if(m_bSetSinkOutputParam)
	{
		memcpy(pValue,&m_outputParam.SinkParam,sizeof(VOEDT_OUTPUT_SINK_PARAM));
		nRC = VOEDT_ERR_NONE;
	}
	return nRC;
}
VO_S32 CVideoEditorEngine::SetAudioOutputParam(VOEDT_OUTPUT_AUDIO_PARAM* pValue)
{
	VO_S32 nRc = 0;
	m_encoderCtrl.UnInit(0);
	m_bSetAudioOutputParam = VO_FALSE;

	nRc = m_encoderCtrl.InitEncoder(0,this,(VOEDTENCODERCALLBACKPROC)CVideoEditorEngine::EncoderCallback);
	nRc |= m_encoderCtrl.SetEncoderParam(0,pValue);
	if(nRc == VOEDT_ERR_NONE)
	{
		memcpy(&m_outputParam.AudioParam,pValue,sizeof(VOEDT_OUTPUT_AUDIO_PARAM));
		m_bSetAudioOutputParam = VO_TRUE;

		VOEDT_CALLBACK_BUFFER edt;
		edt.pClipHandle =  NULL;
		edt.pData =(VO_VOID*) &m_outputParam.AudioParam;
		CallBackFunc(VOEDT_CB_OUTPUT_AUDIO_PARAM,&edt);
	}
	else
		m_bSetAudioOutputParam = VO_FALSE;
	return nRc;
}
VO_S32 CVideoEditorEngine::SetVideoOutputParam(VOEDT_OUTPUT_VIDEO_PARAM* pValue)
{
	pValue->nFrameRate = m_nFrameRate;
	VOLOGI("SetVideoOutpuParam %d ,%d,%d,%d,%f,%d,%d,%d,%d",pValue->nBitRate, pValue->nCodecType,pValue->nColorType,pValue->nEncQuality
																,pValue->nFrameRate,pValue->nWidth,pValue->nHeight,pValue->nKeyFrmInterval,pValue->nRotateType);
	VO_S32 nRc = VOEDT_ERR_FAILED;

	CheckWidthHeight(pValue->nWidth,pValue->nHeight);

	m_sVideoFormat_Encoder.ColorType = VO_COLOR_ARGB32_PACKED;
	m_sVideoFormat_Encoder.nWidth		= pValue->nWidth;
	m_sVideoFormat_Encoder.nHeight	= pValue->nHeight;

	VOMP_VIDEO_FORMAT vf;
	vf.Height = pValue->nHeight;
	vf.Width = pValue->nWidth;
	vf.Type = VO_COLOR_YUV_PLANAR420;
	EX_INIT_BUFFER3(vf, &m_videoOutputBuffer);

	m_encoderCtrl.UnInit(1);
	m_bSetVideoOutputParam = VO_FALSE;

	nRc = m_encoderCtrl.InitEncoder(1,this,(VOEDTENCODERCALLBACKPROC)CVideoEditorEngine::EncoderCallback);
	nRc |= m_encoderCtrl.SetEncoderParam(1,pValue);
	if(nRc == VOEDT_ERR_NONE)
	{
		memcpy(&m_outputParam.VideoParam,pValue,sizeof(VOEDT_OUTPUT_VIDEO_PARAM));
		m_bSetVideoOutputParam = VO_TRUE;

		VOEDT_CALLBACK_BUFFER edt;
		edt.pClipHandle =  NULL;
		edt.pData =(VO_VOID*) &m_outputParam.VideoParam;
		CallBackFunc(VOEDT_CB_OUTPUT_VIDEO_PARAM,&edt);
	}
	else
		m_bSetVideoOutputParam = VO_FALSE;

	return nRc;
}

VO_S32 CVideoEditorEngine::SetSinkOutputParam(VOEDT_OUTPUT_SINK_PARAM* pValue)
{
	VO_S32 nRC = VOEDT_ERR_FAILED;

	m_FileSinkCtrl.Stop();
	m_bSetSinkOutputParam = VO_FALSE;

	if(pValue->dumpFilePath[0] == '\0' ||  pValue->dumpTmpDir[0] == '\0'){
		VOLOGI("Sink File Name error");
		return nRC;
	}
	VOLOGI("SetSinkOutputParam %s ,%s,%d,",pValue->dumpFilePath, pValue->dumpTmpDir,pValue->packetType);
	if(m_bSetVideoOutputParam & m_bSetAudioOutputParam)
	{
// 		VO_S32 num = 1;
// 		CPlayerBaseCtrl *pPlayer;
// 		nRC = FindEditor(&pPlayer,&num,VOEDT_PID_CLIP_TYPE_VIDEO);
// 		CHECK_FAIL(nRC);
// 		VOEDT_AUDIO_FORMAT afa;
// 		nRC = pPlayer->GetClipParam(VOEDT_PID_AUDIO_FORMAT, &afa);
// 		CHECK_FAIL(nRC);

		VO_AUDIO_FORMAT af;
		af.Channels = m_outputParam.AudioParam.nChannels;// = afa.Channels;
		af.SampleBits = m_outputParam.AudioParam.nSampleBits;// = afa.SampleBits;
		af.SampleRate = m_outputParam.AudioParam.nSampleRate;// = afa.SampleRate;
		VOLOGI("AudioFormat: channel:%d, sampleBits:%d, SampleRate:%d", af.Channels, af.SampleBits, af.SampleRate);
		VO_VIDEO_FORMAT vf;
		vf.Height =  m_outputParam.VideoParam.nHeight;
		vf.Width  =  m_outputParam.VideoParam.nWidth;


#ifdef _WIN32
		VO_TCHAR dumpPath[255];
		VO_TCHAR dumpTmpDir[255];
		MultiByteToWideChar (CP_ACP, 0, pValue->dumpFilePath, -1, dumpPath, sizeof (dumpPath));
		MultiByteToWideChar (CP_ACP, 0, pValue->dumpTmpDir, -1, dumpTmpDir, sizeof (dumpTmpDir));
		nRC = m_FileSinkCtrl.SetFileSink(dumpPath	,dumpTmpDir ,pValue->packetType	
			,m_outputParam.AudioParam.nCodecType ,&af
			,m_outputParam.VideoParam.nCodecType ,&vf
			,m_pLibOp);
		if(!out_file_encoder)out_file_encoder = fopen("d:\\Data_Cmp\\dump_video","wb");
		if(!out_file_pcm)out_file_pcm = fopen("d:\\Data_Cmp\\dump_audio","wb");
		if(!out_file_yuv)out_file_yuv = fopen("d:\\Data_Cmp\\dump_yuv","wb");
	//	if(!out_file_ARGB32)out_file_ARGB32 = fopen("d:\\Data_Cmp\\dump_argb32","wb");
#else
		nRC = m_FileSinkCtrl.SetFileSink(pValue->dumpFilePath	,pValue->dumpTmpDir ,pValue->packetType	
				,m_outputParam.AudioParam.nCodecType ,&af
				,m_outputParam.VideoParam.nCodecType ,&vf
				,m_pLibOp);
#ifdef _DUMP
		if(!out_file_encoder)out_file_encoder = fopen("/sdcard/dump_video","wb");
		if(!out_file_pcm)out_file_pcm = fopen("/sdcard/dump_audio","wb");
		if(!out_file_yuv)out_file_yuv = fopen("/sdcard/dump_yuv","wb");
#endif
#endif

		//if(!out_file_pcm ) out_file_pcm = fopen("D:\\Data_Cmp\\aaa_PCM.pcm","wb");

		if(nRC == VOEDT_ERR_NONE)
		{
			memcpy(&m_outputParam.SinkParam,pValue,sizeof(VOEDT_OUTPUT_SINK_PARAM));
			m_bSetSinkOutputParam = VO_TRUE;

			VOEDT_CALLBACK_BUFFER edt;
			edt.pClipHandle =  NULL;
			edt.pData =(VO_VOID*) &m_outputParam.SinkParam;
			CallBackFunc(VOEDT_CB_OUTPUT_SINK_PARAM,&edt);
		}
		else
			m_bSetSinkOutputParam = VO_FALSE;
	}
	else
	{
		nRC = VOEDT_ERR_NO_INIT_AUDIOVIDEO_PARAM_FIRST;
					m_bSetSinkOutputParam = VO_FALSE;

	}

	return nRC;
}
VO_S32 CVideoEditorEngine::SetTextContent(VOEDT_CLIP_HANDLE *pClipHandle,VO_PTR pValue, VO_U32 nFlag)
{
	VO_S32 nRc = VOEDT_ERR_NONE;
	//CTextPlayer *tplayer = (CTextPlayer *)pClipHandle;//->GetVideoEditorClip();
	//nRc = tplayer->SetDataSource(VOEDT_PID_PLAYER_TEXT_URL,pValue,nFlag);
// 	VOEDT_CALLBACK_BUFFER edt;
// 	edt.pClipHandle =(VOEDT_CLIP_HANDLE*) pClipHandle;
// 	edt.pData = pValue;
// 	CallBackFunc(VOEDT_CB_TEXT_CONTENT,&edt);
	return nRc;
}
VO_S32 CVideoEditorEngine::SetTextFormat(VOEDT_CLIP_HANDLE *pClipHandle,VOEDT_TEXT_FORMAT *pFormat)
{
	return 0;

	VO_S32 nRc = VOEDT_ERR_NONE;
	CTextPlayer *tplayer = (CTextPlayer *)pClipHandle;//->GetVideoEditorClip();
	nRc = tplayer->SetClipParam(VOEDT_PID_PLAYER_TEXT_FORMAT, pFormat);

	return nRc;
}
VO_S32 CVideoEditorEngine::GetTextFormat(VOEDT_CLIP_HANDLE *pClipHandle,VOEDT_TEXT_FORMAT *pFormat)
{
	return 0;
	VO_S32 nRc = VOEDT_ERR_NONE;
	CTextPlayer *tplayer = (CTextPlayer *)pClipHandle;//->GetVideoEditorClip();
	VOEDT_TEXT_FORMAT *tf;
	nRc = tplayer->GetClipParam(VOEDT_PID_PLAYER_TEXT_FORMAT, &tf);
	memcpy(pFormat, tf,sizeof(VOEDT_TEXT_FORMAT));

	return nRc;
}

VO_S32 CVideoEditorEngine::SetClipParam(	VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 nPid,VO_VOID *pValue)
{
	VOLOGI("in SetclipParam nID :%08x",nPid);
	VO_S32 nRc = VOEDT_ERR_NONE;
	switch(nPid)
	{
	case VOEDT_PID_CLIP_PARAM:
		{
			CClipPlayer *player = (CClipPlayer*)pClipHandle;
			VOEDT_CLIP_PARAM *pCmp = (VOEDT_CLIP_PARAM* )pValue;
			if(pCmp->videoTransition )
			{
				VOLOGI("Use videoTransition");
				if(pCmp->videoTransition->pEffectFunc == NULL )
				{
					pCmp->videoTransition->pUser = &m_EffectionCtrl;
					pCmp->videoTransition->pEffectFunc = CEffectionCtrl::EffectionVideo;
					pCmp->videoTransition->pSetSize = CEffectionCtrl::SetSize;
					VOLOGI("Use Default videoTransition");
				}
				else
				{
					VOLOGI("Use User videoTransition");
				}
			}
			if(pCmp->audioTransition )
			{
				VOLOGI("Use audioTransition");
				if(pCmp->audioTransition->pEffectFunc == NULL )
				{
					pCmp->audioTransition->pUser = &m_EffectionCtrl;
					pCmp->audioTransition->pEffectFunc = CEffectionCtrl::EffectionAudio;
				}
				else
				{
					VOLOGI("Use User audioTransition");
				}
			}

			nRc = player->SetClipParam(nPid,pValue);
			CHECK_FAIL(nRc);
			nRc = player->GetClipParam(nPid,pValue);
			CHECK_FAIL(nRc);
			VOEDT_CALLBACK_BUFFER edt;
			edt.pClipHandle =(VOEDT_CLIP_HANDLE*) player;
			edt.pData = pValue;
			CallBackFunc(VOEDT_CB_CLIP_PARAM,&edt);
			ReSortEditor(m_listCPlayer_Base);

			break;
		}
	default:
		{
			VOLOGI("in SetclipParam nID :%08x",nPid);
			CPlayerBaseCtrl *player = (CPlayerBaseCtrl*)pClipHandle;
			nRc = player->SetClipParam(nPid,pValue);
			break;
		}
	}
	VOLOGI("out SetclipParam nID :%08x",nPid);
	return nRc;
}
VO_S32 CVideoEditorEngine::GetClipParam(VOEDT_CLIP_HANDLE *pClipHandle, VO_U32 nPid, VO_VOID* pValue)
{	
	CPlayerBaseCtrl *player = (CPlayerBaseCtrl*)pClipHandle;
	return player->GetClipParam(nPid, pValue);
}

VO_S32 CVideoEditorEngine::GetCallbackData(VOEDT_CALLBACK_BUFFER *pBuffer)
{
	//..
	return 0;
}
VO_S32 CVideoEditorEngine::SaveConfig(char* pDest)
{
	VOLOGI("+SaveConfig");
	VO_S32 nRc = VOEDT_ERR_NONE;

	char section[255];
	char key[255]; 
	char value[255];

	VO_U32 i = 0;
	VO_U32 counts = m_listCPlayer_Base.GetCount();

	clean_ini_file((char*)pDest);

	sprintf(section,"THUMBNAIL_FORMAT");
	VOEDT_VIDEO_FORMAT tnf;
	GetThumbNailFormat(&tnf);
	sprintf(key,"ThumbNailColorType");
	sprintf(value, "%d",tnf.ColorType);
	write_profile_string(section,key,value,pDest);
	sprintf(key,"ThumbNailHeight");
	sprintf(value, "%d",tnf.nHeight);
	write_profile_string(section,key,value,pDest);
	sprintf(key,"ThumbNailWidth");
	sprintf(value, "%d",tnf.nWidth);
	write_profile_string(section,key,value,pDest);

	sprintf(section, "CLIPCOUNTS");
	sprintf(key, "Counts");
	sprintf(value, "%d", counts);
	write_profile_string(section,key,value,(char*)pDest);

	VO_CHAR tmp[255];
	VOEDT_CLIP_PARAM param;
	VO_U32 type;
	VOEDT_VIDEO_FORMAT tf;
	POSITION pos = m_listCPlayer_Base.GetHeadPosition ();
	while (pos != NULL)
	{
		section[0]= '\0';
		key[0]= '\0';
		value[0]= '\0';
		CPlayerBaseCtrl *tmpClip = m_listCPlayer_Base.GetNext (pos);
		sprintf(section, "CLIP_%d", ++i);
		GetClipParam((VOEDT_CLIP_HANDLE*)tmpClip, VOEDT_PID_CLIP_NAME,tmp);
		sprintf(key, "ClipName");
		sprintf(value, "%s",tmp);
		write_profile_string(section,key,value,pDest);

		GetClipParam((VOEDT_CLIP_HANDLE*)tmpClip, VOEDT_PID_CLIP_TYPE,&type);
		sprintf(key, "ClipType");
		sprintf(value, "%d",type);
		write_profile_string(section,key,value,pDest);

		GetClipParam((VOEDT_CLIP_HANDLE*)tmpClip, VOEDT_PID_CLIP_PARAM,&param);
		sprintf(key, "PointIn");
		sprintf(value, "%d",param.point_in);
		write_profile_string(section,key,value,pDest);
		sprintf(key, "PointOut");
		sprintf(value, "%d",param.point_out);
		write_profile_string(section,key,value,pDest);
		sprintf(key, "BeginAtTimeline");
		sprintf(value, "%d",param.beginAtTimeline);
		write_profile_string(section,key,value,pDest);
		sprintf(key, "PlayMode");
		sprintf(value, "%d",param.nPlayMode);
		write_profile_string(section,key,value,pDest);
		sprintf(key, "Volume");
		sprintf(value, "%d",param.nVolume);
		write_profile_string(section,key,value,pDest);
		if(type == 4)
		{
/*	
			VOEDT_TEXT_FORMAT ttf;
			GetTextFormat((VOEDT_CLIP_HANDLE*)tmpClip,&ttf);

			sprintf(key, "TextSize");
			sprintf(value, "%d",ttf.nSize);
			write_profile_string(section,key,value,pDest);
			sprintf(key, "TextFont");
			sprintf(value, "%s",ttf.cFont);
			write_profile_string(section,key,value,pDest);
			sprintf(key, "TextColor");
			sprintf(value, "%d",ttf.nColor);
			write_profile_string(section,key,value,pDest);
			sprintf(key, "TextBG");
			sprintf(value, "%d",ttf.nColorBg);
			write_profile_string(section,key,value,pDest);
			*/
		}
		else
		{
			sprintf(key, "VTransition");
			sprintf(value, "%d", param.videoTransition ? 1:0);
			write_profile_string(section,key,value,pDest);
			if(param.videoTransition)
			{
				sprintf(key, "VTDuration");
				sprintf(value, "%d", param.videoTransition->nDuration);
				write_profile_string(section,key,value,pDest);
				sprintf(key, "VTType");
				sprintf(value, "%d", param.videoTransition->nType);
				write_profile_string(section,key,value,pDest);
			}
			sprintf(key, "ATransition");
			sprintf(value, "%d", param.audioTransition ? 1:0);
			write_profile_string(section,key,value,pDest);
			if(param.videoTransition)
			{
				sprintf(key, "ATDuration");
				sprintf(value, "%d", param.audioTransition->nDuration);
				write_profile_string(section,key,value,pDest);
				sprintf(key, "ATType");
				sprintf(value, "%d", param.audioTransition->nType);
				write_profile_string(section,key,value,pDest);
			}
			
			sprintf(key, "PIPLeft");
			sprintf(value, "%d",param.positionFormat.nLeft);
			write_profile_string(section,key,value,pDest);
			sprintf(key, "PIPTop");
			sprintf(value, "%d",param.positionFormat.nTop);
			write_profile_string(section,key,value,pDest);
			sprintf(key, "PIPHeight");
			sprintf(value, "%d",param.positionFormat.nHeight);
			write_profile_string(section,key,value,pDest);
			sprintf(key, "PIPWidth");
			sprintf(value, "%d",param.positionFormat.nWidth);
			write_profile_string(section,key,value,pDest);
		}
	}

	sprintf(section, "OUTPUTPARAM");
	sprintf(key, "AudioAdtsUsed");
	sprintf(value, "%d", m_outputParam.AudioParam.nAdtsUsed);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "AudioBitRate");
	sprintf(value, "%d", m_outputParam.AudioParam.nBitRate);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "AudioChannels");
	sprintf(value, "%d", m_outputParam.AudioParam.nChannels);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "AudioCodecType");
	sprintf(value, "%d", (VO_U32)m_outputParam.AudioParam.nCodecType);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "AudioSampleBits");
	sprintf(value, "%d", (VO_U32)m_outputParam.AudioParam.nSampleBits);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "AudioSampleRate");
	sprintf(value, "%d", m_outputParam.AudioParam.nSampleRate);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "VideoBitRate");
	sprintf(value, "%d", m_outputParam.VideoParam.nBitRate);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "VideoCodecType");
	sprintf(value, "%d", m_outputParam.VideoParam.nCodecType);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "VideoColorType");
	sprintf(value, "%d", m_outputParam.VideoParam.nColorType);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "VideoEncQuality");
	sprintf(value, "%d",(VO_U32)m_outputParam.VideoParam.nEncQuality);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "VideoFrameRate");
	sprintf(value, "%d", (VO_U32)(m_nFrameRate));
	write_profile_string(section,key,value,pDest);
	sprintf(key, "VideoHeight");
	sprintf(value, "%d", m_outputParam.VideoParam.nHeight);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "VideoWidth");
	sprintf(value, "%d", m_outputParam.VideoParam.nWidth);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "VideoKeyFrmInterval");
	sprintf(value, "%d", m_outputParam.VideoParam.nKeyFrmInterval);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "VideoRotateType");
	sprintf(value, "%d", m_outputParam.VideoParam.nRotateType);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "SinkFilePath");
	sprintf(value, "%s", m_outputParam.SinkParam.dumpFilePath);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "SinkTmpDir");
	sprintf(value, "%s", m_outputParam.SinkParam.dumpTmpDir);
	write_profile_string(section,key,value,pDest);
	sprintf(key, "SinkPacketType");
	sprintf(value, "%d", (VO_U32)m_outputParam.SinkParam.packetType);
	write_profile_string(section,key,value,pDest);

	VOLOGI("-SaveConfig");
	return nRc;
}

VO_S32 CVideoEditorEngine::LoadConfig(char* pSource)
{
	VOLOGI("+ LoadConfig:%s",pSource);
	//	DeleteAllClip();
	VO_S32 nRc = VOEDT_ERR_NONE;
	char section[255];
	char key[255]; 
	char strvalue[255];
	VO_U32 nvalue;

	sprintf(section,"THUMBNAIL_FORMAT");
	VOEDT_VIDEO_FORMAT tnf;
	sprintf(key,"ThumbNailColorType");
	tnf.ColorType =  read_profile_int(section,key,0,pSource);
	sprintf(key,"ThumbNailHeight");
	tnf.nHeight =  read_profile_int(section,key,0,pSource);
	sprintf(key,"ThumbNailWidth");
	tnf.nWidth =  read_profile_int(section,key,0,pSource);
	SetThumbNailFormat(&tnf);

	sprintf(section, "CLIPCOUNTS");
	sprintf(key, "Counts");
	VO_U32 counts = read_profile_int(section,key,0,pSource);
	VOLOGI("Clip counts %d ",counts);
	VOEDT_CLIP_HANDLE *pClip;
	VOEDT_CLIP_PARAM param;
	VO_U32 type = 0;
	VO_U32 i =0;
	for ( i = 0; i< counts ; ++i)
	{

		sprintf(section,"CLIP_%d",i+1);
		sprintf(key,"ClipName");
		read_profile_string(section,key,strvalue,255,"",pSource);
		VOLOGI("Add Clip %s ",(VO_CHAR*)strvalue);
		sprintf(key,"ClipType");
		type = read_profile_int(section,key,0,pSource);
		switch(type)
		{
		case VOEDT_PID_CLIP_TYPE_TEXT:	
			nRc = AddTextClip(type,(VO_VOID*)strvalue,0,&pClip);
			break;
		case VOEDT_PID_CLIP_TYPE_VIDEO:
		case VOEDT_PID_CLIP_TYPE_AUDIO:
		case VOEDT_PID_CLIP_TYPE_AUDIO| VOEDT_PID_CLIP_TYPE_VIDEO:
			nRc = AddMediaClip(type,(VO_VOID*)strvalue,0,&pClip);
			break;
		case VOEDT_PID_CLIP_TYPE_IMAGE:
			nRc = AddImgClip(type,(VO_VOID*)strvalue,0,&pClip);
			break;
		}
	
		if(nRc != VOEDT_ERR_NONE)
		{
			VOLOGI("AddClip %s Fail",(VO_CHAR*)strvalue);
			continue;
		}
		VOLOGI("Add Clip %s Succeed",(VO_CHAR*)strvalue);
		memset(&param,0,sizeof(param));

		sprintf(key,"PointIn");
		param.point_in = read_profile_int(section,key,0,pSource);
		sprintf(key,"PointOut");
		param.point_out = read_profile_int(section,key,0,pSource);
		sprintf(key,"BeginAtTimeline");
		param.beginAtTimeline = read_profile_int(section,key,0,pSource);
		sprintf(key, "PlayMode");
		param.nPlayMode = read_profile_int(section,key,0,pSource);
		VOLOGI("PlayMode:%d", param.nPlayMode);
		sprintf(key, "Volume");
		param.nVolume = read_profile_int(section,key,0,pSource);
		if(type == VOEDT_PID_CLIP_TYPE_TEXT)
		{
// 			VOEDT_TEXT_FORMAT ttf;
// 			sprintf(key,"TextSize");
// 			ttf.nSize = read_profile_int(section,key,0,pSource);
//  			sprintf(key,"TextFont");
//  			read_profile_string(section,key,ttf.cFont,255,"",pSource);
// 			sprintf(key,"TextColor");
// 			ttf.nColor = read_profile_int(section,key,0,pSource);
// 			sprintf(key,"TextBG");
// 			ttf.nColorBg = read_profile_int(section,key,0,pSource);

			//SetTextFormat(pClip,&ttf);
		}
		else
		{
			
			sprintf(key,"VTransition");
			nvalue = read_profile_int(section,key,0,pSource);
			param.videoTransition = NULL;
			if(nvalue == 1)
			{
				param.videoTransition = new VOEDT_TRANSITION_EFFECT;
				sprintf(key,"VTType");
				param.videoTransition->nType = read_profile_int(section,key,0,pSource);
				sprintf(key,"VTDuration");
				param.videoTransition->nDuration = read_profile_int(section,key,0,pSource);
				param.videoTransition->pEffectFunc = NULL;
			}
			sprintf(key,"ATransition");
			nvalue = read_profile_int(section,key,0,pSource);
			param.audioTransition = NULL;
			if(nvalue == 1)
			{
				param.audioTransition = new VOEDT_TRANSITION_EFFECT;
				sprintf(key,"ATType");
				param.audioTransition->nType = read_profile_int(section,key,0,pSource);
				sprintf(key,"ATDuration");
				param.audioTransition->nDuration = read_profile_int(section,key,0,pSource);
				param.audioTransition->pEffectFunc = NULL;
			}
		}
		
		sprintf(key,"PIPHeight");
		param.positionFormat.nHeight = read_profile_int(section,key,0,pSource);
		sprintf(key,"PIPWidth");
		param.positionFormat.nHeight = read_profile_int(section,key,0,pSource);
		sprintf(key,"PIPTop");
		param.positionFormat.nTop = read_profile_int(section,key,0,pSource);
		sprintf(key,"PIPLeft");
		param.positionFormat.nLeft = read_profile_int(section,key,0,pSource);
	
		nRc = SetClipParam(pClip, VOEDT_PID_CLIP_PARAM,&param);
		CHECK_FAIL(nRc);
		//voOS_Sleep(5000);
	}

	
	sprintf(section, "OUTPUTPARAM");
	VOEDT_OUTPUT_VIDEO_PARAM videoParam;
	sprintf(key, "VideoFrameRate");
	m_nFrameRate =  read_profile_int(section,key,0,pSource) ;//2997;
	sprintf(key, "VideoBitRate");
	videoParam.nBitRate = read_profile_int(section,key,0,pSource);
	sprintf(key, "VideoHeight");
	videoParam.nHeight = read_profile_int(section,key,0,pSource);
	sprintf(key, "VideoWidth");
	videoParam.nWidth = read_profile_int(section,key,0,pSource);
	sprintf(key, "VideoKeyFrmInterval");
	videoParam.nKeyFrmInterval = read_profile_int(section,key,0,pSource);
	sprintf(key, "VideoRotateType");
	videoParam.nRotateType = (VO_IV_RTTYPE) read_profile_int(section,key,0,pSource);
	sprintf(key, "VideoColorType");
	videoParam.nColorType = (VO_IV_COLORTYPE ) read_profile_int(section,key,0,pSource);
	sprintf(key, "VideoCodecType");
	videoParam.nCodecType =(VOEDT_VIDEO_CODINGTYPE)read_profile_int(section,key,0,pSource);//VOEDT_VIDEO_CodingH264;//VOEDT_VIDEO_CodingMPEG4;//VOEDT_VIDEO_CodingH264;//
	sprintf(key, "VideoEncQuality");
	videoParam.nEncQuality =(VO_IV_QUALITY) read_profile_int(section,key,0,pSource);

	nRc = SetVideoOutputParam(&videoParam);
	CHECK_FAIL(nRc);

	VOEDT_OUTPUT_AUDIO_PARAM AudioParam;
	sprintf(key, "AudioAdtsUsed");
	AudioParam.nAdtsUsed = read_profile_int(section,key,0,pSource);
	sprintf(key, "AudioBitRate");
	AudioParam.nBitRate = read_profile_int(section,key,0,pSource);
	sprintf(key, "AudioChannels");
	AudioParam.nChannels = read_profile_int(section,key,0,pSource);
	sprintf(key, "AudioSampleRate");
	AudioParam.nSampleRate =read_profile_int(section,key,0,pSource);
	sprintf(key, "AudioSampleBits");
	AudioParam.nSampleBits = read_profile_int(section,key,0,pSource);
	sprintf(key, "AudioCodecType");
	AudioParam.nCodecType =(VOEDT_AUDIO_CODINGTYPE)read_profile_int(section,key,0,pSource);
	nRc = SetAudioOutputParam(&AudioParam);
	CHECK_FAIL(nRc);

	VOEDT_OUTPUT_SINK_PARAM sink;
	sprintf(key, "SinkFilePath");
	read_profile_string(section,key,sink.dumpFilePath,255,"",pSource);
	sprintf(key, "SinkTmpDir");
	read_profile_string(section,key,sink.dumpTmpDir,255,"",pSource);
	sprintf(key, "SinkPacketType");
	sink.packetType =(VO_FILE_FORMAT)read_profile_int(section,key,0,pSource);
	nRc = SetSinkOutputParam(&sink);
	CHECK_FAIL(nRc);

	int actionmode = 4;
	nRc  = SetActionMode((VO_S32*)&actionmode);
	CHECK_FAIL(nRc);
	//SetEditorParam(m_hEditor,VOEDT_PID_ACTION_MODE,(void*)&actionmode);
	VOLOGI("- LoadConfig");
	return nRc;
}
