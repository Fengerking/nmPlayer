#include "RTSPSource.h"
#include "mylog.h"

#define MODULE_NAME "voFLOEngine"
#define BUILD_NO 1121
#include "voVersion.c"

namespace FLO {

VO_U32 Create(VO_HANDLE * phFLOEngine)
{
	CRTSPSource * pRTSPSource = new CRTSPSource();
	if(pRTSPSource == NULL)
		return VO_ERR_FLOENGINE_OUTOFMEMORY;
	
	*phFLOEngine = pRTSPSource;

	CDumper::SetFolder(DEFAULT_LOG_DIR);

    voShowModuleVersion(NULL);
	return VO_ERR_FLOENGINE_OK;
}

VO_U32 Destroy(VO_HANDLE hFLOEngine)
{
	if(hFLOEngine == NULL)
		return VO_ERR_FLOENGINE_NULLPOINTER;

    CRTSPSource * pRTSPSource = (CRTSPSource *)hFLOEngine;
	delete pRTSPSource;

	return VO_ERR_FLOENGINE_OK;
}

VO_U32 Open(VO_HANDLE hFLOEngine, const char * pDataSource)
{
	if(hFLOEngine == NULL)
		return VO_ERR_FLOENGINE_NULLPOINTER;
	
	CRTSPSource * pRTSPSource = (CRTSPSource *)hFLOEngine;
	return pRTSPSource->Open(pDataSource);
}

VO_U32 Close(VO_HANDLE hFLOEngine)
{
	if(hFLOEngine == NULL)
		return VO_ERR_FLOENGINE_NULLPOINTER;

	CRTSPSource * pRTSPSource = (CRTSPSource *)hFLOEngine;
	return pRTSPSource->Close();
}

VO_U32 Start(VO_HANDLE hFLOEngine)
{
	if(hFLOEngine == NULL)
		return VO_ERR_FLOENGINE_NULLPOINTER;

	CRTSPSource * pRTSPSource = (CRTSPSource *)hFLOEngine;
	return pRTSPSource->Start();
}

VO_U32 Stop(VO_HANDLE hFLOEngine)
{
	if(hFLOEngine == NULL)
		return VO_ERR_FLOENGINE_NULLPOINTER;

	CRTSPSource * pRTSPSource = (CRTSPSource *)hFLOEngine;
	return pRTSPSource->Stop();
}

VO_U32 GetParam(VO_HANDLE hFLOEngine, VO_U32 nParamID, VO_S32 * pParam)
{
	if(hFLOEngine == NULL)
		return VO_ERR_FLOENGINE_NULLPOINTER;

	CRTSPSource * pRTSPSource = (CRTSPSource *)hFLOEngine;
	return pRTSPSource->GetParam(nParamID, pParam);
}

VO_U32 SetParam(VO_HANDLE hFLOEngine, VO_U32 nParamID, VO_PTR pParam)
{
	if(hFLOEngine == NULL)
		return VO_ERR_FLOENGINE_NULLPOINTER;

	CRTSPSource * pRTSPSource = (CRTSPSource *)hFLOEngine;
	return pRTSPSource->SetParam(nParamID, pParam);
}

} //FLO

VO_S32 voGetFLOEngineAPI(VO_FLOENGINE_API * pFLOEngineAPI)
{
	pFLOEngineAPI->Create = FLO::Create;
	pFLOEngineAPI->Destroy = FLO::Destroy;
	pFLOEngineAPI->Open = FLO::Open;
	pFLOEngineAPI->Close = FLO::Close;
	pFLOEngineAPI->Start = FLO::Start;
	pFLOEngineAPI->Stop = FLO::Stop;
	pFLOEngineAPI->GetParam = FLO::GetParam;
	pFLOEngineAPI->SetParam = FLO::SetParam;

	return VO_ERR_FLOENGINE_OK;
}
