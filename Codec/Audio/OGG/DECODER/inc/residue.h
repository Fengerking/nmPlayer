//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    residue.h

Abstract:

    residue backend 0, 1 and 2 unpack header file.

Author:

    Witten Wen 9-October-2009

Revision History:

*************************************************************************/

#ifndef __RESIDUE_H_
#define __RESIDUE_H_

#include "codebook.h"

typedef struct VorbisInfoResidue0
{
	/* block-partitioned VQ coded straight residue */
	long  m_Begin;//begin;
	long  m_End;//end;

	/* first stage (lossless partitioning) */
	int    m_Grouping;//grouping;         /* group n vectors per partition */
	int    m_Partitions;       /* possible codebooks for a partition */
	int    m_GroupBook;//groupbook;        /* huffbook for partitioning */
	int    m_SecondStage[64];//secondstages[64]; /* expanded out to pointers in lookup */
	int    m_BookList[256];//booklist[256];    /* list of second stage books */
} VorbisInfoResidue0;		//vorbis_info_residue0

typedef struct {
	VorbisInfoResidue0 *info;
//	int         map;

	int         m_Parts;//parts;
	int         m_Stages;//stages;
//	CodeBook   *fullbooks;
	CodeBook   *m_pPhraseBook;//phrasebook;
	CodeBook ***m_pppPartBooks;//partbooks;

	int         m_Partvals;//partvals;
	int       **m_ppDecodeMap;//decodemap;

} VorbisLookResidue0;	//vorbis_look_residue0

/* Residue backend generic *****************************************/
typedef struct{
  VorbisInfoResidue *(*unpack)(CVOVorbisObjectDecoder *, VorbisInfo *, VOOGGInputBitStream *);
  VorbisLookResidue *(*look)  (CVOVorbisObjectDecoder *, VorbisDSPState *,
								VorbisInfoMode *, VorbisInfoResidue *);
  void (*free_info)    (CVOVorbisObjectDecoder *, VorbisInfoResidue *);
  void (*free_look)    (CVOVorbisObjectDecoder *, VorbisLookResidue *);
  int  (*inverse)      (CVOVorbisObjectDecoder *, struct VorbisBlock *, VorbisLookResidue *,
			OGG_S32 **,int *,int);
} VorbisFuncResidue;	//vorbis_func_residue

extern VorbisFuncResidue residue0_exportbundle;
extern VorbisFuncResidue residue1_exportbundle;
extern VorbisFuncResidue residue2_exportbundle;

#endif	//__RESIDUE_H_
