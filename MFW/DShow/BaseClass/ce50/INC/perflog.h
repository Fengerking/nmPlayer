//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//
//--------------------------------------------------------------------------;

// Performance logging facilities, Oct 2003

#ifndef __PERFLOG__
#define __PERFLOG__

#include <celog.h>
#include <perflogdata.h>

extern DBGPARAM dpCurSettings;

//
// Macros for perf logging for DShow
//
#ifdef SHIP_BUILD

#define PERFLOG_TRACE_PACKET(dwEvent,pSample,llCurrentTime,llPresentationTime,msPacketDuration,dwNBytes,dwElapsed) ((void)0)
#define PERFLOG_TRACE_OBJECT(dwEvent,StreamId,dwObjId,pObject,llCurrentTime,llPresentationTime,dwNBytes,bIsKeyFrame,dwElapsed) ((void)0)

#else

#define PERFLOG_TRACE_PACKET(dwEvent,pSample,llCurrentTime,llPresentationTime,dwNBytes,dwElapsed) \
        PerfLogTracePacket(dwEvent,pSample,llCurrentTime,llPresentationTime,dwNBytes,dwElapsed)

#define PERFLOG_TRACE_OBJECT(dwEvent,StreamId,dwObjId,pObject,llCurrentTime,llPresentationTime,dwNBytes,bIsKeyFrame,dwElapsed) \
        PerfLogTraceObject(dwEvent,StreamId,dwObjId,pObject,llCurrentTime,llPresentationTime,dwNBytes,bIsKeyFrame,dwElapsed)
#endif


//
// Logs the desired packet event into celog (iff DebugZone ZONE_CELOG is turned on)
//
__inline void PerfLogTracePacket (
    DWORD    dwEvent,
    PVOID    pPacket,
    LONGLONG msCurrentTime,
    LONGLONG msPresentationTime,
    DWORD    msPacketDuration,
    DWORD    dwNBytes,
    DWORD    dwElapsed)
{
    if (ZONE_CELOG)
    {
        PERFLOG_PACKET_INFO packetInfo;

        packetInfo.dwEvent = dwEvent;
        packetInfo.pPacket = pPacket;
        packetInfo.msCurrentTime = msCurrentTime;
        packetInfo.msPresentationTime = msPresentationTime;
        packetInfo.msPacketDuration = msPacketDuration;
        packetInfo.dwNBytes = dwNBytes;
        packetInfo.dwElapsed = dwElapsed;

        CeLogData(TRUE, CELID_DSHOW_PACKET, &packetInfo, sizeof(packetInfo), 1, CELZONE_MISC, 0, FALSE);
    }
}


//
// Logs the desired object event into celog (iff DebugZone ZONE_CELOG is turned on)
//
__inline void PerfLogTraceObject (
    DWORD             dwEvent,
    DWORD             dwStreamID,
    DWORD             dwObjectID,
    PVOID             pObject,
    LONGLONG          msCurrentTime,
    LONGLONG          msPresentationTime,
    DWORD             dwNBytes,
    BOOL              bIsKeyFrame,
    DWORD             dwElapsed)
{
    if (ZONE_CELOG)
    {
        PERFLOG_OBJECT_INFO objInfo;

        objInfo.dwEvent = dwEvent;
        objInfo.dwStreamID = dwStreamID;
        objInfo.dwObjectID = dwObjectID;
        objInfo.pObject = pObject;
        objInfo.msCurrentTime = msCurrentTime;
        objInfo.msPresentationTime = msPresentationTime;
        objInfo.dwNBytes = dwNBytes;
        objInfo.bIsKeyFrame = bIsKeyFrame;
        objInfo.dwElapsed = dwElapsed;

        CeLogData(TRUE, CELID_DSHOW_OBJECT, &objInfo, sizeof(objInfo), 1, CELZONE_MISC, 0, FALSE);
    }
}


#endif // __PERFLOG__


