/************************************************************************
VisualOn Proprietary
Copyright (c) 2003, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
/************************************************************************
* @file CFileFormatConverter.cpp
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#ifndef __CFileFormatConverter_H__
#define __CFileFormatConverter_H__

#include "voType.h"
#include "voString.h"


class	CFileFormatConverter
{
public:
	CFileFormatConverter(void);
	virtual ~CFileFormatConverter(void);

	virtual	bool Convert(void * env, VO_BYTE* o_szText, int *pnSize );
};

#endif // __CFileFormatConverter_H__
