#include "voString.h"
#include "voSource2SourceAdaptiveStreaming.h"
#include "voSource2SourceOther.h"
#include "SourceInfoXMLDumper.h"
#include "SourceDataDumper.h"

#define MOUDLENAME _T("voAdaptiveStreamController")
#define APINAME _T("voGetAdaptiveStreamControllerAPI")
#define TESTURL "http://dash-iis.visualon.com:9001/dash/envivio/test3_baseline_timeline/manifest.mpd"
#define DUMPDIR "D:\\"

VO_SOURCE2_EVENTCALLBACK g_eventcb;

VO_S32 SendEvent(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

int _tmain(int argc, _TCHAR* argv[])
{
	g_eventcb.pUserData = 0;
	g_eventcb.SendEvent = SendEvent;

	ISource2Interface * ptr_obj = new voSource2SourceAdaptiveStreaming( MOUDLENAME , APINAME );

	ptr_obj->Init( TESTURL , VO_SOURCE2_FLAG_OPEN_URL , 0 );

	ptr_obj->SetParam( VO_PID_SOURCE2_EVENTCALLBACK , &g_eventcb );

	ptr_obj->Open();

	{
		SourceInfoXMLDumper xmldump( DUMPDIR , TESTURL , ptr_obj );
		xmldump.Dump();
	}

	ptr_obj->Start();

	{
		SourceDataDumper datadump( DUMPDIR , TESTURL , ptr_obj );
		datadump.Dump();
	}

	ptr_obj->Stop();

	ptr_obj->Close();

	ptr_obj->Uninit();

	delete ptr_obj;

	return 0;
}

VO_S32 SendEvent(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	return 0;
}