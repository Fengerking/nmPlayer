/****************************************************************************
 *        
 *   Module Title : yv12extend.h
 *
 *   Description  : functions for extending frame buffer into motion 
 *                  border 
 *
 *   Copyright (c) 1999 - 2005  On2 Technologies Inc. All Rights Reserved.
 *
 ***************************************************************************/
#ifndef YV12EXTEND_H
#define YV12EXTEND_H

#include "yv12config.h"

#ifdef __cplusplus
extern "C"
{
#endif

void On2YV12_ExtendFrameBorders (YV12_BUFFER_CONFIG *ybf);

#ifdef __cplusplus
}
#endif

#endif

