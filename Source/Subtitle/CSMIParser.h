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
* @file CSMIParser.h
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#ifndef __CSMIParser_H__
#define __CSMIParser_H__


#include "CBaseSubtitleParser.h"

typedef struct LanguageTable {
	VO_CHAR	szLang[64];
	Subtitle_Language nLang;
	VO_BYTE*	     pNext;
} LANGUAGETABLE;

class	CSMIParser : CBaseSubtitleParser
{
public:
	CSMIParser(void);
	virtual ~CSMIParser(void);

protected:
	VO_BYTE*	m_pLanguageTable;


public:
	virtual	bool	Parse (void);

protected:
	bool ContainChar (char * pText);

	void AddToCharsetTable(char* pText, Subtitle_Language nLanguage);
	void FixWrongCharsetTable();
	virtual Subtitle_Language  FindCharsetInCharsetTable(char* pText);
	Subtitle_Language  GetLangID(char* pText);

};

#endif // __CSMIParser_H__
