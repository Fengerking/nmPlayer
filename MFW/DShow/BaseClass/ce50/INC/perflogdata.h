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

#ifndef __PERFLOGDATA__
#define __PERFLOGDATA__

#define CELID_DSHOW_BASE   (CELID_USER + 0x300)

#define CELID_DSHOW_PACKET (CELID_DSHOW_BASE)
#define CELID_DSHOW_OBJECT (CELID_DSHOW_BASE + 1)
#define CELID_DSHOW_MAX    (CELID_DSHOW_BASE + 2)

//
// Events that can be logged into CeLog
//
typedef enum
{
    PERFLOG_EV_ASFREADER_PACKET_READ,
    PERFLOG_EV_ASFREADER_PACKET_DISPATCHED,

    PERFLOG_EV_ASFDEMUX_OBJECT_ADDED,
    PERFLOG_EV_ASFDEMUX_OBJECT_DELIVERED,

    PERFLOG_EV_DMOWRAPPER_OBJECT_RECEIVED,
    PERFLOG_EV_DMOWRAPPER_OBJECT_READY,

    PERFLOG_EV_DMO_OBJECT_RECEIVED,
    PERFLOG_EV_DMO_OBJECT_DECODE,
    PERFLOG_EV_DMO_OBJECT_READY,
    PERFLOG_EV_DMO_OBJECT_DROPPED,

    PERFLOG_EV_RENDERER_OBJECT_RECEIVED,
    PERFLOG_EV_RENDERER_OBJECT_RENDER,
    PERFLOG_EV_RENDERER_OBJECT_READY,
    PERFLOG_EV_RENDERER_OBJECT_DROPPED,

    PERFLOG_EV_SIZEOF
} PerfLogEventType;


//
// Should match the above events
//
#define PERFLOG_EVENT_MESSAGES                           \
{                                                        \
    L"PERFLOG_EV_ASFREADER_PACKET_READ",                 \
    L"PERFLOG_EV_ASFREADER_PACKET_DISPATCHED",           \
    L"PERFLOG_EV_ASFDEMUX_OBJECT_ADDED",                 \
    L"PERFLOG_EV_ASFDEMUX_OBJECT_DELIVERED",             \
    L"PERFLOG_EV_DMOWRAPPER_OBJECT_RECEIVED",            \
    L"PERFLOG_EV_DMOWRAPPER_OBJECT_READY",               \
    L"PERFLOG_EV_DMO_OBJECT_RECEIVED",                   \
    L"PERFLOG_EV_DMO_OBJECT_DECODE",                     \
    L"PERFLOG_EV_DMO_OBJECT_READY",                      \
    L"PERFLOG_EV_DMO_OBJECT_DROPPED",                    \
    L"PERFLOG_EV_RENDERER_OBJECT_RECEIVED",              \
    L"PERFLOG_EV_RENDERER_OBJECT_RENDER",                \
    L"PERFLOG_EV_RENDERER_OBJECT_READY",                 \
    L"PERFLOG_EV_RENDERER_OBJECT_DROPPED"                \
}

//
// set to a high value so as to not confuse with values from asf stream. To be used when values from asf stream are 
// not available (for example, inside the video DMO).
//
#define PERFLOG_STREAM_AUDIO   (32 * 1024)
#define PERFLOG_STREAM_VIDEO   ((32 * 1024) + 1)

//
// A packet is a piece of either an audio or video frame
//
typedef struct _PERFLOG_PACKET_INFO
{
    DWORD    dwEvent;                      // which packet event needs to be logged
    PVOID    pPacket;                      // pointer to the packet
    LONGLONG msCurrentTime;                // current stream time, in millisecs (-1 means unknown)
    LONGLONG msPresentationTime;           // presentation time stamp, in millisecs (-1 means unknown)
    DWORD    msPacketDuration;             // packet duration, in millisecs (-1 means unknown)
    DWORD    dwNBytes;                     // size of the packet in bytes (-1 means unknown)
    DWORD    dwElapsed;                    // elapsed # of ticks for the specific operation (-1 means unknown)
} PERFLOG_PACKET_INFO;


//
// An object is either an audio or video frame
//
typedef struct _PERFLOG_OBJECT_INFO
{
    DWORD             dwEvent;             // which object event needs to be logged
    PVOID             pObject;             // pointer to the object
    DWORD             dwStreamID;          // stream identifier (use identifier from ASF stream if available; otherwise, use PERFLOG_STREAM_AUDIO or VIDEO. If unknown, set to -1)
    DWORD             dwObjectID;          // object identifier (-1 means unknown)
    LONGLONG          msCurrentTime;       // current stream time, in millisecs (-1 means unknown)    
    LONGLONG          msPresentationTime;  // presentation time stamp, in millisecs (-1 means unknown)
    DWORD             dwNBytes;            // size of the object in bytes (-1 means unknown)
    BOOL              bIsKeyFrame;         // set if object is a key frame
    DWORD             dwElapsed;           // elapsed # of ticks for the specific operation (-1 means unknown)
} PERFLOG_OBJECT_INFO;

#endif // __PERFLOGDATA__


