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
* @file CLrcParser.h
*
* @author  Ferry Zhang
* 
* Change History
* 2013-01-08    Create File
************************************************************************/


#ifndef __CLrcParser_H__
#define __CLrcParser_H__

#include "CBaseSubtitleParser.h"


class	CLrcParser : CBaseSubtitleParser
{
public:
	CLrcParser(void);
	virtual ~CLrcParser(void);

public:
	virtual	bool	Parse (void);
};

#endif // __CSrtParser_H__

