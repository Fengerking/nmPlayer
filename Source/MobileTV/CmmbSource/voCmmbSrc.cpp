#include "voSource2.h"
#include "voOnStreamType.h"
#include "CmmbSource.h"

#if defined __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}
#endif

VO_U32 VO_API voCMMBSrcInit(VO_HANDLE * phHandle, VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam)
{
  if (!phHandle)
    return VOOSMP_ERR_Pointer;

  CCMMBSource* pSrc = new CCMMBSource();
  *phHandle = (VO_HANDLE)pSrc;
  return pSrc->Init(pSource, nFlag, pInitParam);
}

VO_U32 VO_API voCMMBSrcUninit(VO_HANDLE hHandle)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->Uninit();
}

VO_U32 VO_API voCMMBSrcOpen(VO_HANDLE hHandle)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->Open();
}

VO_U32 VO_API voCMMBSrcClose(VO_HANDLE hHandle)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->Close();
}

VO_U32 VO_API voCMMBSrcStart(VO_HANDLE hHandle)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->Start();
}

VO_U32 VO_API voCMMBSrcPause(VO_HANDLE hHandle)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->Pause();
}

VO_U32 VO_API voCMMBSrcStop(VO_HANDLE hHandle)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->Stop();
}

VO_U32 VO_API voCMMBSrcSeek(VO_HANDLE hHandle, VO_U64* pTimeStamp)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->Seek(pTimeStamp);
}

VO_U32 VO_API voCMMBSrcGetDuration(VO_PTR hHandle, VO_U64 * pDuration)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->GetDuration(pDuration);
}

VO_U32 VO_API voCMMBSrcGetSample(VO_HANDLE hHandle , VO_SOURCE2_TRACK_TYPE nTrackType , VO_PTR pSample)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->GetSample(nTrackType, pSample);
}

VO_U32 VO_API voCMMBSrcGetProgramCount(VO_HANDLE hHandle, VO_U32 *pProgramCount)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->GetProgramCount(pProgramCount);
}

VO_U32 VO_API voCMMBSrcGetProgramInfo(VO_HANDLE hHandle, VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->GetProgramInfo(nProgram, pProgramInfo);
}

VO_U32 VO_API voCMMBSrcGetCurTrackInfo(VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->GetCurTrackInfo(nTrackType, ppTrackInfo);
}

VO_U32 VO_API voCMMBSrcSelectProgram(VO_HANDLE hHandle, VO_U32 nProgram)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->SelectProgram(nProgram);
}

VO_U32 VO_API voCMMBSrcSelectStream(VO_HANDLE hHandle, VO_U32 nStream)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->SelectStream(nStream);
}

VO_U32 VO_API voCMMBSrcSelectTrack(VO_HANDLE hHandle, VO_U32 nTrack)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->SelectTrack(nTrack);
}

VO_U32 VO_API voCMMBSrcGetDRMInfo(VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->GetDRMInfo(ppDRMInfo);
}

VO_U32 VO_API voCMMBSrcSendBuffer(VO_HANDLE hHandle, const VO_SOURCE2_SAMPLE& buffer)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->SendBuffer(buffer);
}

bool voSendData(VO_HANDLE hHandle, unsigned char* pData, unsigned int nDataLen)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->SendData(pData, nDataLen);
}

VO_U32 VO_API voCMMBSrcGetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
  VOLOGI("Start voCMMBSrcGetParam");
  if (nParamID == 0x12344321)
  {
    int* addr = (int*)pParam;
    VOLOGI("Start Hook!");
    *addr = (int)voSendData;
    VOLOGI("End Hook!, 0x%X, 0x%X", addr, *addr);
    return VOOSMP_ERR_None;
  }
  else
  {
    CCMMBSource* pSrc = (CCMMBSource*)hHandle;
    return pSrc->GetParam(nParamID, pParam);
  }
}

VO_U32 VO_API voCMMBSrcSetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
  CCMMBSource* pSrc = (CCMMBSource*)hHandle;
  return pSrc->SetParam(nParamID, pParam);
}

int VO_API voGetCMMBSrcApi(VO_SOURCE2_API* pFuncSet)
{
  if(pFuncSet == 0)
    return VOOSMP_ERR_Pointer;

  pFuncSet->Close = voCMMBSrcClose;
  pFuncSet->GetCurTrackInfo = voCMMBSrcGetCurTrackInfo;
  pFuncSet->GetDRMInfo = voCMMBSrcGetDRMInfo;
  pFuncSet->GetDuration = voCMMBSrcGetDuration;
  pFuncSet->GetParam = voCMMBSrcGetParam;
  pFuncSet->GetProgramCount = voCMMBSrcGetProgramCount;
  pFuncSet->GetProgramInfo = voCMMBSrcGetProgramInfo;
  pFuncSet->GetSample = voCMMBSrcGetSample;
  pFuncSet->Init = voCMMBSrcInit;
  pFuncSet->Open = voCMMBSrcOpen;
  pFuncSet->Pause = voCMMBSrcPause;
  pFuncSet->Seek = voCMMBSrcSeek;
  pFuncSet->SelectProgram = voCMMBSrcSelectProgram;
  pFuncSet->SelectStream = voCMMBSrcSelectStream;
  pFuncSet->SelectTrack = voCMMBSrcSelectTrack;
  pFuncSet->SendBuffer = voCMMBSrcSendBuffer;
  pFuncSet->SetParam = voCMMBSrcSetParam;
  pFuncSet->Start = voCMMBSrcStart;
  pFuncSet->Stop = voCMMBSrcStop;
  pFuncSet->Uninit = voCMMBSrcUninit;

  return VOOSMP_ERR_None;
}


#if defined __cplusplus
}
#endif
