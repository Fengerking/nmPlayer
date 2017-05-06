//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    ogg.h

Abstract:

    Ogg FIFO header file.

Author:

    Witten Wen 04-September-2009

Revision History:

*************************************************************************/
#ifndef __OGG_H_
#define __OGG_H_

#include "macros.h"

//#define MAX_DATA_SIZE 0x4000

typedef struct OGGStreamState {

	int            m_EOS;//e_o_s;    
								/* set when we have buffered the last 
							 packet in the logical bitstream */
	int            m_BOS;//b_o_s;    
							/* set after we've written the initial page
							 of a logical bitstream */
	long           m_SerialNo;//serialno;
	long           m_PageNo;//pageno;
	OGG_S64        m_PacketNo;//packetno; 
						/* sequence number for decode; the framing
						 knows where there's a hole in the data,
						 but we need coupling so that the codec
						 (which is in a seperate abstraction
						 layer) also knows about the gap */
	OGG_S64			m_GranulePos;//granulepos;

	//  int            lacing_fill;
	//  OGG_U32   body_fill;

	/* decode-side state data */
	//  int            holeflag;
	//	int            m_SpanFlag;
	//  int            clearflag;
	//  int            laceptr;
	//  OGG_U32   body_fill_next;
	VO_U8			m_LaceSep[256];
	int				m_LaceSize;
	int				m_Laceptr;
	int				m_CurLaceLength;
	int				m_SpanLaceStore;
	VO_BOOL			m_LaceEnd;
	VO_BOOL			m_InitFlag;
	VO_BOOL			m_ResetFlag;
} OGGStreamState;

//typedef struct {
//  ogg_reference *header;
//  int            header_len;
//  ogg_reference *body;
//  long           body_len;
//} OggPage;

typedef struct {
	struct PacketStage    *m_PacketStage;
//    OGGStreamState *packet;
//    long           bytes;
    long			m_BOS;
    long			m_EOS;
    OGG_S64			m_GranulePos;
    OGG_S64			m_PacketNo;     /* sequence number for decode; the framing
                                  knows where there's a hole in the data,
                                  but we need coupling so that the codec
                                  (which is in a seperate abstraction
                                  layer) also knows about the gap */
} OGGPacket;

//extern int ogg_sync_wrote(ogg_sync_state *oy, long bytes);

/* Ogg BITSTREAM PRIMITIVES: return codes ***************************/

#define  OGG_SUCCESS   0

#define  OGG_HOLE     -10
#define  OGG_SPAN     -11
#define  OGG_EVERSION -12
#define  OGG_ESERIAL  -13
#define  OGG_EINVAL   -14
#define  OGG_EEOS     -15
#define  OGG_NEXT	  -16

extern void voOGGDecResetOGGStreamState(OGGStreamState *os);
extern void RestOGGStreamStateSpan(OGGStreamState *os);
extern int	voOGGDecStreamReset(OGGStreamState *os);
#endif	//__OGG_H_
