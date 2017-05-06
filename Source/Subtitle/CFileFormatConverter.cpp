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


#define LOG_TAG "CFileFormatConverter"

#include "voLog.h"
#include "CFileFormatConverter.h"
#include "CCharsetDecoder.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif



CFileFormatConverter::CFileFormatConverter(void)
{	

}

CFileFormatConverter::~CFileFormatConverter(void)
{

}

bool CFileFormatConverter::Convert(void * env, VO_BYTE* o_szText, int *pnSize )
{
	if(*pnSize<3)
		return false;

	if (o_szText[0] == 0xef &&  o_szText[1] == 0xbb &&  o_szText[2] == 0xbf)
		return true;

	bool bLittleEnd = true;
	if (o_szText[0] == 0xFe &&  o_szText[1] == 0xFf)
		bLittleEnd = false;
	else if (o_szText[0] != 0xFF ||  o_szText[1] != 0xFe)
	{		
		VOLOGI("---> Subtitle Normal file");
		return false;
	}
	if(!bLittleEnd)
	{
		for(int i = 0;i<*pnSize-1;i+=2)
		{
			VO_BYTE bt = o_szText[i];
			o_szText[i] = o_szText[i+1];
			o_szText[i+1] = bt;
		}
	}

	VO_BYTE* pb = o_szText+2;
	CCharsetDecoder dec;
	int nOut = *pnSize-2;
	VOLOGI("---> Subtitle Convert begin");
	VO_CHAR* ch = (VO_CHAR*)dec.UnicodeToUtf8(env, (VO_CHAR*)pb, &nOut);
	if(ch!=NULL)
	{
		//ch[nOut] = 0;
		int n = nOut<(*pnSize)?nOut:(*pnSize);
		o_szText[n] = 0;
		memcpy((VO_CHAR*)o_szText,ch,n);
		delete ch;
		ch = NULL;
	}
	return true;		

//#endif
}
