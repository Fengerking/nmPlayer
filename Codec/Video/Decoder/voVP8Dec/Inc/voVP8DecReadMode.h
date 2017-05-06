#ifndef __VP8_DEC_READMODE_H
#define __VP8_DEC_READMODE_H
#include "voVP8DecFrame.h"
#include "voVP8DecFindNearMV.h"
#include "voVP8DecReadBits.h"

void vp8_read_mb_features(vp8_reader *r, MB_MODE_INFO *mi, MACROBLOCKD *x);

#endif