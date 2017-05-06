//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    floor.h

Abstract:

    floor and mapping header file.

Author:

    Witten Wen 07-September-2009

Revision History:

*************************************************************************/

#ifndef __FLOOR_H_
#define __FLOOR_H_

#include "macros.h"
#include "vovorbisdec.h"

typedef struct{
	int   m_Order;//order;
	long  m_Rate;//rate;
	long  m_BarkMap;//barkmap;

	int   m_AmpBits;//ampbits;
	int   m_AmpDB;//ampdB;

	int   m_NumBooks;//numbooks; /* <= 16 */
	int   m_Books[16];//books[16];

} VorbisInfoFloor0;	//vorbis_info_floor0

#define VIF_POSIT 63
#define VIF_CLASS 16
#define VIF_PARTS 31
typedef struct{
  int   m_Partitions;//partitions;                /* 0 to 31 */
  int   m_ParttClass[VIF_PARTS];//partitionclass[VIF_PARTS]; /* 0 to 15 */

  int   m_ClassDim[VIF_CLASS];//class_dim[VIF_CLASS];        /* 1 to 8 */
  int   m_ClassSubs[VIF_CLASS];//class_subs[VIF_CLASS];       /* 0,1,2,3 (bits: 1<<n poss) */
  int   m_ClassBook[VIF_CLASS];//class_book[VIF_CLASS];       /* subs ^ dim entries */
  int   m_ClassSubbook[VIF_CLASS][8];//class_subbook[VIF_CLASS][8]; /* [VIF_CLASS][subs] */


  int   m_Mult;//mult;                      /* 1 2 3 or 4 */ 
  int   m_PostList[VIF_POSIT+2];//postlist[VIF_POSIT+2];    /* first two implicit */ 

} VorbisInfoFloor1;	//vorbis_info_floor1

typedef struct {
	long m_N;//n;
	int  m_Ln;//ln;
	int  m_M;//m;
	int *m_pLinearMap;//linearmap;

	VorbisInfoFloor0 *vi;
	OGG_S32 *m_pLSPLook;//lsp_look;
	OGG_S32 *m_pIlsp;//ilsp;
} VorbisLookFloor0;//vorbis_look_floor0;

typedef struct {
  int m_ForwardIndex[VIF_POSIT+2];//forward_index[VIF_POSIT+2];
  
  int m_HiNeighbor[VIF_POSIT];//hineighbor[VIF_POSIT];
  int m_LoNeighbor[VIF_POSIT];//loneighbor[VIF_POSIT];
  int m_Posts;//posts;

  int m_N;//n;
  int m_QuantQ;//quant_q;
  VorbisInfoFloor1 *vi;

} VorbisLookFloor1;	//vorbis_look_floor1

/* Floor backend generic *****************************************/
typedef struct{
  VorbisInfoFloor     *(*unpack)(CVOVorbisObjectDecoder *, VorbisInfo *, VOOGGInputBitStream *);
  VorbisLookFloor     *(*look)  (CVOVorbisObjectDecoder *pvorbisdec, 
									VorbisDSPState *, 
									VorbisInfoMode *,
									VorbisInfoFloor *);
  void (*free_info) (CVOVorbisObjectDecoder *pvorbisdec, VorbisInfoFloor *);
  void (*free_look) (CVOVorbisObjectDecoder *pvorbisdec, VorbisLookFloor *);
  void *(*inverse1)  (CVOVorbisObjectDecoder *pvorbisdec, struct VorbisBlock *, VorbisLookFloor *);
  int   (*inverse2)  (CVOVorbisObjectDecoder *pvorbisdec, struct VorbisBlock *, VorbisLookFloor *,
		     void *buffer, OGG_S32 *);
} VorbisFuncFloor;	//vorbis_func_floor

extern VorbisFuncFloor floor0_exportbundle;
extern VorbisFuncFloor floor1_exportbundle;

#endif	//__FLOOR_H_

