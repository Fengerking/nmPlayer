/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: tngpkt.c,v 1.1.1.1.2.1 2005/05/04 18:22:04 hubbe Exp $
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

/*
 * This is generated code, do not modify. Look in
 * /home/gregory/helix/protocol/transport/rdt/pub/tngpkt.pm to make
 * modifications
 */


#include <stdlib.h>
#include <string.h>
#include "tngpktrtsp.h"
#include "rm_memory_default.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
const UINT32 TNGDataPacket_static_size() {return 16;}

UINT8* TNGDataPacket_pack(UINT8* buf, UINT32 len, struct TNGDataPacket* pkt)
{
    UINT8* off = buf;

    UINT8* pLen = 0;

    {*off &= ~(1<<7); *off |= (pkt->length_included_flag&1)<<7;}
    {*off &= ~(1<<6); *off |= (pkt->need_reliable_flag&1)<<6;}
    {
	*off &= ~0x3e; *off |= (pkt->stream_id&0x1f)<<1;
    }
    {*off &= ~1; *off++ |= pkt->is_reliable&1;}
    {*off++ = (UINT8) (pkt->seq_no>>8); *off++ = (UINT8) (pkt->seq_no);}
    if ((pkt->length_included_flag== 1))
    {
      pLen = off;
      off += 2;
      #define PMC_PACK_LENGTH(off,_packlenwhendone) {*off++ = (UINT8) (_packlenwhendone>>8); *off++ = (UINT8) (_packlenwhendone);}
    }
    {*off &= ~(1<<7); *off |= (pkt->back_to_back_packet&1)<<7;}
    {*off &= ~(1<<6); *off |= (pkt->slow_data&1)<<6;}
    {
	*off &= ~0x3f; *off++ |= pkt->asm_rule_number&0x3f;
    }
    {
	*off++ = (UINT8) (pkt->timestamp>>24); *off++ = (UINT8) (pkt->timestamp>>16); *off++ = (UINT8) (pkt->timestamp>>8); *off++ = (UINT8) (pkt->timestamp);
    }
    if ((pkt->stream_id== 31))
    {
      {*off++ = (UINT8) (pkt->stream_id_expansion>>8); *off++ = (UINT8) (pkt->stream_id_expansion);}
    }
    if ((pkt->need_reliable_flag== 1))
    {
      {*off++ = (UINT8) (pkt->total_reliable>>8); *off++ = (UINT8) (pkt->total_reliable);}
    }
    if ((pkt->asm_rule_number== 63))
    {
      {*off++ = (UINT8) (pkt->asm_rule_number_expansion>>8); *off++ = (UINT8) (pkt->asm_rule_number_expansion);}
    }
    {
	if (pkt->data.data) memcpy(off, pkt->data.data, pkt->data.len);
	off += pkt->data.len;
    }
    len = off-buf;
    if (pLen)
    {
        PMC_PACK_LENGTH(pLen,len);
    }
    #undef PMC_PACK_LENGTH
    return off;
}

UINT8* TNGDataPacket_unpack(UINT8* buf, UINT32 len, struct TNGDataPacket* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->length_included_flag = (*off>>7)&1;
    pkt->need_reliable_flag = (*off>>6)&1;
    {
	pkt->stream_id  = (*off&0x3e)>>1;
    }
    pkt->is_reliable = *off++&1;
    {pkt->seq_no = *off++<<8; pkt->seq_no |= *off++;}
    if ((pkt->length_included_flag== 1))
    {
      {pkt->_packlenwhendone = *off++<<8; pkt->_packlenwhendone |= *off++;}
    }
    pkt->back_to_back_packet = (*off>>7)&1;
    pkt->slow_data = (*off>>6)&1;
    {
	pkt->asm_rule_number  = *off++&0x3f;
    }
    {
	pkt->timestamp = ((UINT32)*off++)<<24; pkt->timestamp |= ((UINT32)*off++)<<16;
	pkt->timestamp |= ((UINT32)*off++)<<8; pkt->timestamp |= ((UINT32)*off++);
    }
    if ((pkt->stream_id== 31))
    {
      {pkt->stream_id_expansion = *off++<<8; pkt->stream_id_expansion |= *off++;}
    }
    if ((pkt->need_reliable_flag== 1))
    {
      {pkt->total_reliable = *off++<<8; pkt->total_reliable |= *off++;}
    }
    if ((pkt->asm_rule_number== 63))
    {
      {pkt->asm_rule_number_expansion = *off++<<8; pkt->asm_rule_number_expansion |= *off++;}
    }
    {
	pkt->data.len = len - (off - buf);
	if (off-buf+pkt->data.len > (int)len)
	    return 0;
	pkt->data.data = (INT8 *)off; off += pkt->data.len;
    }
    return off;
}

const UINT32 TNGMultiCastDataPacket_static_size() {return 17;}

UINT8* TNGMultiCastDataPacket_pack(UINT8* buf, UINT32 len, struct TNGMultiCastDataPacket* pkt)
{
    UINT8* off = buf;

    {*off &= ~(1<<7); *off |= (pkt->length_included_flag&1)<<7;}
    {*off &= ~(1<<6); *off |= (pkt->need_reliable_flag&1)<<6;}
    {
	*off &= ~0x3e; *off |= (pkt->stream_id&0x1f)<<1;
    }
    {*off &= ~1; *off++ |= pkt->is_reliable&1;}
    {*off++ = (UINT8) (pkt->seq_no>>8); *off++ = (UINT8) (pkt->seq_no);}
    if ((pkt->length_included_flag== 1))
    {
      {*off++ = (UINT8) (pkt->length>>8); *off++ = (UINT8) (pkt->length);}
    }
    {*off &= ~(1<<7); *off |= (pkt->back_to_back_packet&1)<<7;}
    {*off &= ~(1<<6); *off |= (pkt->slow_data&1)<<6;}
    {
	*off &= ~0x3f; *off++ |= pkt->asm_rule_number&0x3f;
    }
    *off++ = pkt->group_seq_no;
    {
	*off++ = (UINT8) (pkt->timestamp>>24); *off++ = (UINT8) (pkt->timestamp>>16); *off++ = (UINT8) (pkt->timestamp>>8); *off++ = (UINT8) (pkt->timestamp);
    }
    if ((pkt->stream_id== 31))
    {
      {*off++ = (UINT8) (pkt->stream_id_expansion>>8); *off++ = (UINT8) (pkt->stream_id_expansion);}
    }
    if ((pkt->need_reliable_flag== 1))
    {
      {*off++ = (UINT8) (pkt->total_reliable>>8); *off++ = (UINT8) (pkt->total_reliable);}
    }
    if ((pkt->asm_rule_number== 63))
    {
      {*off++ = (UINT8) (pkt->asm_rule_number_expansion>>8); *off++ = (UINT8) (pkt->asm_rule_number_expansion);}
    }
    {
	if (pkt->data.data) memcpy(off, pkt->data.data, pkt->data.len);
	off += pkt->data.len;
    }
    len = off-buf;
    return off;
}

UINT8* TNGMultiCastDataPacket_unpack(UINT8* buf, UINT32 len, struct TNGMultiCastDataPacket* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->length_included_flag = (*off>>7)&1;
    pkt->need_reliable_flag = (*off>>6)&1;
    {
	pkt->stream_id  = (*off&0x3e)>>1;
    }
    pkt->is_reliable = *off++&1;
    {pkt->seq_no = *off++<<8; pkt->seq_no |= *off++;}
    if ((pkt->length_included_flag== 1))
    {
      {pkt->length = *off++<<8; pkt->length |= *off++;}
    }
    pkt->back_to_back_packet = (*off>>7)&1;
    pkt->slow_data = (*off>>6)&1;
    {
	pkt->asm_rule_number  = *off++&0x3f;
    }
    pkt->group_seq_no = *off++;
    {
	pkt->timestamp = ((UINT32)*off++)<<24; pkt->timestamp |= ((UINT32)*off++)<<16;
	pkt->timestamp |= ((UINT32)*off++)<<8; pkt->timestamp |= ((UINT32)*off++);
    }
    if ((pkt->stream_id== 31))
    {
      {pkt->stream_id_expansion = *off++<<8; pkt->stream_id_expansion |= *off++;}
    }
    if ((pkt->need_reliable_flag== 1))
    {
      {pkt->total_reliable = *off++<<8; pkt->total_reliable |= *off++;}
    }
    if ((pkt->asm_rule_number== 63))
    {
      {pkt->asm_rule_number_expansion = *off++<<8; pkt->asm_rule_number_expansion |= *off++;}
    }
    {
	pkt->data.len = len - (off - buf);
	if (off-buf+pkt->data.len > (int)len)
	    return 0;
	pkt->data.data = (INT8 *)off; off += pkt->data.len;
    }
    return off;
}

const UINT32 TNGASMActionPacket_static_size() {return 9;}

UINT8* TNGASMActionPacket_pack(UINT8* buf, UINT32 len, struct TNGASMActionPacket* pkt)
{
    UINT8* off = buf;

    {*off &= ~(1<<7); *off |= (pkt->length_included_flag&1)<<7;}
    {
	*off &= ~0x7c; *off |= (pkt->stream_id&0x1f)<<2;
    }
    {*off &= ~(1<<1); *off |= (pkt->dummy0&1)<<1;}
    {*off &= ~1; *off++ |= pkt->dummy1&1;}
    {*off++ = (UINT8) (pkt->packet_type>>8); *off++ = (UINT8) (pkt->packet_type);}
    {*off++ = (UINT8) (pkt->reliable_seq_no>>8); *off++ = (UINT8) (pkt->reliable_seq_no);}
    if ((pkt->length_included_flag== 1))
    {
      {*off++ = (UINT8) (pkt->length>>8); *off++ = (UINT8) (pkt->length);}
    }
    if ((pkt->stream_id== 31))
    {
      {*off++ = (UINT8) (pkt->stream_id_expansion>>8); *off++ = (UINT8) (pkt->stream_id_expansion);}
    }
    {
	if (pkt->data.data) memcpy(off, pkt->data.data, pkt->data.len);
	off += pkt->data.len;
    }
    len = off-buf;
    return off;
}

UINT8* TNGASMActionPacket_unpack(UINT8* buf, UINT32 len, struct TNGASMActionPacket* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->length_included_flag = (*off>>7)&1;
    {
	pkt->stream_id  = (*off&0x7c)>>2;
    }
    pkt->dummy0 = (*off>>1)&1;
    pkt->dummy1 = *off++&1;
    {pkt->packet_type = *off++<<8; pkt->packet_type |= *off++;}
    {pkt->reliable_seq_no = *off++<<8; pkt->reliable_seq_no |= *off++;}
    if ((pkt->length_included_flag== 1))
    {
      {pkt->length = *off++<<8; pkt->length |= *off++;}
    }
    if ((pkt->stream_id== 31))
    {
      {pkt->stream_id_expansion = *off++<<8; pkt->stream_id_expansion |= *off++;}
    }
    {
	pkt->data.len = len - (off - buf);
	if (off-buf+pkt->data.len > (int)len)
	    return 0;
	pkt->data.data = (INT8 *)off; off += pkt->data.len;
    }
    return off;
}

const UINT32 TNGBandwidthReportPacket_static_size() {return 12;}

UINT8* TNGBandwidthReportPacket_pack(UINT8* buf, UINT32 len, struct TNGBandwidthReportPacket* pkt)
{
    UINT8* off = buf;

    {*off &= ~(1<<7); *off |= (pkt->length_included_flag&1)<<7;}
    {
	*off &= ~0x7c; *off |= (pkt->dummy0&0x1f)<<2;
    }
    {*off &= ~(1<<1); *off |= (pkt->dummy1&1)<<1;}
    {*off &= ~1; *off++ |= pkt->dummy2&1;}
    {*off++ = (UINT8) (pkt->packet_type>>8); *off++ = (UINT8) (pkt->packet_type);}
    if ((pkt->length_included_flag== 1))
    {
      {*off++ = (UINT8) (pkt->length>>8); *off++ = (UINT8) (pkt->length);}
    }
    {*off++ = (UINT8) (pkt->interval>>8); *off++ = (UINT8) (pkt->interval);}
    {
	*off++ = (UINT8) (pkt->bandwidth>>24); *off++ = (UINT8) (pkt->bandwidth>>16); *off++ = (UINT8) (pkt->bandwidth>>8); *off++ = (UINT8) (pkt->bandwidth);
    }
    *off++ = pkt->sequence;
    len = off-buf;
    return off;
}

UINT8* TNGBandwidthReportPacket_unpack(UINT8* buf, UINT32 len, struct TNGBandwidthReportPacket* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->length_included_flag = (*off>>7)&1;
    {
	pkt->dummy0  = (*off&0x7c)>>2;
    }
    pkt->dummy1 = (*off>>1)&1;
    pkt->dummy2 = *off++&1;
    {pkt->packet_type = *off++<<8; pkt->packet_type |= *off++;}
    if ((pkt->length_included_flag== 1))
    {
      {pkt->length = *off++<<8; pkt->length |= *off++;}
    }
    {pkt->interval = *off++<<8; pkt->interval |= *off++;}
    {
	pkt->bandwidth = ((UINT32)*off++)<<24; pkt->bandwidth |= ((UINT32)*off++)<<16;
	pkt->bandwidth |= ((UINT32)*off++)<<8; pkt->bandwidth |= ((UINT32)*off++);
    }
    pkt->sequence = *off++;
    return off;
}

const UINT32 TNGReportPacket_static_size() {return 5;}

UINT8* TNGReportPacket_pack(UINT8* buf, UINT32 len, struct TNGReportPacket* pkt)
{
    UINT8* off = buf;

    {*off &= ~(1<<7); *off |= (pkt->length_included_flag&1)<<7;}
    {
	*off &= ~0x7c; *off |= (pkt->dummy0&0x1f)<<2;
    }
    {*off &= ~(1<<1); *off |= (pkt->dummy1&1)<<1;}
    {*off &= ~1; *off++ |= pkt->dummy2&1;}
    {*off++ = (UINT8) (pkt->packet_type>>8); *off++ = (UINT8) (pkt->packet_type);}
    if ((pkt->length_included_flag== 1))
    {
      {*off++ = (UINT8) (pkt->length>>8); *off++ = (UINT8) (pkt->length);}
    }
    {
	if (pkt->data.data) memcpy(off, pkt->data.data, pkt->data.len);
	off += pkt->data.len;
    }
    len = off-buf;
    return off;
}

UINT8* TNGReportPacket_unpack(UINT8* buf, UINT32 len, struct TNGReportPacket* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->length_included_flag = (*off>>7)&1;
    {
	pkt->dummy0  = (*off&0x7c)>>2;
    }
    pkt->dummy1 = (*off>>1)&1;
    pkt->dummy2 = *off++&1;
    {pkt->packet_type = *off++<<8; pkt->packet_type |= *off++;}
    if ((pkt->length_included_flag== 1))
    {
      {pkt->length = *off++<<8; pkt->length |= *off++;}
    }
    {
	pkt->data.len = len - (off - buf);
	if (off-buf+pkt->data.len > (int)len)
	    return 0;
	pkt->data.data = (INT8 *)off; off += pkt->data.len;
    }
    return off;
}

const UINT32 TNGACKPacket_static_size() {return 5;}

UINT8* TNGACKPacket_pack(UINT8* buf, UINT32 len, struct TNGACKPacket* pkt)
{
    UINT8* off = buf;

    {*off &= ~(1<<7); *off |= (pkt->length_included_flag&1)<<7;}
    {*off &= ~(1<<6); *off |= (pkt->lost_high&1)<<6;}
    {
	*off &= ~0x3e; *off |= (pkt->dummy0&0x1f)<<1;
    }
    {*off &= ~1; *off++ |= pkt->dummy1&1;}
    {*off++ = (UINT8) (pkt->packet_type>>8); *off++ = (UINT8) (pkt->packet_type);}
    if ((pkt->length_included_flag== 1))
    {
      {*off++ = (UINT8) (pkt->length>>8); *off++ = (UINT8) (pkt->length);}
    }
    {
	if (pkt->data.data) memcpy(off, pkt->data.data, pkt->data.len);
	off += pkt->data.len;
    }
    len = off-buf;
    return off;
}

UINT8* TNGACKPacket_unpack(UINT8* buf, UINT32 len, struct TNGACKPacket* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->length_included_flag = (*off>>7)&1;
    pkt->lost_high = (*off>>6)&1;
    {
	pkt->dummy0  = (*off&0x3e)>>1;
    }
    pkt->dummy1 = *off++&1;
    {pkt->packet_type = *off++<<8; pkt->packet_type |= *off++;}
    if ((pkt->length_included_flag== 1))
    {
      {pkt->length = *off++<<8; pkt->length |= *off++;}
    }
    {
	pkt->data.len = len - (off - buf);
	if (off-buf+pkt->data.len > (int)len)
	    return 0;
	pkt->data.data = (INT8 *)off; off += pkt->data.len;
    }
    return off;
}
const UINT32 TNGRTTRequestPacket_static_size() {return 3;}

UINT8* TNGRTTRequestPacket_pack(UINT8* buf, UINT32 len, struct TNGRTTRequestPacket* pkt)
{
    UINT8* off = buf;

    {*off &= ~(1<<7); *off |= (pkt->dummy0&1)<<7;}
    {
	*off &= ~0x7c; *off |= (pkt->dummy1&0x1f)<<2;
    }
    {*off &= ~(1<<1); *off |= (pkt->dummy2&1)<<1;}
    {*off &= ~1; *off++ |= pkt->dummy3&1;}
    {*off++ = (UINT8) (pkt->packet_type>>8); *off++ = (UINT8) (pkt->packet_type);}
    len = off-buf;
    return off;
}

UINT8* TNGRTTRequestPacket_unpack(UINT8* buf, UINT32 len, struct TNGRTTRequestPacket* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->dummy0 = (*off>>7)&1;
    {
	pkt->dummy1  = (*off&0x7c)>>2;
    }
    pkt->dummy2 = (*off>>1)&1;
    pkt->dummy3 = *off++&1;
    {pkt->packet_type = *off++<<8; pkt->packet_type |= *off++;}
    return off;
}

const UINT32 TNGRTTResponsePacket_static_size() {return 11;}

UINT8* TNGRTTResponsePacket_pack(UINT8* buf, UINT32 len, struct TNGRTTResponsePacket* pkt)
{
    UINT8* off = buf;

    {*off &= ~(1<<7); *off |= (pkt->dummy0&1)<<7;}
    {
	*off &= ~0x7c; *off |= (pkt->dummy1&0x1f)<<2;
    }
    {*off &= ~(1<<1); *off |= (pkt->dummy2&1)<<1;}
    {*off &= ~1; *off++ |= pkt->dummy3&1;}
    {*off++ = (UINT8) (pkt->packet_type>>8); *off++ = (UINT8) (pkt->packet_type);}
    {
	*off++ = (UINT8) (pkt->timestamp_sec>>24); *off++ = (UINT8) (pkt->timestamp_sec>>16); *off++ = (UINT8) (pkt->timestamp_sec>>8); *off++ = (UINT8) (pkt->timestamp_sec);
    }
    {
	*off++ = (UINT8) (pkt->timestamp_usec>>24); *off++ = (UINT8) (pkt->timestamp_usec>>16); *off++ = (UINT8) (pkt->timestamp_usec>>8); *off++ = (UINT8) (pkt->timestamp_usec);
    }
    len = off-buf;
    return off;
}

UINT8* TNGRTTResponsePacket_unpack(UINT8* buf, UINT32 len, struct TNGRTTResponsePacket* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->dummy0 = (*off>>7)&1;
    {
	pkt->dummy1  = (*off&0x7c)>>2;
    }
    pkt->dummy2 = (*off>>1)&1;
    pkt->dummy3 = *off++&1;
    {pkt->packet_type = *off++<<8; pkt->packet_type |= *off++;}
    {
	pkt->timestamp_sec = ((UINT32)*off++)<<24; pkt->timestamp_sec |= ((UINT32)*off++)<<16;
	pkt->timestamp_sec |= ((UINT32)*off++)<<8; pkt->timestamp_sec |= ((UINT32)*off++);
    }
    {
	pkt->timestamp_usec = ((UINT32)*off++)<<24; pkt->timestamp_usec |= ((UINT32)*off++)<<16;
	pkt->timestamp_usec |= ((UINT32)*off++)<<8; pkt->timestamp_usec |= ((UINT32)*off++);
    }
    return off;
}


const UINT32 TNGCongestionPacket_static_size() {return 11;}

UINT8* TNGCongestionPacket_pack(UINT8* buf, UINT32 len, struct TNGCongestionPacket* pkt)
{
    UINT8* off = buf;

    {*off &= ~(1<<7); *off |= (pkt->dummy0&1)<<7;}
    {
	*off &= ~0x7c; *off |= (pkt->dummy1&0x1f)<<2;
    }
    {*off &= ~(1<<1); *off |= (pkt->dummy2&1)<<1;}
    {*off &= ~1; *off++ |= pkt->dummy3&1;}
    {*off++ = (UINT8) (pkt->packet_type>>8); *off++ = (UINT8) (pkt->packet_type);}
    {
	*off++ = (UINT8) (pkt->xmit_multiplier>>24); *off++ = (UINT8) (pkt->xmit_multiplier>>16); *off++ = (UINT8) (pkt->xmit_multiplier>>8); *off++ = (UINT8) (pkt->xmit_multiplier);
    }
    {
	*off++ = (UINT8) (pkt->recv_multiplier>>24); *off++ = (UINT8) (pkt->recv_multiplier>>16); *off++ = (UINT8) (pkt->recv_multiplier>>8); *off++ = (UINT8) (pkt->recv_multiplier);
    }
    len = off-buf;
    return off;
}

UINT8* TNGCongestionPacket_unpack(UINT8* buf, UINT32 len, struct TNGCongestionPacket* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->dummy0 = (*off>>7)&1;
    {
	pkt->dummy1  = (*off&0x7c)>>2;
    }
    pkt->dummy2 = (*off>>1)&1;
    pkt->dummy3 = *off++&1;
    {pkt->packet_type = *off++<<8; pkt->packet_type |= *off++;}
    {
	pkt->xmit_multiplier = ((INT32)*off++)<<24; pkt->xmit_multiplier |= ((INT32)*off++)<<16;
	pkt->xmit_multiplier |= ((INT32)*off++)<<8; pkt->xmit_multiplier |= ((INT32)*off++);
    }
    {
	pkt->recv_multiplier = ((INT32)*off++)<<24; pkt->recv_multiplier |= ((INT32)*off++)<<16;
	pkt->recv_multiplier |= ((INT32)*off++)<<8; pkt->recv_multiplier |= ((INT32)*off++);
    }
    return off;
}

const UINT32 TNGStreamEndPacket_static_size() {return 20;}

UINT8* TNGStreamEndPacket_pack(UINT8* buf, UINT32 len, struct TNGStreamEndPacket* pkt)
{
    UINT8* off = buf;

    {*off &= ~(1<<7); *off |= (pkt->need_reliable_flag&1)<<7;}
    {
	*off &= ~0x7c; *off |= (pkt->stream_id&0x1f)<<2;
    }
    {*off &= ~(1<<1); *off |= (pkt->packet_sent&1)<<1;}
    {*off &= ~1; *off++ |= pkt->ext_flag&1;}
    {*off++ = (UINT8) (pkt->packet_type>>8); *off++ = (UINT8) (pkt->packet_type);}
    {*off++ = (UINT8) (pkt->seq_no>>8); *off++ = (UINT8) (pkt->seq_no);}
    {
	*off++ = (UINT8) (pkt->timestamp>>24); *off++ = (UINT8) (pkt->timestamp>>16); *off++ = (UINT8) (pkt->timestamp>>8); *off++ = (UINT8) (pkt->timestamp);
    }
    if ((pkt->stream_id== 31))
    {
      {*off++ = (UINT8) (pkt->stream_id_expansion>>8); *off++ = (UINT8) (pkt->stream_id_expansion);}
    }
    if ((pkt->need_reliable_flag== 1))
    {
      {*off++ = (UINT8) (pkt->total_reliable>>8); *off++ = (UINT8) (pkt->total_reliable);}
    }
    if ((pkt->ext_flag== 1))
    {
      {memcpy(off, pkt->reason_dummy, 3); off += 3; }
      {
	  *off++ = (UINT8) (pkt->reason_code>>24); *off++ = (UINT8) (pkt->reason_code>>16); *off++ = (UINT8) (pkt->reason_code>>8); *off++ = (UINT8) (pkt->reason_code);
      }
      {
	  int _len = strlen(pkt->reason_text); *off++ = _len>>8; *off++ = _len;
	  memcpy(off, pkt->reason_text, _len); off += _len;
      }
    }
    len = off-buf;
    return off;
}

UINT8* TNGStreamEndPacket_unpack(UINT8* buf, UINT32 len, struct TNGStreamEndPacket* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->need_reliable_flag = (*off>>7)&1;
    {
	pkt->stream_id  = (*off&0x7c)>>2;
    }
    pkt->packet_sent = (*off>>1)&1;
    pkt->ext_flag = *off++&1;
    {pkt->packet_type = *off++<<8; pkt->packet_type |= *off++;}
    {pkt->seq_no = *off++<<8; pkt->seq_no |= *off++;}
    {
	pkt->timestamp = ((UINT32)*off++)<<24; pkt->timestamp |= ((UINT32)*off++)<<16;
	pkt->timestamp |= ((UINT32)*off++)<<8; pkt->timestamp |= ((UINT32)*off++);
    }
    if ((pkt->stream_id== 31))
    {
      {pkt->stream_id_expansion = *off++<<8; pkt->stream_id_expansion |= *off++;}
    }
    if ((pkt->need_reliable_flag== 1))
    {
      {pkt->total_reliable = *off++<<8; pkt->total_reliable |= *off++;}
    }
    if ((pkt->ext_flag== 1))
    {
      if (off-buf+3 > (int)len)
	  return 0;
      {memcpy(pkt->reason_dummy, off, 3); off += 3; }
      {
	  pkt->reason_code = ((UINT32)*off++)<<24; pkt->reason_code |= ((UINT32)*off++)<<16;
	  pkt->reason_code |= ((UINT32)*off++)<<8; pkt->reason_code |= ((UINT32)*off++);
      }
      {
	  int _len = *off++>>8; _len |= *off++;
	  if (off-buf+_len > (int)len)
	      return 0;
	  pkt->reason_text = (char*)rm_memory_default_malloc(NULL, _len+1 *sizeof(char));
	  memcpy(pkt->reason_text, (char*)off, _len); off += _len; pkt->reason_text[_len] = 0;
      }
    }
    return off;
}


UINT8* TNGStreamEndPacket_unpack_fromPool(UINT8* buf,
                                          UINT32 len,
                                          struct TNGStreamEndPacket* pkt,
                                          rm_malloc_func_ptr fpMalloc,
                                          void* pMemoryPool)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->need_reliable_flag = (*off>>7)&1;
    {
	pkt->stream_id  = (*off&0x7c)>>2;
    }
    pkt->packet_sent = (*off>>1)&1;
    pkt->ext_flag = *off++&1;
    {pkt->packet_type = *off++<<8; pkt->packet_type |= *off++;}
    {pkt->seq_no = *off++<<8; pkt->seq_no |= *off++;}
    {
	pkt->timestamp = ((UINT32)*off++)<<24; pkt->timestamp |= ((UINT32)*off++)<<16;
	pkt->timestamp |= ((UINT32)*off++)<<8; pkt->timestamp |= ((UINT32)*off++);
    }
    if ((pkt->stream_id== 31))
    {
      {pkt->stream_id_expansion = *off++<<8; pkt->stream_id_expansion |= *off++;}
    }
    if ((pkt->need_reliable_flag== 1))
    {
      {pkt->total_reliable = *off++<<8; pkt->total_reliable |= *off++;}
    }
    if ((pkt->ext_flag== 1))
    {
      if (off-buf+3 > (int)len)
	  return 0;
      {memcpy(pkt->reason_dummy, off, 3); off += 3; }
      {
	  pkt->reason_code = ((UINT32)*off++)<<24; pkt->reason_code |= ((UINT32)*off++)<<16;
	  pkt->reason_code |= ((UINT32)*off++)<<8; pkt->reason_code |= ((UINT32)*off++);
      }
      {
	  int _len = *off++>>8; _len |= *off++;
	  if (off-buf+_len > (int)len)
	      return 0;
	  pkt->reason_text =(char*)fpMalloc(pMemoryPool, _len+1 *sizeof(char));
	  memcpy(pkt->reason_text, (char*)off, _len); off += _len; pkt->reason_text[_len] = 0;
      }
    }
    return off;
}




const UINT32 TNGLatencyReportPacket_static_size() {return 9;}

UINT8* TNGLatencyReportPacket_pack(UINT8* buf, UINT32 len, struct TNGLatencyReportPacket* pkt)
{
    UINT8* off = buf;

    {*off &= ~(1<<7); *off |= (pkt->length_included_flag&1)<<7;}
    {
	*off &= ~0x7c; *off |= (pkt->dummy0&0x1f)<<2;
    }
    {*off &= ~(1<<1); *off |= (pkt->dummy1&1)<<1;}
    {*off &= ~1; *off++ |= pkt->dummy2&1;}
    {*off++ = (UINT8) (pkt->packet_type>>8); *off++ = (UINT8) (pkt->packet_type);}
    if ((pkt->length_included_flag== 1))
    {
      {*off++ = (UINT8) (pkt->length>>8); *off++ = (UINT8) (pkt->length);}
    }
    {
	*off++ = (UINT8) (pkt->server_out_time>>24); *off++ = (UINT8) (pkt->server_out_time>>16); *off++ = (UINT8) (pkt->server_out_time>>8); *off++ = (UINT8) (pkt->server_out_time);
    }
    len = off-buf;
    return off;
}

UINT8* TNGLatencyReportPacket_unpack(UINT8* buf, UINT32 len, struct TNGLatencyReportPacket* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->length_included_flag = (*off>>7)&1;
    {
	pkt->dummy0  = (*off&0x7c)>>2;
    }
    pkt->dummy1 = (*off>>1)&1;
    pkt->dummy2 = *off++&1;
    {pkt->packet_type = *off++<<8; pkt->packet_type |= *off++;}
    if ((pkt->length_included_flag== 1))
    {
      {pkt->length = *off++<<8; pkt->length |= *off++;}
    }
    {
	pkt->server_out_time = ((UINT32)*off++)<<24; pkt->server_out_time |= ((UINT32)*off++)<<16;
	pkt->server_out_time |= ((UINT32)*off++)<<8; pkt->server_out_time |= ((UINT32)*off++);
    }
    return off;
}


    /*
     * RDTFeatureLevel 3 packets
     */


const UINT32 RDTTransportInfoRequestPacket_static_size() {return 7;}

UINT8* RDTTransportInfoRequestPacket_pack(UINT8* buf, UINT32 len, struct RDTTransportInfoRequestPacket* pkt)
{
    UINT8* off = buf;

    {*off &= ~(1<<7); *off |= (pkt->dummy0&1)<<7;}
    {
	*off &= ~0x7c; *off |= (pkt->dummy1&0x1f)<<2;
    }
    {*off &= ~(1<<1); *off |= (pkt->request_rtt_info&1)<<1;}
    {*off &= ~1; *off++ |= pkt->request_buffer_info&1;}
    {*off++ = (UINT8) (pkt->packet_type>>8); *off++ = (UINT8) (pkt->packet_type);}
    if ((pkt->request_rtt_info== 1))
    {
      {
	  *off++ = (UINT8) (pkt->request_time_ms>>24); *off++ = (UINT8) (pkt->request_time_ms>>16); *off++ = (UINT8) (pkt->request_time_ms>>8); *off++ = (UINT8) (pkt->request_time_ms);
      }
    }
    len = off-buf;
    return off;
}

unsigned char* RDTTransportInfoRequestPacket_unpack(unsigned char* buf, unsigned int len, struct RDTTransportInfoRequestPacket* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->dummy0 = (*off>>7)&1;
    {
	pkt->dummy1  = (*off&0x7c)>>2;
    }
    pkt->request_rtt_info = (*off>>1)&1;
    pkt->request_buffer_info = *off++&1;
    {pkt->packet_type = *off++<<8; pkt->packet_type |= *off++;}
    if ((pkt->request_rtt_info== 1))
    {
      {
	  pkt->request_time_ms = ((UINT32)*off++)<<24; pkt->request_time_ms |= ((UINT32)*off++)<<16;
	  pkt->request_time_ms |= ((UINT32)*off++)<<8; pkt->request_time_ms |= ((UINT32)*off++);
      }
    }
    return off;
}

const UINT32 RDTBufferInfo_static_size() {return 14;}

UINT8* RDTBufferInfo_pack(UINT8* buf, UINT32 len, struct RDTBufferInfo* pkt)
{
    UINT8* off = buf;

    {*off++ = (UINT8) (pkt->stream_id>>8); *off++ = (UINT8) (pkt->stream_id);}
    {
	*off++ = (UINT8) (pkt->lowest_timestamp>>24); *off++ = (UINT8) (pkt->lowest_timestamp>>16); *off++ = (UINT8) (pkt->lowest_timestamp>>8); *off++ = (UINT8) (pkt->lowest_timestamp);
    }
    {
	*off++ = (UINT8) (pkt->highest_timestamp>>24); *off++ = (UINT8) (pkt->highest_timestamp>>16); *off++ = (UINT8) (pkt->highest_timestamp>>8); *off++ = (UINT8) (pkt->highest_timestamp);
    }
    {
	*off++ = (UINT8) (pkt->bytes_buffered>>24); *off++ = (UINT8) (pkt->bytes_buffered>>16); *off++ = (UINT8) (pkt->bytes_buffered>>8); *off++ = (UINT8) (pkt->bytes_buffered);
    }
    len = off-buf;
    return off;
}

UINT8* RDTBufferInfo_unpack(UINT8* buf, UINT32 len, struct RDTBufferInfo* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    {pkt->stream_id = *off++<<8; pkt->stream_id |= *off++;}
    {
	pkt->lowest_timestamp = ((UINT32)*off++)<<24; pkt->lowest_timestamp |= ((UINT32)*off++)<<16;
	pkt->lowest_timestamp |= ((UINT32)*off++)<<8; pkt->lowest_timestamp |= ((UINT32)*off++);
    }
    {
	pkt->highest_timestamp = ((UINT32)*off++)<<24; pkt->highest_timestamp |= ((UINT32)*off++)<<16;
	pkt->highest_timestamp |= ((UINT32)*off++)<<8; pkt->highest_timestamp |= ((UINT32)*off++);
    }
    {
	pkt->bytes_buffered = ((UINT32)*off++)<<24; pkt->bytes_buffered |= ((UINT32)*off++)<<16;
	pkt->bytes_buffered |= ((UINT32)*off++)<<8; pkt->bytes_buffered |= ((UINT32)*off++);
    }
    return off;
}

const UINT32 RDTTransportInfoResponsePacket_static_size() {return 13;}

UINT8* RDTTransportInfoResponsePacket_pack(UINT8* buf, UINT32 len, struct RDTTransportInfoResponsePacket* pkt)
{
    UINT8* off = buf;

    {*off &= ~(1<<7); *off |= (pkt->dummy0&1)<<7;}
    {
	*off &= ~0x78; *off |= (pkt->dummy1&0xf)<<3;
    }
    {*off &= ~(1<<2); *off |= (pkt->has_rtt_info&1)<<2;}
    {*off &= ~(1<<1); *off |= (pkt->is_delayed&1)<<1;}
    {*off &= ~1; *off++ |= pkt->has_buffer_info&1;}
    {*off++ = (UINT8) (pkt->packet_type>>8); *off++ = (UINT8) (pkt->packet_type);}
    if ((pkt->has_rtt_info== 1))
    {
      {
	  *off++ = (UINT8) (pkt->request_time_ms>>24); *off++ = (UINT8) (pkt->request_time_ms>>16); *off++ = (UINT8) (pkt->request_time_ms>>8); *off++ = (UINT8) (pkt->request_time_ms);
      }
      if ((pkt->is_delayed== 1))
      {
	{
	    *off++ = (UINT8) (pkt->response_time_ms>>24); *off++ = (UINT8) (pkt->response_time_ms>>16); *off++ = (UINT8) (pkt->response_time_ms>>8); *off++ = (UINT8) (pkt->response_time_ms);
	}
      }
    }
    if ((pkt->has_buffer_info== 1))
    {
      {*off++ = (UINT8) (pkt->buffer_info_count>>8); *off++ = (UINT8) (pkt->buffer_info_count);}
      {int i; for(i = 0;  i < pkt->buffer_info_count; i++)
	  off = RDTBufferInfo_pack(off, len, &pkt->buffer_info[i]);
      }
    }
    len = off-buf;
    return off;
}

UINT8* RDTTransportInfoResponsePacket_unpack(UINT8* buf, UINT32 len, struct RDTTransportInfoResponsePacket* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->dummy0 = (*off>>7)&1;
    {
	pkt->dummy1  = (*off&0x78)>>3;
    }
    pkt->has_rtt_info = (*off>>2)&1;
    pkt->is_delayed = (*off>>1)&1;
    pkt->has_buffer_info = *off++&1;
    {pkt->packet_type = *off++<<8; pkt->packet_type |= *off++;}
    if ((pkt->has_rtt_info== 1))
    {
      {
	  pkt->request_time_ms = ((UINT32)*off++)<<24; pkt->request_time_ms |= ((UINT32)*off++)<<16;
	  pkt->request_time_ms |= ((UINT32)*off++)<<8; pkt->request_time_ms |= ((UINT32)*off++);
      }
      if ((pkt->is_delayed== 1))
      {
	{
	    pkt->response_time_ms = ((UINT32)*off++)<<24; pkt->response_time_ms |= ((UINT32)*off++)<<16;
	    pkt->response_time_ms |= ((UINT32)*off++)<<8; pkt->response_time_ms |= ((UINT32)*off++);
	}
      }
    }
    if ((pkt->has_buffer_info== 1))
    {
      {pkt->buffer_info_count = *off++<<8; pkt->buffer_info_count |= *off++;}
      {
	  pkt->buffer_info = (RDTBufferInfo*)rm_memory_default_malloc(NULL, sizeof(struct RDTBufferInfo)*pkt->buffer_info_count);
	  {int i; for (i = 0;  i < pkt->buffer_info_count; i++)
	  off = RDTBufferInfo_unpack(off, len, (RDTBufferInfo*)&pkt->buffer_info[i]);
	  }
      }
    }
    return off;
}


UINT8* RDTTransportInfoResponsePacket_unpack_fromPool(UINT8* buf,
                                                      UINT32 len,
                                                      struct RDTTransportInfoResponsePacket* pkt,
                                                      rm_malloc_func_ptr fpMalloc,
                                                      void* pMemoryPool)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->dummy0 = (*off>>7)&1;
    {
	pkt->dummy1  = (*off&0x78)>>3;
    }
    pkt->has_rtt_info = (*off>>2)&1;
    pkt->is_delayed = (*off>>1)&1;
    pkt->has_buffer_info = *off++&1;
    {pkt->packet_type = *off++<<8; pkt->packet_type |= *off++;}
    if ((pkt->has_rtt_info== 1))
    {
      {
	  pkt->request_time_ms = ((UINT32)*off++)<<24; pkt->request_time_ms |= ((UINT32)*off++)<<16;
	  pkt->request_time_ms |= ((UINT32)*off++)<<8; pkt->request_time_ms |= ((UINT32)*off++);
      }
      if ((pkt->is_delayed== 1))
      {
	{
	    pkt->response_time_ms = ((UINT32)*off++)<<24; pkt->response_time_ms |= ((UINT32)*off++)<<16;
	    pkt->response_time_ms |= ((UINT32)*off++)<<8; pkt->response_time_ms |= ((UINT32)*off++);
	}
      }
    }
    if ((pkt->has_buffer_info== 1))
    {
      {pkt->buffer_info_count = *off++<<8; pkt->buffer_info_count |= *off++;}
      {
	  pkt->buffer_info = (RDTBufferInfo*)fpMalloc(pMemoryPool, sizeof(struct RDTBufferInfo)*pkt->buffer_info_count);
	  {int i; for (i = 0;  i < pkt->buffer_info_count; i++)
	  off = RDTBufferInfo_unpack(off, len, (RDTBufferInfo*)&pkt->buffer_info[i]);
	  }
      }
    }
    return off;
}



const UINT32 TNGBWProbingPacket_static_size() {return 10;}

UINT8* TNGBWProbingPacket_pack(UINT8* buf, UINT32 len, struct TNGBWProbingPacket* pkt)
{
    UINT8* off = buf;

    {*off &= ~(1<<7); *off |= (pkt->length_included_flag&1)<<7;}
    {
	*off &= ~0x7c; *off |= (pkt->dummy0&0x1f)<<2;
    }
    {*off &= ~(1<<1); *off |= (pkt->dummy1&1)<<1;}
    {*off &= ~1; *off++ |= pkt->dummy2&1;}
    {*off++ = (UINT8) (pkt->packet_type>>8); *off++ = (UINT8) (pkt->packet_type);}
    if ((pkt->length_included_flag== 1))
    {
      {*off++ = (UINT8) (pkt->length>>8); *off++ = (UINT8) (pkt->length);}
    }
    *off++ = pkt->seq_no;
    {
	*off++ = (UINT8) (pkt->timestamp>>24); *off++ = (UINT8) (pkt->timestamp>>16); *off++ = (UINT8) (pkt->timestamp>>8); *off++ = (UINT8) (pkt->timestamp);
    }
    {
	if (pkt->data.data) memcpy(off, pkt->data.data, pkt->data.len);
	off += pkt->data.len;
    }
    len = off-buf;
    return off;
}

UINT8* TNGBWProbingPacket_unpack(UINT8* buf, UINT32 len, struct TNGBWProbingPacket* pkt)
{
    UINT8* off = buf;

    if (!buf || !len)
	return 0;
    pkt->length_included_flag = (*off>>7)&1;
    {
	pkt->dummy0  = (*off&0x7c)>>2;
    }
    pkt->dummy1 = (*off>>1)&1;
    pkt->dummy2 = *off++&1;
    {pkt->packet_type = *off++<<8; pkt->packet_type |= *off++;}
    if ((pkt->length_included_flag== 1))
    {
      {pkt->length = *off++<<8; pkt->length |= *off++;}
    }
    pkt->seq_no = *off++;
    {
	pkt->timestamp = ((UINT32)*off++)<<24; pkt->timestamp |= ((UINT32)*off++)<<16;
	pkt->timestamp |= ((UINT32)*off++)<<8; pkt->timestamp |= ((UINT32)*off++);
    }
    {
	pkt->data.len = len - (off - buf);
	if (off-buf+pkt->data.len > (int)len)
	    return 0;
	pkt->data.data = (INT8 *)off; off += pkt->data.len;
    }
    return off;
}

#ifdef _VONAMESPACE
}
#endif