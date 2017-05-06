//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    mapping.h

Abstract:

    channel mapping 0 unpack header file.

Author:

    Witten Wen 10-October-2009

Revision History:

*************************************************************************/

#ifndef __MAPPING_H_
#define __MAPPING_H_

#include "floor.h"
#include "residue.h"

typedef struct VorbisInfoMapping0{
  int   m_Submaps;//submaps;  /* <= 16 */
  int   m_ChMuxList[256];//chmuxlist[256];   /* up to 256 channels in a Vorbis stream */
  
  int   m_FloorSubmap[16];//floorsubmap[16];   /* [mux] submap to floors */
  int   m_ResidueSubmap[16];//residuesubmap[16]; /* [mux] submap to residue */

/*  int   psy[2];  by blocktype; impulse/padding for short,
                   transition/normal for long */

  int   m_CouplingSteps;//coupling_steps;
  int   m_CouplingMag[256];//coupling_mag[256];
  int   m_CouplingAng[256];//coupling_ang[256];
} VorbisInfoMapping0;		//VorbisInfoMapping0

/* simplistic, wasteful way of doing this (unique lookup for each
   mode/submapping); there should be a central repository for
   identical lookups.  That will require minor work, so I'm putting it
   off as low priority.

   Why a lookup for each backend in a given mode?  Because the
   blocksize is set by the mode, and low backend lookups may require
   parameters from other areas of the mode/mapping */

typedef struct {
//  VorbisInfoMode		*mode;
  VorbisInfoMapping0	*m_pMap;//map;

  VorbisLookFloor		**m_ppFloorLook;//floor_look;

  VorbisLookResidue		**m_ppResidueLook;//residue_look;

  VorbisFuncFloor		**m_ppFloorFunc;//floor_func;
  VorbisFuncResidue		**m_ppResidueFunc;//residue_func;

//  int			m_Ch;//ch;
//  long			lastframe; /* if a different mode is called, we need to invalidate decay */

  /* Witten added to avoid allocing memory in mapping reverse function */
  OGG_S32		**m_ppPCMBundle;
  void			**m_ppFloorMemo;     // m_ppFloorMemo[ch][i] has int type, bit0~bit15 is data, bit16 is a floor1_step2_flag  
  int			*m_pZeroBundle;
  int			*m_pNonzero;  
} VorbisLookMapping0;	//vorbis_look_mapping0

/* Mapping backend generic *****************************************/
typedef struct{
  VorbisInfoMapping *(*unpack)(CVOVorbisObjectDecoder *, VorbisInfo *);
  VorbisLookMapping *(*look)  (CVOVorbisObjectDecoder *, VorbisDSPState *,
								VorbisInfoMode *, VorbisInfoMapping *);
  void (*free_info)    (CVOVorbisObjectDecoder *, VorbisInfoMapping *);
  void (*free_look)    (CVOVorbisObjectDecoder *, VorbisLookMapping *);
  int  (*inverse)      (CVOVorbisObjectDecoder *, struct VorbisBlock *vb,VorbisLookMapping *);
} VorbisFuncMapping;		//vorbis_func_mapping

extern VorbisFuncMapping mapping0_exportbundle;

#endif	//__MAPPING_H_
