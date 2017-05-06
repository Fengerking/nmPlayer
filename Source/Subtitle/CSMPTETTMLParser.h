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
* @file CSMPTETTMLParser.h
*
* @author  Dolby Du
* 
* Change History
* 2013-6-18    Create File
************************************************************************/

#ifndef __CSMPTETTMLParser_H__
#define __CSMPTETTMLParser_H__

#include "CTimedTextMLParser.h"

class	CSMPTETTMLParser : public CTimedTextMLParser
{
public:
	CSMPTETTMLParser();
	virtual ~CSMPTETTMLParser();
protected:
	bool	ParseLine (char* pPage, char* pDivEnd);
	VO_BOOL	GetTwoValueFromPercent(char* pString, VO_U32& uValue1, VO_U32& uValue2);
private:
	ImageInfo	m_ImageInfo;///<currently just support one picture
};

#endif // __CSMPTETTMLParser_H__
