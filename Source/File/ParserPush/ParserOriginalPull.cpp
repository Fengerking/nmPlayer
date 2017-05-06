#include "ParserOriginalPull.h"
#include "voLog.h"
#include "voOSFunc.h"

typedef VO_S32 (VO_API *pvoGetReadAPI)(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag);

//bool g_bSave = true;
//FILE *pSave = NULL;


VO_PTR VO_API stream_open(VO_FILE_SOURCE * pSource)
{
	VOLOGI("stream_open");

	CParserOriginalPull * pFPaser = (CParserOriginalPull *)pSource->nReserve;

	return pFPaser->getStream();
}

//unsigned g_ui = 0;
//int iName = 0;
VO_S32 VO_API stream_read(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
	//DWORD wok = GetCurrentThreadId();

	//VOLOGI("threadid %ld", wok);

	CStreamCache * pStream = (CStreamCache*)pFile;
	int ii = pStream->read( (VO_PBYTE)pBuffer , uSize );
	//if (g_bSave)
	//{
	//	VOLOGI("read from %X ", g_ui);
	//	g_ui += uSize;
	//	long ll = ftell(pSave);
	//	VOLOGI("by %X to %X", uSize, g_ui);
	//	VOLOGI("\treal from %lX by %X", ll, ii);
	//	size_t iRet = fwrite(pBuffer, 1, ii, pSave);
	//	if (0 != fflush(pSave))
	//		VOLOGI("\n!fflush!\n");
	//}
	//VOLOGI("real to %x", ii);

	return ii;
}

VO_S32 VO_API stream_write(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
	VOLOGI("stream_write");

	return -1;
}

VO_S64 VO_API stream_seek(VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag)
{
	//DWORD wok = GetCurrentThreadId();
	//VOLOGI("threadid %ld", wok);
	//switch (uFlag)
	//{
	//case 0:
	//	g_ui = nPos;
	//	break;
	//case 1:
	//	g_ui += nPos;
	//case 2:
	//	g_ui = g_ui;
	//}

	//if (g_bSave)
	//	fseek(pSave, nPos, uFlag);

	CStreamCache * pStream = (CStreamCache*)pFile;

	return pStream->seek( nPos , uFlag );
}

VO_S32 VO_API stream_flush(VO_PTR pFile)
{
	VOLOGI("stream_flush");

	return -1;
}

VO_S64 VO_API stream_size(VO_PTR pFile)
{
	VOLOGI("stream_size");

	CStreamCache * pStream = (CStreamCache*)pFile;

	return pStream->FileSize();
}

VO_S64 VO_API stream_save(VO_PTR pFile)
{
	VOLOGI("stream_save");

	return -1;
}

VO_S32 VO_API stream_close(VO_PTR pFile)
{
	VOLOGI("stream_close");
	//g_ui = 0;
	//if (g_bSave)
	//	fseek(pSave, 0, 0);

	CStreamCache * pStream = (CStreamCache*)pFile;

	return pStream->seek(0 , VO_FILE_BEGIN);
}

VO_U32 GetSample(VO_PTR params) {
	TrackThreadInfo* info = static_cast<TrackThreadInfo*>(params);

	CParserOriginalPull* pParser = static_cast<CParserOriginalPull*>(info->pSelf);
	pParser->thread_GetSample(info);

	return 0;
}


CParserOriginalPull::CParserOriginalPull(void)
	: m_parser_handle(NULL)
	, m_nLastTimeStamp(0)
	, m_nAddendTimeStamp(0)
	, m_bRunning(VO_FALSE)
	, m_bInited(VO_FALSE)
{
	memset( &m_parser_init_info, 0, sizeof(VO_PARSER_INIT_INFO) );
	memset( &m_track_thread_info[0], 0, sizeof(TrackThreadInfo) );
	memset( &m_track_thread_info[1], 0, sizeof(TrackThreadInfo) );

	m_opFile.Close	= stream_close;
	m_opFile.Flush	= stream_flush;
	m_opFile.Open	= stream_open;
	m_opFile.Read	= stream_read;
	m_opFile.Save	= stream_save;
	m_opFile.Seek	= stream_seek;
	m_opFile.Size	= stream_size;
	m_opFile.Write	= stream_write;
}


CParserOriginalPull::~CParserOriginalPull(void)
{
}

VO_U32 CParserOriginalPull::Open(VO_PARSER_INIT_INFO *pParma)
{
	m_bRunning = VO_TRUE;
	m_bInited = VO_FALSE;

	m_Stream.open();
	
	if (pParma)
		memcpy( &m_parser_init_info, pParma, sizeof(VO_PARSER_INIT_INFO) );

	return 0;
}

VO_U32 CParserOriginalPull::Close()
{
	VOLOGF();

	m_bRunning = VO_FALSE;
	m_Stream.close();
	while (m_track_thread_info[0].Handle || m_track_thread_info[1].Handle)
		m_event.Wait(700);

	if (m_parser_handle)
	{
		m_parser_api.Close(m_parser_handle);
		m_dlEngine.FreeLib();
		m_parser_handle = NULL;

		memset( &m_parser_api, 0, sizeof(VO_SOURCE_READAPI) );
	}

	return 0;
}

VO_U32 CParserOriginalPull::Reset()
{
	VOLOGF();

	m_Stream.setFileEnd();

	m_event.Reset();
	while (!m_bInited || m_track_thread_info[0].Handle || m_track_thread_info[1].Handle)
		m_event.Wait(700);

	if (m_parser_handle)
	{
		m_parser_api.Close(m_parser_handle);
		m_dlEngine.FreeLib();
		m_parser_handle = NULL;

		memset( &m_parser_api, 0, sizeof(VO_SOURCE_READAPI) );
	}

	return 0;
}

VO_U32 CParserOriginalPull::Process(VO_PARSER_INPUT_BUFFER* pBuffer)
{
	return m_Stream.write(pBuffer->pBuf, pBuffer->nBufLen);
}

VO_U32 CParserOriginalPull::SetParam(VO_U32 uID, VO_PTR pParam)
{
	return m_parser_api.SetSourceParam(&m_parser_handle, uID, pParam);
}

VO_U32 CParserOriginalPull::GetParam(VO_U32 uID, VO_PTR pParam)
{
	return m_parser_api.GetSourceParam(&m_parser_handle, uID, pParam);
}

VO_U32 CParserOriginalPull::setDLL(VO_TCHAR *szDllFile, VO_TCHAR *szAPIName)
{
	vostrcpy(m_dlEngine.m_szDllFile, szDllFile);
	vostrcpy(m_dlEngine.m_szAPIName, szAPIName);

	if(m_dlEngine.LoadLib(NULL) == 0) {
		VOLOGE("!LoadLib ERR");
		return VO_FALSE;
	}

	pvoGetReadAPI getapi = (pvoGetReadAPI)m_dlEngine.m_pAPIEntry;
	if (getapi)
	{
		getapi(&m_parser_api , 0);

		return ThreadStart();
	}
	else
	{
		VOLOGI("!getapi");
		return VO_FALSE;
	}

	return 0;
}

void CParserOriginalPull::ThreadMain()
{
	VOLOGF();

	VO_FILE_SOURCE source;
	source.nFlag	= 0x102;//0x00020101;
	source.pSource	= NULL;
	source.nReserve = (VO_U32)this;

	VO_SOURCE_OPENPARAM param	= {0};//TODO
	param.nFlag					= source.nFlag;
	param.pSource				= &source;
	param.pSourceOP				= &m_opFile;
	//param.pDrmCB				= m_source_param.pDrmCB;
	//param.pMemOP				= m_source_param.pMemOP;

	if ( VO_ERR_NONE != m_parser_api.Open(&m_parser_handle, &param) ) {
		VOLOGI("!m_parser_api.Open");
		return;
	}
	else
		VOLOGI("m_parser_api.Open OK");

	// -----------------------------------------------
	VO_SOURCE_INFO sourceinfo = {0};
	if (VO_ERR_NONE != m_parser_api.GetSourceInfo(m_parser_handle, &sourceinfo)) {
		VOLOGI("!m_parser_api.GetSourceInfo");
		return;
	}
	else
		VOLOGI("m_parser_api.GetSourceInfo OK");

	VO_PARSER_STREAMINFO streaminfo = {0};

	m_track_thread_info[0].pSelf	= this;
	m_track_thread_info[0].nTrack	= 0;
	m_track_thread_info[1].pSelf	= this;
	m_track_thread_info[1].nTrack	= 1;

	VO_BOOL bVideo = VO_FALSE, bAudio = VO_FALSE;

	VO_SOURCE_TRACKINFO trackinfo[2];
	/*memset( &trackinfo0, 0, sizeof(VO_SOURCE_TRACKINFO) );*/
	for (VO_S32 i = 0; i < sourceinfo.Tracks; i++)
	{
		if ( VO_ERR_NONE != m_parser_api.GetTrackInfo(m_parser_handle, i, &trackinfo[i]) )
		{
			VOLOGI("!m_parser_api.GetTrackInfo");
			return;
		}
		else
		{
			if (VO_SOURCE_TT_VIDEO == trackinfo[i].Type)
			{
				m_track_thread_info[i].Type = VO_PARSER_OT_VIDEO;

				bVideo = VO_TRUE;

				streaminfo.nVideoCodecType	= trackinfo[i].Codec;
				streaminfo.pVideoExtraData	= trackinfo[i].HeadData;
				streaminfo.nVideoExtraSize	= trackinfo[i].HeadSize;
			}

			if (VO_SOURCE_TT_AUDIO == trackinfo[i].Type)
			{
				m_track_thread_info[i].Type = VO_PARSER_OT_AUDIO;

				bAudio = VO_TRUE;

				streaminfo.nAudioCodecType	= trackinfo[i].Codec;
				streaminfo.pAudioExtraData	= trackinfo[i].HeadData;
				streaminfo.nAudioExtraSize	= trackinfo[i].HeadSize;
			}
		}
	}

	VO_S32 iMediaType = -1;
	if (bVideo && bAudio)
		iMediaType = VO_MEDIA_AUDIO_VIDEO;
	else if (bAudio)
		iMediaType = VO_MEDIA_PURE_AUDIO;
	else if (bVideo)
		iMediaType = VO_MEDIA_PURE_VIDEO;

	VO_PARSER_OUTPUT_BUFFER output_buf = {0};
	output_buf.nStreamID	= 0;
	output_buf.nType		= VO_PARSER_OT_MEDIATYPE;
	output_buf.pOutputData	= &iMediaType;
	output_buf.pUserData	= m_parser_init_info.pUserData;
	output_buf.pReserved	= NULL;

	m_parser_init_info.pProc(&output_buf);
	VOLOGI("m_parser_init_info.pProc VO_PARSER_OT_MEDIATYPE");

	memset( &output_buf, 0, sizeof(VO_PARSER_OUTPUT_BUFFER) );
	output_buf.nType		= VO_PARSER_OT_STREAMINFO;
	output_buf.pOutputData	= &streaminfo;
	output_buf.pUserData	= m_parser_init_info.pUserData;
	VOLOGI("+m_parser_init_info.pProc VO_PARSER_OT_STREAMINFO");
	m_parser_init_info.pProc(&output_buf);
	VOLOGI("-m_parser_init_info.pProc VO_PARSER_OT_STREAMINFO");

	//---------------------------------------
	for (VO_S32 i = 0; i < sourceinfo.Tracks; i++)
	{
		VO_U32 ID = 0;

		voThreadCreate(&m_track_thread_info[i].Handle, &ID, GetSample, &m_track_thread_info[i], 0);
		VOLOGI("Create Thread GetASample");
	}

	m_bInited = VO_TRUE;
}

VO_VOID CParserOriginalPull::thread_GetSample(TrackThreadInfo *pInfo)
{
	VOLOGF();

	VO_PARSER_OUTPUT_BUFFER output_buf = {0};

	output_buf.nStreamID	= 0;
	output_buf.nType		= pInfo->Type;
	output_buf.pUserData	= m_parser_init_info.pUserData;
	output_buf.pReserved	= NULL;

	do {
		VO_SOURCE_SAMPLE sample = {0};
		VO_MTV_FRAME_BUFFER MTVFrameBuffer = {0};

		VO_U32 uiRet = m_parser_api.GetSample(m_parser_handle, pInfo->nTrack, &sample);
		if (!m_bRunning || 0 != uiRet)
		{
			memset( pInfo, 0, sizeof(TrackThreadInfo) );

			m_event.Signal();

			return;
		}

		MTVFrameBuffer.pData		= sample.Buffer;
		MTVFrameBuffer.nSize		= sample.Size;
		MTVFrameBuffer.nStartTime	= sample.Time + m_nAddendTimeStamp;
		//MTVFrameBuffer.nEndTime	= sample.Time + sample.Duration + m_nAddendTimeStamp;

		m_nLastTimeStamp = MTVFrameBuffer.nStartTime;

		output_buf.pOutputData = &MTVFrameBuffer;

		m_parser_init_info.pProc(&output_buf);
	} while(1);
}

VO_S64 CParserOriginalPull::getLastTimeStamp()
{
	return m_nLastTimeStamp;
}

VO_VOID CParserOriginalPull::setAddEndTimeStamp(VO_S64 ll)
{
	m_nAddendTimeStamp = ll;
}