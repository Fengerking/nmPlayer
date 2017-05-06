/************************************************************************
*									                                    *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2010	        *
*									     *
************************************************************************/
/***********************************************************************
File:		voAPE_D_API.h

Contains:	APE Decoder API function declaration

Written by:	Huaping Liu

Change History (most recent first):
2009-11-11		LHP			Create file

*************************************************************************/
#ifndef __VOAPE_D_API_H__
#define __VOAPE_D_API_H__

#include "voTypedef.h"


//APE bit rate mode
#define COMPRESSION_LEVEL_FAST          1000
#define COMPRESSION_LEVEL_NORMAL        2000
#define COMPRESSION_LEVEL_HIGH          3000
#define COMPRESSION_LEVEL_EXTRA_HIGH    4000
#define COMPRESSION_LEVEL_INSANE        5000

int voAPEMetaInfo(VOPTR hCodec, unsigned char* inbuf, int length);

#endif   //__VOAPE_D_API_H__


