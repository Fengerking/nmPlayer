/*
   Noiseless coder module
 */
#ifndef __DYN_BITS_H
#define __DYN_BITS_H

#include "psy_const.h"
#include "tns.h"
#include "bit_cnt.h"



#define MAX_SECTIONS          MAX_GROUPED_SFB
#define SECT_ESC_VAL_LONG    31
#define SECT_ESC_VAL_SHORT    7
#define CODE_BOOK_BITS        4
#define SECT_BITS_LONG        5
#define SECT_BITS_SHORT       3

typedef struct
{
  Word16 codeBook;
  Word16 sfbStart;
  Word16 sfbCnt;
  Word16 sectionBits;
}
SECTION_INFO;




typedef struct
{
  Word16 blockType;
  Word16 noOfGroups;
  Word16 sfbCnt;
  Word16 maxSfbPerGroup;
  Word16 sfbPerGroup;
  Word16 noOfSections;
  SECTION_INFO sectionInfo[MAX_SECTIONS];
  Word16 sideInfoBits;             /* sectioning bits       */
  Word16 huffmanBits;              /* huffman    coded bits */
  Word16 scalefacBits;             /* scalefac   coded bits */
  Word16 firstScf;                 /* first scf to be coded */
  Word16 bitLookUp[MAX_SFB_LONG*(CODE_BOOK_ESC_NDX+1)];
  Word16 mergeGainLookUp[MAX_SFB_LONG];
}
SECTION_DATA; /*  Word16 size: 10 + 60(MAX_SECTIONS)*4(SECTION_INFO) + 51(MAX_SFB_LONG)*12(CODE_BOOK_ESC_NDX+1) + 51(MAX_SFB_LONG) = 913 */


Word16 BCInit(void);

Word16 dynBitCount(const Word16 *quantSpectrum,
                   const UWord16 *maxValueInSfb,
                   const Word16 *scalefac,
                   const Word16 blockType,
                   const Word16 sfbCnt,
                   const Word16 maxSfbPerGroup,
                   const Word16 sfbPerGroup,
                   const Word16 *sfbOffset,
                   SECTION_DATA *sectionData);

Word16 RecalcSectionInfo(SECTION_DATA * sectionData,
                         const UWord16 *maxValueInSfb,
                         const Word16 *scalefac,
                         const Word16 blockType);

#endif
