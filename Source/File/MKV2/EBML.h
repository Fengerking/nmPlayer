
#pragma once

#include "voType.h"

//convert EBML vint to int64
//return 0 means succeed
//return 1 means bytelength is not enough, you can check bytelength to know the size needed
VO_U8 TranslateVINT( VO_U8 * pByte , VO_U8 * bytelength , VO_S64 * pResult );

//convert EBML vsint to int64
//return 0 means succeed
//return 1 means bytelength is not enough, you can check bytelength to know the size needed
const VO_S64 gc_vsintsub[] =
{ 0x3F, 0x1FFF, 0x0FFFFF, 0x07FFFFFF,
0x03FFFFFFFFLL, 0x01FFFFFFFFFFLL,
0x00FFFFFFFFFFFFLL, 0x007FFFFFFFFFFFFFLL };

VO_U8 TranslateVSINT( VO_U8 * pByte , VO_U8 * bytelength , VO_S64 * pResult );