#include "stdio.h"
#include "string.h"
#include "windows.h"
#include "voAdaptiveStreamParser.h"
#include "voAdaptiveStreamHLS.h"
#include "voLog.h"
#include "voHLSDRM.h"


#ifdef _WIN32
#include "stdafx.h"
#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

_CrtMemState s1;
_CrtMemState s2;
_CrtMemState s3;
char OutputStr[256];
#endif	//_WIN32







FILE*     pVideoDump = NULL;
FILE*     pAudioDump = NULL;

#define LIBRARY_PATH TEXT("../Debug/voAdaptiveStreamHLS.dll")

typedef VO_S32 (VO_API* GETPARSERAPI)(VO_ADAPTIVESTREAM_PARSER_API* pReadHandle);



HINSTANCE  gHandle = NULL;
HANDLE     hAdaptiveHLS = NULL;
GETPARSERAPI     gpGetParserAPI = NULL;
VO_ADAPTIVESTREAM_PARSER_API gvarParserAPI;
VO_U32   gulTestForLive = 0;
VO_U32   gulLiveIndex = 0;
VO_CHAR*          gstrTestForliveArray[] = {"C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/live1/live1.m3u8", 
"C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/live2/live1.m3u8",
"C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/live3/live1.m3u8",};


VO_CHAR*          gstrTestForLiveVariantTroy00[] = {"C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/00/01/troyThmb-00.m3u8",
                                                    "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/00/02/troyThmb-00.m3u8",
                                                    "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/00/03/troyThmb-00.m3u8",};
VO_CHAR*          gstrTestForLiveVariantTroy01[] = {"C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/01/01/troyThmb-01.m3u8",
                                                    "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/01/02/troyThmb-01.m3u8",
                                                    "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/01/03/troyThmb-01.m3u8",};
VO_CHAR*          gstrTestForLiveVariantTroy02[] = {"C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/02/01/troyThmb-02.m3u8",
                                                    "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/02/02/troyThmb-02.m3u8",
                                                    "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/02/03/troyThmb-02.m3u8",};
VO_CHAR*          gstrTestForLiveVariantTroy03[] = {"C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/03/01/troyThmb-03.m3u8",
                                                    "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/03/02/troyThmb-03.m3u8",
                                                    "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/03/03/troyThmb-03.m3u8",};

VO_CHAR*          gstrTestForLiveJumpAndRollback[] = {"C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/live_jump_rollback/live1/live1.m3u8",
                                                      "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/live_jump_rollback/live2/live1.m3u8",
                                                      "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/live_jump_rollback/live3/live1.m3u8",};

VO_S32 SendEventCallback(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
VO_U32 CheckItemInfo(VO_ADAPTIVESTREAMPARSER_CHUNK*    pChunck);
VO_CHAR*    ConvertURLForLive(VO_CHAR*   pURL);

HINSTANCE LoadLib(TCHAR* tPathLib)
{
	return ::LoadLibrary(tPathLib);
}

VO_ADAPTIVESTREAM_PLAYLISTDATA   gsAdaptivePlaylistData;
VO_BYTE                          gWorkBuffer[65536];

VO_VOID    LoadPlaylist(VO_CHAR*   pstrPlaylistURL);
VO_VOID    FreePlaylist(VO_ADAPTIVESTREAM_PLAYLISTDATA*  pAdaptivePlaylistData);
VO_VOID    LoadHLS();

int   main()
{
    VO_U64   ullDuration = 0;
	VO_U32   ulRet = 0;
	VO_U8    sps[] = {0x00, 0x00, 0x01, 0x27, 0x4D, 0x40, 0x1E, 0xE8, 
		              0x80, 0x5A, 0x12 ,0x6C, 0x09, 0xA8, 0x30, 0x30, 
					  0x37, 0x80, 0x00, 0x02, 0x58, 0x00, 0x00, 0x75, 
					  0x30, 0x74, 0x40, 0x01, 0x18, 0xC0, 0x00, 0xBE, 
					  0xE9, 0x7B, 0xDC, 0x05, 0x00 };
	VO_U8    pps[] = {0x00,0x00,0x01,0x28,0xfa, 0x40, 0xdc, 0x80, 0x00};
	FILE*    pFileDump = fopen("C:/Personal/H264Dump.dat", "wb");
	fwrite(sps,1,37, pFileDump);
	fwrite(pps, 1, 9, pFileDump);
	fclose(pFileDump);

	

#ifdef _WIN32
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtMemCheckpoint( &s1 );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
#endif	//_WIN32

	LoadHLS();

	VO_SOURCE2_PROGRAM_INFO*    pProgramInfo = NULL;

	VO_SOURCE2_EVENTCALLBACK    varEventCallback;
	varEventCallback.pUserData = NULL;
	varEventCallback.SendEvent = SendEventCallback;

	VO_ADAPTIVESTREAMPARSER_CHUNK*    pChunck = NULL;
    VO_ADAPTIVESTREAMPARSER_CHUNK*    pChunckAudio = NULL;
    VO_DRM2_HSL_PROCESS_INFO*         pDrmInfo = NULL;
    VO_U32                            ulGetAudio = 0;

	VO_CHAR*          strSingleManifest = "C:/Personal/work/HSL_Test_Clips/troyThmb_clear/troyThmb-00.m3u8";
	VO_CHAR*          strMainManifest = "C:/Personal/work/HSL_Test_Clips/troyThmb_clear/troyThmb.m3u8";
	VO_CHAR*          strTestForLive = "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/live1.m3u8";
    VO_CHAR*          strVariantForLive = "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/troyThmb.m3u8";
    VO_CHAR*          strMultiLanguage  = "./TestManifest/v8/bipbop_16x9_variant.m3u8";
	VO_CHAR*          strSingle  = "./TestManifest/bipball/10C_192x144_208K.m3u8";
	VO_CHAR*          strDiscritx  = "./TestManifest/JohnnyEnglish_480P_1400Kbps.m3u8";
	VO_CHAR*          strTroy = "./TestManifest/troy/hls.m3u8";
	VO_CHAR*          strvideoonly = "./TestManifest/videoonly/FamiltyGuyPlaylist.m3u8";
    VO_CHAR*          strMdialog = "C:/Personal/work/task/new_feature/mdialog/pre_mid_post_roll_1.m3u8";
	VO_CHAR*          strbipball = "C:/Personal/work/HSL_Test_Clips/Bipball/bipbop_16x9_variant.m3u8";
	VO_CHAR*          strDrm = 	"C:/Personal/work/HSL_Test_Clips/Drm/500_n.m3u8";
	VO_CHAR*          strDrmOriginal = 	"C:/Personal/work/HSL_Test_Clips/Drm/500_original.m3u8";
    VO_CHAR*          strLiveJumpAndRollback = 	"C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/live_jump_rollback/live1.m3u8";

	FILE*             pFile = NULL;
    VO_U32            ulChangeTime = 16;
	VO_U32            ulChangeTime1 = 20;

    VO_U32            ulSeekPoint = 10;
	VO_U32            ulItemCount = 0;
	VO_U64            ullSeekTime = 1800016;
    VO_U64            ullPos = 0;
    VO_U32            ulSequenceId = 0;
    VO_U32            ulEndCount = 175;

	if(gvarParserAPI.Init == NULL)
	{
		return 0;
	}

    //The Flag 1 For Live;
    gulTestForLive = 0;
	LoadPlaylist(strMultiLanguage);


	gvarParserAPI.Init(&hAdaptiveHLS, &gsAdaptivePlaylistData, &varEventCallback);
    gvarParserAPI.Open(hAdaptiveHLS);

    gvarParserAPI.GetProgramInfo(hAdaptiveHLS, 0, &pProgramInfo);

	gvarParserAPI.SelectStream(hAdaptiveHLS, pProgramInfo->ppStreamInfo[0]->uStreamID);


    Sleep(500);
	ulRet = gvarParserAPI.GetChunk(hAdaptiveHLS, VO_ADAPTIVESTREAMPARSER_CHUNKTYPE_VIDEO, &pChunck);
    ulGetAudio = gvarParserAPI.GetChunk(hAdaptiveHLS, VO_ADAPTIVESTREAMPARSER_CHUNKTYPE_AUDIO, &pChunckAudio);
	ulItemCount = 1;
	while (ulRet == 0)
	{
        ulRet = gvarParserAPI.GetChunk(hAdaptiveHLS, VO_ADAPTIVESTREAMPARSER_CHUNKTYPE_VIDEO, &pChunck);
		ulGetAudio = gvarParserAPI.GetChunk(hAdaptiveHLS, VO_ADAPTIVESTREAMPARSER_CHUNKTYPE_AUDIO, &pChunckAudio);

        if(ulRet == 0)
        {
			pDrmInfo = (VO_DRM2_HSL_PROCESS_INFO*)(pChunck->pChunkDRMInfo);
			ulSequenceId = 0;
			if(pDrmInfo != NULL)
			{
				ulSequenceId = pDrmInfo->uSequenceNum;
			}
			VOLOGI("the video ret url:%s, the sequence id:%d", pChunck->szUrl, ulSequenceId);
	    }
		else
		{
			VOLOGI("the return value %d", ulRet);
		    switch(ulRet)
			{
			    case VO_RET_SOURCE2_END:
			    {
					VOLOGI("the stream is end!");
					break;
				}
				case VO_RET_SOURCE2_NEEDRETRY:
			    {
				    VOLOGI("need retry!");
					break;
				}
			}
		}

		if(ulGetAudio == 0)
		{
			VOLOGI("the video ret url:%s, the sequence id:%d", pChunckAudio->szUrl);
		}

		ulItemCount++;
		//ulRet = gvarParserAPI.GetChunk(hAdaptiveHLS, VO_ADAPTIVESTREAMPARSER_CHUNKTYPE_AUDIO, &pChunckAudio);
		//if(ulRet == 0)
		//{
		//	VOLOGI("the audio ret url:%s", pChunckAudio->szUrl);
		//}
		/*
 
		if(ulItemCount == ulChangeTime)
		{
			gvarParserAPI.SelectStream(hAdaptiveHLS, pProgramInfo->ppStreamInfo[3]->uStreamID);
		}
		*/


        if(ulItemCount == ulChangeTime)
		{
			gvarParserAPI.SelectTrack(hAdaptiveHLS, pProgramInfo->ppStreamInfo[0]->ppTrackInfo[1]->uTrackID);
		}

		if(ulItemCount == ulChangeTime1)
		{
			gvarParserAPI.SelectTrack(hAdaptiveHLS, pProgramInfo->ppStreamInfo[0]->ppTrackInfo[0]->uTrackID);
		}

		if(ulItemCount == ulEndCount)
		{
			int skdjksdjf = 0;
		}

		/*
		if(ulItemCount == ulSeekPoint)
		{
		    gvarParserAPI.Seek(hAdaptiveHLS, &ullSeekTime);
		}
		*/
		//Sleep(50);
	}

	//gvarParserAPI.Seek(hAdaptiveHLS, &ullPos);

    gvarParserAPI.Close(hAdaptiveHLS);
	gvarParserAPI.GetDuration(hAdaptiveHLS, &ullDuration);
	gvarParserAPI.UnInit(hAdaptiveHLS);

#ifdef WIN32
	memset(OutputStr, 0, 256);
	_CrtMemCheckpoint( &s2 );
	if ( _CrtMemDifference( &s3, &s1, &s2) )
		_CrtMemDumpStatistics( &s3 );
	sprintf(OutputStr,"ThreadID: %d\n",GetCurrentThreadId());
	OutputDebugStringA(OutputStr);
#endif


	return 0;
}



VO_VOID    LoadPlaylist(VO_CHAR*   pstrPlaylistURL)
{
	VO_S32       iLen = 0;
	VO_U32       ulIndex = 0;
	FILE*        pFile = NULL;
	VO_CHAR*     pURLConvert = NULL;

    if(pstrPlaylistURL == NULL  || strlen(pstrPlaylistURL) == 0 )
	{
	    return;
	}


    if(gulTestForLive == 1)
	{
		memset(gWorkBuffer, 0, 65536);
        pURLConvert = ConvertURLForLive(pstrPlaylistURL);
		if(pURLConvert == NULL)
		{
		    return;
		}

		pFile = fopen(pURLConvert, "rb");
		if(pFile == NULL)
		{
			return;
		}

		memset(&gsAdaptivePlaylistData, 0, sizeof(VO_ADAPTIVESTREAM_PLAYLISTDATA ));
		iLen = fread(gWorkBuffer+ulIndex, 1, 1024, pFile);
		while(iLen > 0)
		{
			ulIndex += iLen;
			iLen = fread(gWorkBuffer+ulIndex, 1, 1024, pFile);
		}
	}
	else
	{
		memset(gWorkBuffer, 0, 65536);
		pFile = fopen(pstrPlaylistURL, "rb");
		if(pFile == NULL)
		{
			return;
		}

		memset(&gsAdaptivePlaylistData, 0, sizeof(VO_ADAPTIVESTREAM_PLAYLISTDATA ));
		iLen = fread(gWorkBuffer+ulIndex, 1, 1024, pFile);
		while(iLen > 0)
		{
			ulIndex += iLen;
			iLen = fread(gWorkBuffer+ulIndex, 1, 1024, pFile);
		}
	}

	gsAdaptivePlaylistData.pData = gWorkBuffer;
	memcpy(gsAdaptivePlaylistData.szUrl, pstrPlaylistURL, strlen(pstrPlaylistURL));
	memcpy(gsAdaptivePlaylistData.szNewUrl, pstrPlaylistURL, strlen(pstrPlaylistURL));
	gsAdaptivePlaylistData.uDataSize = ulIndex;
	fclose(pFile);
}

VO_VOID    FreePlaylist(VO_ADAPTIVESTREAM_PLAYLISTDATA*  pAdaptivePlaylistData)
{
    return;
}



VO_S32 SendEventCallback (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	VO_ADAPTIVESTREAM_PLAYLISTDATA*   pParam = (VO_ADAPTIVESTREAM_PLAYLISTDATA*)(nParam1);
    switch(nID)
	{
	    case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_NEEDPARSEITEM:
	    {
			LoadPlaylist(pParam->szUrl);
            pParam->pData = gsAdaptivePlaylistData.pData;
			pParam->uDataSize = gsAdaptivePlaylistData.uDataSize;
            memset(pParam->szNewUrl, 0, 2048);
			memcpy(pParam->szNewUrl, pParam->szUrl, strlen(pParam->szUrl));
		}
	}

	return 0;
}

VO_U32 CheckItemInfo(VO_ADAPTIVESTREAMPARSER_CHUNK*    pChunck)
{
	FILE*  pFile = NULL;
	if(pChunck == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	pFile = fopen(pChunck->szUrl, "rb");
	if(pFile != NULL)
	{
		fclose(pFile);
		return 0;
	}

	return VO_RET_SOURCE2_OUTPUTNOTFOUND;
}


VO_VOID    LoadHLS()
{
	gHandle = LoadLib(LIBRARY_PATH);
	gpGetParserAPI = (GETPARSERAPI)::GetProcAddress(gHandle, "voGetAdaptiveStreamHLSAPI");    
	gpGetParserAPI(&gvarParserAPI);
}


VO_CHAR*    ConvertURLForLive(VO_CHAR*   pURL)
{
    VO_CHAR*    pURLRet = pURL;
    if(memcmp(pURL, "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/00/troyThmb-00.m3u8", strlen(pURL)) == 0)
	{
		if(gulLiveIndex >= 3)
		{
			return NULL;
		}
	    pURLRet = gstrTestForLiveVariantTroy00[gulLiveIndex];
		gulLiveIndex++;
	}
	else if (memcmp(pURL, "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/01/troyThmb-01.m3u8", strlen(pURL)) == 0)
	{
		if(gulLiveIndex >= 3)
		{
			return NULL;
		}		
		pURLRet = gstrTestForLiveVariantTroy01[gulLiveIndex];
		gulLiveIndex++;
	}
	else if(memcmp(pURL, "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/02/troyThmb-02.m3u8", strlen(pURL)) == 0)
	{
		if(gulLiveIndex >= 3)
		{
			return NULL;
		}
        pURLRet = gstrTestForLiveVariantTroy02[gulLiveIndex];
		gulLiveIndex++;
	}
	else if(memcmp(pURL, "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/livevariant/03/troyThmb-03.m3u8", strlen(pURL)) == 0)
	{
		if(gulLiveIndex >= 3)
		{
			return NULL;
		}
		pURLRet = gstrTestForLiveVariantTroy03[gulLiveIndex];
		gulLiveIndex++;
	}
	else if(memcmp(pURL, "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/live1.m3u8", strlen(pURL)) == 0)
	{
		if(gulLiveIndex >= 3)
		{
			return NULL;
		}	
		pURLRet = gstrTestForliveArray[gulLiveIndex];
		gulLiveIndex++;
	}
	else if(memcmp(pURL, "C:/Personal/work/task/new_hls_parser/hls_parser/hls_parser/live/live_jump_rollback/live1.m3u8", strlen(pURL)) == 0)
	{
		if(gulLiveIndex >= 3)
		{
			return NULL;
		}	
		pURLRet = gstrTestForLiveJumpAndRollback[gulLiveIndex];
		gulLiveIndex++;
	}

	return pURLRet;
}