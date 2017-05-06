

#ifndef _NALU_H_
#define _NALU_H_

#if !BUILD_WITHOUT_C_LIB
#include <stdio.h>
#endif

#include "nalucommon.h"

avdInt32 GetAnnexbNALU (NALU_t *nalu);
avdInt32 NALUtoRBSP (ImageParameters *img,NALU_t *nalu);

#endif
