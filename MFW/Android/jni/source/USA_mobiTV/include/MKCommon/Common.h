#ifndef MK_COMMON_H
#define MK_COMMON_H

#include <MKBase/Type.h>
#include <MKBase/Result.h>
#include <MKBase/Platform.h>
#include <MKBase/List.h>
#include <MKBase/CStr.h>
#include <MKBase/Socket.h>
#include <MKBase/NetAddr.h>
#include <MKBase/EvtCtrl.h>
#include <MKBase/Time.h>
#include <MKCommon/Chain.h>
#include <MKBase/Log.h>
#include <MKCommon/MediaInfo.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum MK_PlayType
{
    kPlayType_Unknown = 0,
    kPlayType_AfterOpen,
    kPlayType_AfterStop,
    kPlayType_AfterSeek
} MK_PlayType;

typedef struct _MK_Connection MK_Connection;
struct _MK_Connection
{   
    MK_Sock    mSock;
    MK_NetAddr mAddr;
    MK_NetAddr mRemoteAddr;
    MK_SockEvt mSockEvt;
};

/*
 * Creating and adding a MK_ListNodeCStrKV node to aProps if aKey does not exist. If aKey exists
 * the value of the property will be updated using aValue. A copy of aKey and aValue will added 
 * to the MK_ListNodeCStrKV node.
 */
void MK_Common_AddProperty(MK_List* aProps, const MK_Char* aKey, const MK_Char* aValue);

/*
 * Deallocates the entries in aProps. Will not deallocate
 * aProps itself.
 */
void MK_Common_FreePropertyList(MK_List* aProps);

/*
 * Retrieves property with key aKey from aList and pointer to the value.
 */
MK_Char* MK_Common_GetProperty(MK_List* aProps, const MK_Char* aKey); 


MK_INLINE MK_Bool MK_Common_IsFile(const MK_Char* aUrl)
{
    return  aUrl ? (0 != MK_CStr_FindStr(aUrl, "file://")) : MK_FALSE;
}

MK_INLINE MK_Bool MK_Common_IsHTTP(const MK_Char* aUrl)
{
    return  aUrl ? (0 != MK_CStr_FindStr(aUrl, "http://")) : MK_FALSE;
}

MK_INLINE MK_Bool MK_Common_IsRTSP(const MK_Char* aUrl)
{
    return  aUrl ? (0 != MK_CStr_FindStr(aUrl, "rtsp://")) : MK_FALSE;
}

MK_INLINE MK_Bool MK_Common_IsFragMented(const MK_Char* aUrl)
{
    return  aUrl ? (0 != MK_CStr_FindStr(aUrl, ".fmp4") || 0 != MK_CStr_FindStr(aUrl, "/start") || 0 != MK_CStr_FindStr(aUrl, "/live")) : MK_FALSE;
}

MK_INLINE MK_Bool MK_Common_IsSDP(const MK_Char* aUrl)
{
    return  aUrl ? (0 != MK_CStr_FindStr(aUrl, "sdp://")) : MK_FALSE;
}

/*
* Returns the port in aUrl if available. If no port is
* available but the protocol is known the default port is
* returned. Otherwise NULL is returned.
*
* Examples:
* rtsp://127.0.0.1:7070/foo/bar.3gp -> 7070
* http://127.0.0.1/foo/bar.3gp -> 80
* file://foo/bar.3gp -> NULL
*/
MK_Char* MK_Common_GetPortFromUrl(const MK_Char* aUrl);


/*
* Returns the host in aUrl on success otherwise NULL.
*
* Examples:
* rtsp://127.0.0.1:7070/foo/bar.3gp -> 172.0.0.1
* http://127.0.0.1/foo/bar.3gp -> 172.0.0.1
* file://foo/bar.3gp -> NULL
*/
MK_Char* MK_Common_GetHostFromUrl(const MK_Char* aUrl);


/*
 * Returns the resource describes by aUrl, i.e. strips protocol, host, port etc, 
 * on success otherwise NULL.
 *
 * Examples:
 * file://foo/bar.3gp -> foo/bar.3gp
 * rtsp://127.0.0.1:7070/foo/bar.3gp -> foo/bar.3gp
 * http://127.0.0.1/foo/bar.3gp -> /foo/bar.3gp
 */
MK_Char* MK_Common_GetResourceFromUrl(const MK_Char* aUrl);

MK_INLINE MK_Bool MK_Common_IsMulticast(const MK_Char* aAddr)
{
    MK_Char* lAddr = 0;
    MK_Char* lTemp = 0;
    MK_U32 lValue = 0;
    MK_ASSERT(aAddr);
    
    lAddr = MK_CStr_Clone(aAddr);
    if ((lTemp = MK_CStr_FindStr(lAddr, ".")))
    {
        lTemp = '\0';
        lValue = MK_CStr_DecimalToU32(lAddr, NULL);
    }
    MK_Mem_Free(lAddr);

    if (lValue >= 224 && lValue <= 239)
    {
        return MK_TRUE;
    }
    return MK_FALSE;
}

MK_INLINE MK_Result MK_Common_InitUDPSocket(MK_Sock* aSock, MK_NetAddr* aAddr, MK_Char* aHost, MK_Char* aPort)
{
    MK_Result lRes = MK_S_OK;
    MK_ASSERT(aSock);
    MK_ASSERT(aAddr);
    MK_ASSERT(aHost);
    MK_ASSERT(aPort);
    if (!aSock || !aAddr || !aHost || !aPort)
    {
        return MK_E_IARG;
    }

    if (MK_FAIL(lRes = MK_NetAddr_GetBindAddr(aHost, aPort, aAddr)))
    {
        MK_LOGERR("<Common> MK_NetAddr_GetBindAddr failed");
        return lRes;
    }
    if (MK_FAIL(lRes = MK_Sock_OpenUDP(aSock)))
    {
        MK_LOGERR("<Common> MK_Sock_OpenUDP failed");
        return lRes;
    }
    if (MK_FAIL(lRes = MK_Sock_SetReuseAddr(aSock, MK_TRUE)))
    {
        MK_LOGERR("<Common> MK_Sock_SetReuseAddr failed");
        return lRes;
    }
    if (MK_FAIL(lRes = MK_Sock_SetNonBlocking(aSock, MK_TRUE)))
    {
        MK_LOGERR("<Common> MK_Sock_SetNonBlocking failed");
        return lRes;
    }
    if (MK_FAIL(lRes = MK_Sock_SetRecvBuf(aSock, 1000000)))
    {
        MK_LOGERR("<Common> MK_Sock_SetRecvBuf failed");
        return lRes;
    }
    if (MK_FAIL(lRes = MK_Sock_Bind(aSock, aAddr)))
    {
        MK_LOGERR("<Common> MK_Sock_Bind failed");
        return lRes;
    }

    return lRes;
}

MK_INLINE MK_Result MK_Common_SendDataOnConnection(MK_Connection* aConnection, MK_U8* aBuf, MK_U32 aBufSize)
{
    if (0 >= MK_Sock_SendTo(&aConnection->mSock, aBuf, aBufSize, &aConnection->mRemoteAddr))
    {
        return MK_E_FAIL;
    }

    return MK_S_OK;
}

/*
 * Creates a MK_ListNode_Ptr that wraps aObject.
 *
 * Return value: Pointer to a MK_ListNode_Ptr.
 */
MK_INLINE MK_ListNode_Ptr* MK_Common_CreateListNodePtr(void* aObject)
{
    MK_ListNode_Ptr* lNode = (MK_ListNode_Ptr*)MK_Mem_AllocAndZero(sizeof(MK_ListNode_Ptr));
    lNode->Ptr = aObject;

    return lNode;
}

MK_INLINE MK_Bool MK_Common_IsSameHost(const MK_Char* aCurUrl, const MK_Char* aNewUrl)
{
    MK_Char* lCurHost = 0;
    MK_Char* lNewHost = 0;
    MK_Bool lSameHost = MK_FALSE;

    if (aCurUrl && aNewUrl)
    {
        lCurHost = MK_Common_GetHostFromUrl(aCurUrl);
        lNewHost = MK_Common_GetHostFromUrl(aNewUrl);
        lSameHost = (0 == MK_CStr_Cmp(lCurHost, lNewHost)) ? MK_TRUE : MK_FALSE;
        MK_Mem_Free(lCurHost);
        MK_Mem_Free(lNewHost);
    }
    return lSameHost;
}

MK_INLINE MK_Bool MK_Common_IsVOD(const MK_Char* aUrl)
{
    return MK_CStr_FindStr(aUrl, ".3gp") ? MK_TRUE : MK_FALSE;
}

MK_INLINE MK_Bool MK_Common_IsLive(const MK_Char* aUrl)
{
    return MK_CStr_FindStr(aUrl, ".sdp") ? MK_TRUE : MK_FALSE;
}

/************************************************************************/

MK_INLINE void MK_Common_List_CStr_Destroy(MK_List* aList)
{
    MK_ListNode_CStr* lNode = 0;
    MK_ASSERT(aList);
    while ((lNode = MK_List_CStr_PopFirst(aList)))
    {
        MK_Mem_Free(lNode->CStr);
        MK_Mem_Free(lNode);
    }
}

MK_INLINE MK_Result MK_Common_CStr_Split(const MK_Char* aStr, MK_Char* aSet, MK_List* aList)
{
    MK_Char* lStr = 0;
    MK_Char* lStrOrg = 0;
    MK_Char* lTemp = 0;
    MK_ListNode_CStr* lNode = 0;

    MK_ASSERT(aStr);
    MK_ASSERT(aSet);
    MK_ASSERT(aList);

    lStrOrg = MK_CStr_Clone(aStr);
    lStr = lStrOrg;

    while ((lTemp = MK_CStr_FindStr(lStr, aSet)))
    {
        *lTemp = '\0';  

        lNode = (MK_ListNode_CStr*)MK_Mem_AllocAndZero(sizeof(MK_ListNode_CStr));
        
        while (MK_Char_IsBlank(*lStr))
        {
            lStr++;
        }
        lNode->CStr = MK_CStr_Clone(lStr);
        MK_List_CStr_PushLast(aList, lNode);

        lStr = (lTemp += MK_CStr_Len(aSet));
        while (MK_Char_IsBlank(*lStr))
        {
            lStr++;
        }
    }
    if (0 < MK_CStr_Len(lStr))
    {
        lNode = (MK_ListNode_CStr*)MK_Mem_AllocAndZero(sizeof(MK_ListNode_CStr));
        lNode->CStr = MK_CStr_Clone(lStr);
        MK_List_CStr_PushLast(aList, lNode);
    }
    MK_Mem_Free(lStrOrg);

    return MK_S_OK;
}

MK_INLINE MK_Result MK_Common_ParseParam(MK_Char* aParam, MK_Char** aKey, MK_Char** aVal)
{
    MK_Char* lParam = 0;
    MK_Char* lTemp = 0;
    MK_ASSERT(aParam);
    MK_ASSERT(aKey);
    MK_ASSERT(aVal);

    lParam = MK_CStr_Clone(aParam);
    if ((lTemp = MK_CStr_Break(lParam, "=")))
    {
        *lTemp = '\0';
        *aKey = MK_CStr_Clone(lParam);
        *aVal = MK_CStr_Clone(++lTemp);
    }
    else
    {
        *aKey = MK_CStr_Clone(aParam);
    }
    MK_Mem_Free(lParam);
    return MK_S_OK;
}

#ifdef __cplusplus
}
#endif

#endif

