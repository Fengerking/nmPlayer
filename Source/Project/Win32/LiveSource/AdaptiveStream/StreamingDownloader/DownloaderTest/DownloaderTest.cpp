// TestProject.cpp : Defines the entry point for the console application.
//

#include "windows.h"
#include "voStreamingDownloader.h"
//#include "volog.h"
//#include "voStreamingDownloaderAPI.h"
#include "tchar.h"

typedef int (* VOGETDOWNLOADERAPI)(VO_ADAPTIVESTREAM_DOWNLOADER_API3* pVersion);
VO_S32 OnEvent(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

//int OnEvent(void* pUserData, UINT32 nID, UINT32 nParam1, UINT32 nParam2);

void* handle = 0;
bool flag = TRUE;
#define _VOLOG_ERROR
int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE h = 0;

	VO_SOURCE2_EVENTCALLBACK Callback;
//	DOWNLOADER_API_EVENTCALLBACK Callback;

	h = LoadLibrary( _T("voStreamingDownloader.dll") );
//int voGetDownloaderAPI(ADAPTIVESTREAM_DOWNLOADER_API* pHandle)

	VOGETDOWNLOADERAPI func = (VOGETDOWNLOADERAPI)GetProcAddress( h , "voGetStreamingDownloaderAPI3" );
	VO_ADAPTIVESTREAM_DOWNLOADER_API3 api;

//	VOGETDOWNLOADERAPI func = (VOGETDOWNLOADERAPI)GetProcAddress( h , "voGetDownloaderAPI" );
//	ADAPTIVESTREAM_DOWNLOADER_API api;
	func( &api );

	Callback.pUserData = (void*)&api;
	Callback.SendEvent = OnEvent;

	api.Init(&handle,&Callback, NULL);

//	VO_CHAR* ptr = "http://64.57.247.9/PlayReadyDemo/SPDemo/HLS/high/Zombieland_HLS_25FPS_Enc/Zombieland.m3u8";
//	VO_CHAR* ptr = "http://64.57.247.9/PlayReadyDemo/SPDemo/HLS/high/Avatar_HLS_25FPS_Enc/Avatar.m3u8";
//	VO_CHAR* ptr = "http://64.57.247.9/PlayReadyDemo/SPDemo/HLS/high/Salt_HLS_25FPS_Enc/Salt.m3u8";
//	VO_CHAR* ptr = "http://64.57.247.9/PlayReadyDemo/SPDemo/HLS/High/Cars2_HLS_25FPS_Enc/Cars2.m3u8";
//	VO_CHAR* ptr = "http://10.2.68.7:8082/hls/multibitrate/troy5677_clear_10min/hls.m3u8";
//	VO_CHAR* ptr = "http://qatest.visualon.com:8082/hls/Customers/Ericsson/vod6/index.m3u8";
//	VO_CHAR* ptr = "http://hls-iis.visualon.com:8082/hls/v10/gear/bipbop_16x9_variant_v10_2.m3u8";
//	char* ptr = "http://hls-iis.visualon.com/stream/hls/v8/bipbop_16x9_variant.m3u8";
//	char* ptr = "http://hls-iis.visualon.com:8082/hls/closedcaption/0926_01/1C_index.m3u8";	//drm
//	char* ptr = "http://10.2.68.7:8082/hls/customers/mdialog/vod/audio.m3u8";
	char* ptr = "http://10.2.68.7:8082/hls/v10/gear/bipbop_16x9_variant_v10_2.m3u8";
	api.Open(handle, ptr, 0, (TCHAR*)"G:\\dumpfile\\");//"http://ss-iis.visualon.com:9001/customer/ttml/Elephants_Dream_H264_720p/ElephantsDream.ism/Manifest"

	while(flag){
	}

	api.Close(handle);

	//ptr = "http://10.2.68.7:8082/hls/v10/gear/bipbop_16x9_variant_v10_2.m3u8";
	//api.Open(handle, ptr, 0, (TCHAR*)"G:\\dumpfile\\");//"http://ss-iis.visualon.com:9001/customer/ttml/Elephants_Dream_H264_720p/ElephantsDream.ism/Manifest"
	//flag = TRUE;
	//while(flag){
	//}

	//api.Close(handle);

	api.Uninit(handle);
	handle=0;
	return 0;
}

VO_S32 OnEvent(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
//int OnEvent(void* pUserData, UINT32 nID, UINT32 nParam1, UINT32 nParam2)
{
	VO_ADAPTIVESTREAM_DOWNLOADER_API3* api = (VO_ADAPTIVESTREAM_DOWNLOADER_API3*)pUserData;
//	ADAPTIVESTREAM_DOWNLOADER_API* api = (ADAPTIVESTREAM_DOWNLOADER_API*)pUserData;

	switch (nID)
	{
	case VO_EVENTID_STREAMDOWNLOADER_OPENCOMPLETE:
//	case EVENTID_INFOR_STREAMDOWNLOADER_OPENCOMPLETE:
		{
	//		api->SelectStream(handle, 0);
			int count = 0;
			count = api->GetAudioCount(handle);
			count = api->GetVideoCount(handle);
			count = api->GetSubtitleCount(handle);
			api->SelectVideo(handle, 1);
			api->SelectAudio(handle, 1);
//			api->CommitSelection(handle);
			VOOSMP_SRC_CURR_TRACK_INDEX CurrIndex;
			memset(&CurrIndex, 0x0, sizeof(VOOSMP_SRC_CURR_TRACK_INDEX));
			api->GetCurrSelectedTrackIndex(handle, &CurrIndex);
			api->GetCurrPlayingTrackIndex(handle, &CurrIndex);
			api->StartDownload(handle);
		}
		break;
//	case EVENTID_INFOR_STREAMDOWNLOADER_MANIFEST_OK:
//		break;
	case VO_EVENTID_STREAMDOWNLOADER_END:
//	case EVENTID_INFOR_STREAMDOWNLOADER_END:
		flag = FALSE;
		break;
//	case EVENTID_INFOR_STREAMDOWNLOADER_PROGRAMINFO_CHANGE:
//		break;
//	case EVENTID_INFOR_STREAMDOWNLOADER_MANIFEST_UPDATE:
//		{
////			DOWNLOADER_PROGRESS_INFO* pInfo = (DOWNLOADER_PROGRESS_INFO*)nParam1;		
//		}
//		break;
//	case EVENTID_ERROR_STREAMDOWNLOADER_DOWNLOADMANIFEST_FAIL:
//	case EVENTID_ERROR_STREAMDOWNLOADER_WRITEMANIFEST_FAIL:
//	case EVENTID_ERROR_STREAMDOWNLOADER_DOWNCHUNK_FAIL:
//	case EVENTID_ERROR_STREAMDOWNLOADER_WRITECHUNK_FAIL:
//	case EVENTID_ERROR_STREAMDOWNLOADER_SDCARD_FULL:
//		flag = FALSE;
//		break;
	}

	return 0;
}