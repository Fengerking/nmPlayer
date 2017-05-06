#ifndef __MEDIAKITEXTENDEDAPI_H__
#define __MEDIAKITEXTENDEDAPI_H__

#include "MKPlayer/Player.h"
#include "MKPlayer/Result.h"
#include "DRM/DRMExternalApi.h"


/* Static API */
typedef	MK_Bool     (*MK_MobiDrmContentIsEncrypted_FP)      (const MK_U8* aBuf, MK_U32 aSize);
typedef	MK_Result   (*MK_MobiDrmGetContentInfo_FP)          (const MK_U8* aBuf, MK_U32 aSize, sMK_MobiDrmMbdcHeader* aHeader);
typedef MK_Result   (*MK_MobiDrmGetLicenseInfo_FP)          (const MK_U8* aBuf, MK_U32 aSize, const MK_U8* aPrivKey, 
							     MK_U32 aPrivKeyLen, MK_U32 aTime,
							     sMK_MobiDrmIDs* aIDs, sMK_MobiDrmLicense* aLicense, MK_List* aKeys);

/* Dynamic API */
typedef	MK_Result   (*MK_Player_Net_SetLocalIF_FP)          (MK_Player* aPlay, const MK_Char* aName);
typedef	MK_Result   (*MK_Player_Net_SetTimeout_FP)          (MK_Player* aPlay, MK_U32 aMSec);
typedef	MK_Result   (*MK_Player_MobiDRM_SetKey_FP)          (MK_Player* aPlay, const void* aKey, MK_U32 aSz);
typedef	MK_Result   (*MK_Player_MobiDRM_SetDeviceIDs_FP)    (MK_Player* aPlay, const void* aDID, MK_U32 aDSz, const void* aXID, MK_U32 aXSz);
typedef	MK_Result   (*MK_Player_MobiDRM_SetUserID_FP)       (MK_Player* aPlay, const void* aUID, MK_U32 aSz);
typedef	MK_Result   (*MK_Player_MobiDRM_SetLicense_FP)      (MK_Player* aPlay, const void* aLic, MK_U32 aSz, MK_U32 aNow);
typedef	MK_Result   (*MK_Player_HTTP_SetProxy_FP)           (MK_Player* aPlay, const MK_Char* aHost, MK_U16 aPort);
typedef	MK_Result   (*MK_Player_HTTP_SetUserAgentVals_FP)   (MK_Player* aPlay, const MK_Char* aName, const MK_Char* aVer, const MK_Char* aDev, const MK_Char* aExtra);
typedef	MK_Result   (*MK_Player_Media_SetBufferLimits_FP)   (MK_Player* aPlay, MK_U32 aMinMSec, MK_U32 aLowMSec, MK_U32 aMaxMSec);

#endif // __MEDIAKITEXTENDEDAPI_H__
