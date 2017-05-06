
#ifndef __RVLC_H
#define __RVLC_H

#include "voMpeg4DecGlobal.h"

#define RVLC_BITS 9
/* run length table */
typedef struct RL_VLC_ELEM {
    VO_S16 level;
    VO_S8 len;
    VO_U8 run;
} RL_VLC_ELEM;


extern VOCONST RL_VLC_ELEM rvld_inter[1072];
extern VOCONST RL_VLC_ELEM rvld_intra[1072];
#endif
