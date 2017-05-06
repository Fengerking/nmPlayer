/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: rdtpck.c,v 1.1.1.1.2.1 2005/05/04 18:22:04 hubbe Exp $
 * 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Portions Copyright (c) 1995-2005 RealNetworks, Inc.
 * All Rights Reserved.
 * 
 * The contents of this file, and the files included with this file,
 * are subject to the current version of the Real Format Source Code
 * Porting and Optimization License, available at
 * https://helixcommunity.org/2005/license/realformatsource (unless
 * RealNetworks otherwise expressly agrees in writing that you are
 * subject to a different license).  You may also obtain the license
 * terms directly from RealNetworks.  You may not use this file except
 * in compliance with the Real Format Source Code Porting and
 * Optimization License. There are no redistribution rights for the
 * source code of this file. Please see the Real Format Source Code
 * Porting and Optimization License for the rights, obligations and
 * limitations governing use of the contents of the file.
 * 
 * RealNetworks is the developer of the Original Code and owns the
 * copyrights in the portions it created.
 * 
 * This file, and the files included with this file, is distributed and
 * made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL
 * SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT
 * OR NON-INFRINGEMENT.
 * 
 * Technology Compatibility Kit Test Suite(s) Location:
 * https://rarvcode-tck.helixcommunity.org
 * 
 * Contributor(s):
 * 
 * ***** END LICENSE BLOCK ***** */

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "rm_memory_default.h"
#include "rdtpck.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

UINT16 GetRDTPacketType(UINT8* pBuf, UINT32 nLen)
{
    UINT16 unPacketType = RDT_UNKNOWN_TYPE;
    UINT16 unTemp       = 0;

    if( nLen >= 3 )
    {
        unTemp = getshort( pBuf+1 );

        if( unTemp < 0xFF00 )
        {
            unPacketType = RDT_DATA_PACKET;
        }
        else if( unTemp > RDT_BANDWIDTH_PROBE_PKT )
        {
            unPacketType = RDT_UNKNOWN_TYPE;
        }
        else
        {
            unPacketType = unTemp;
        }
    }
    
    return unPacketType;
}



void _createACKPacket( struct TNGACKPacket* pkt,
                       UINT16 unStreamNum,
                       UINT16 unBegSeqNum,
                       UINT16 unEndSeqNum,
                       HXBOOL bLost,
                       rm_malloc_func_ptr fpMalloc,
                       void*  pMemoryPool
                       )
{
    UINT16 unBitCount  = unEndSeqNum-unBegSeqNum;
    UINT8  unByteCount = 0;
    UINT16 unDataLen   = 0;
    UINT8* pData       = NULL;
    UINT8* pOff        = NULL;
    
    if( unBitCount > 0 )
        unByteCount = unBitCount/8+1;


    /* Pack the Ack/Nak bit field with the sequence
     * number and stream number info */
    unDataLen = 7+unByteCount;
    pData = (UINT8*)fpMalloc(pMemoryPool, sizeof(BYTE)*unDataLen);
    
    /* Setting this all to ones means we received all the
     * packets, all zeros means lost all packets.
     */
    memset( pData, (UINT8)(bLost?0:0xFF), unDataLen );
    pOff = pData;

    pOff = addshort(pOff, unStreamNum);
    pOff = addshort(pOff, unEndSeqNum);
    pOff = addshort(pOff, unBitCount);
    pOff = addbyte(pOff, unByteCount);


    /* Fill in the packet structure */
    pkt->length_included_flag = 0;
    pkt->dummy0               = 0;
    pkt->dummy1               = 0;
    pkt->packet_type = RDT_ACK_PKT;
    pkt->lost_high   = (UINT8)(bLost?1:0);
    pkt->data.data   = (INT8*)pData;
    pkt->data.len    = unDataLen;
}

void createNAKPacket( struct TNGACKPacket* pkt,
                      UINT16 unStreamNum,
                      UINT16 unBegSeqNum,
                      UINT16 unEndSeqNum
                      )
{
    _createACKPacket( pkt,
                      unStreamNum,
                      unBegSeqNum,
                      unEndSeqNum,
                      TRUE,
                      rm_memory_default_malloc,
                      NULL);
}

void createNAKPacketFromPool( struct TNGACKPacket* pkt,
                              UINT16 unStreamNum,
                              UINT16 unBegSeqNum,
                              UINT16 unEndSeqNum,
                              rm_malloc_func_ptr fpMalloc,
                              void*  pMemoryPool
                              )
{
    _createACKPacket( pkt,
                      unStreamNum,
                      unBegSeqNum,
                      unEndSeqNum,
                      TRUE,
                      fpMalloc,
                      pMemoryPool);
}


/* All seq numbers will be marked as received [beg, end] */
void createACKPacket( struct TNGACKPacket* pkt,
                      UINT16 unStreamNum,
                      UINT16 unBegSeqNum,
                      UINT16 unEndSeqNum
                      )
{
    _createACKPacket( pkt,
                      unStreamNum,
                      unBegSeqNum,
                      unEndSeqNum,
                      FALSE,
                      rm_memory_default_malloc,
                      NULL);    
}



void createACKPacketFromPool( struct TNGACKPacket* pkt,
                              UINT16 unStreamNum,
                              UINT16 unBegSeqNum,
                              UINT16 unEndSeqNum,
                              rm_malloc_func_ptr fpMalloc,
                              void*  pMemoryPool
                              )
{
    _createACKPacket( pkt,
                      unStreamNum,
                      unBegSeqNum,
                      unEndSeqNum,
                      FALSE,
                      fpMalloc,
                      pMemoryPool);    
}



/* 
 * Marshalling funcs.
 */
UINT8 getbyte(UINT8* data)
{
    return *data;
}

UINT16 getshort(UINT8* data)
{
    return ((UINT16)data[0])<<8|(UINT16)data[1];
}

INT32 getlong(UINT8* data)                           
{
    return (INT32) (((UINT32)getshort(data))<<16|(UINT32)getshort(data+2));
}

void putbyte(UINT8* data, INT8 v)
{
    *data = v;
}

void putshort(UINT8* data, UINT16 v)
{
    *data++ = (BYTE)(v>>8);
    *data++ = (BYTE)(v&0xff);
}

void putlong(UINT8* data, UINT32 v)
{
    *data++ = (BYTE)(v>>24);
    *data++ = (BYTE)((v>>16)&0xff);
    *data++ = (BYTE)((v>>8)&0xff);
    *data++ = (BYTE)(v&0xff);
}

UINT8* addbyte(UINT8* cp, UINT8 data)
{
    *cp = data;
    return cp+sizeof(UINT8);
}

UINT8* addshort(UINT8* cp, UINT16 data)
{
    putshort(cp, data);
    return cp + sizeof(UINT16);
}

UINT8* addlong(UINT8* cp, UINT32 data)
{
    putlong(cp, data);
    return cp + sizeof(UINT32);
}

UINT8* addstring(UINT8* cp, const UINT8* string, int len)
{
    memcpy(cp, string, len);
    return cp + len;
}
#ifdef _VONAMESPACE
}
#endif