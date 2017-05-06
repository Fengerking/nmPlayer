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
* @file CCharserDecoder.h
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#ifndef __CCharsetDecoder_H__
#define __CCharsetDecoder_H__

#include "voType.h"
#include "voSubtitleParser.h"
#if (defined _IOS || defined _MAC_OS) 
#include "voFormatConversion.h"
#endif

class	CCharsetDecoder 
{
public:
	CCharsetDecoder(void);
	virtual ~CCharsetDecoder(void);


protected:
	Subtitle_Language	m_nLanguage;
	Subtitle_File_Format m_nFileFormat;
	char				m_szCharset[256];
#if (defined _IOS || defined _MAC_OS) 
	voFormatConversion* m_pFormatCovert;
    	char*               m_pTempBuffer;
#endif
public:
	void		SetLanguage(Subtitle_Language nLanguage);
	void		SetFileFormat(Subtitle_File_Format nFileFormat);
	//void		Deocde(VO_CHAR* szText, int* npLength);
	VO_CHAR*	UnicodeToUtf8(VO_CHAR* szText, int* npLength);
	void		ToUtf16(void* env, VO_CHAR* szTextIn, VO_BYTE* btTextOut, int* nOutLength);
	VO_CHAR*	UnicodeToUtf8(void* env, VO_CHAR* szText, int* npLength);

};

#endif // __CCharsetDecoder_H__
