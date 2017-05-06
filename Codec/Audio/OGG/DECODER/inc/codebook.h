//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    codebook.h

Abstract:

    basic shared codebook operations.

Author:

    Witten Wen 07-September-2009

Revision History:

*************************************************************************/

#ifndef __CODE_BOOK_H_
#define __CODE_BOOK_H_

#include "ogg.h"

/* These structures encapsulate huffman and VQ style encoding books; it
   doesn't do anything specific to either.

   m_pValueList/m_pQuantList are nonNULL (and q_* significant) only if
   there's entry->value mapping to be done.

   If encode-side mapping must be done (and thus the entry needs to be
   hunted), the auxiliary encode pointer will point to a decision
   tree.  This is true for both VQ and huffman, however is mostly useful
   with VQ.

   StaticCodebook is built by reading info from bitstream and is deleted after creating CodeBook;
   CodeBook is initilized by function VorbisBookDecodeInit.

*/

typedef struct CodeBook{
  long			m_Dim;                          /* codebook dimensions (elements per vector) */
  long			m_Entries;                      /* codebook entries */
  long			m_UsedEntries;                  /* populated codebook entries */

  /* the below are ordered by bitreversed codeword and only used
     entries are populated */
  int           m_BinaryPoint; 
  OGG_U32		*m_pCodeList;                   /* list of bitstream codewords for each entry */
  OGG_S32		*m_pValueList;                  /* list of dim*entries actual entry values */  


  int			*m_pDecIndex;
  char			*m_pDecCodeLengths;
  OGG_U32		*m_pDecFirstTable;
  int           m_DecFirstTableN;
  int           m_DecMAXLength;

} CodeBook;

typedef struct StaticCodebook{
  long   m_Dim;                                 /* codebook dimensions (elements per vector) */
  long   m_Entries;                             /* codebook entries */
  long   *m_pLenthList;                         /* codeword lengths in bits */

  /* mapping ***************************************************************/
  int    m_MapType;      
                                                /* 0=none
                                                   1=implicitly populated values from map column 
                                                   2=listed arbitrary values */

  /* The below does a linear, single monotonic sequence mapping. */
  long     m_Qdelta;                            /* packed 32 bit float; val 1 - val 0 == delta */
  long     m_Qmin;                              /* packed 32 bit float; quant value 0 maps to minval */
  int      m_Qsequencep;                        /* bitflag */
  int      m_Qquant;                            /* bits: 0 < quant <= 16 */
  

  long     *m_pQuantList;
                                                /* map == 1: (int)(entries^(1/dim)) element column map
                                                   map == 2: list of dim*entries quantized entry vals
                                                 */
} StaticCodebook;

extern int ILOG(unsigned int);		
extern int _ILOG(unsigned int v);	
extern long voOGGDecBookMaptype1Quantvals(const StaticCodebook *b);	
extern OGG_U32 bitreverse(OGG_U32 x);

#endif	//__CODE_BOOK_H_


