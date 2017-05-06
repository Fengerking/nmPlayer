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
* @file CCharsetDecoder.cpp
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#define LOG_TAG "CCharsetDecoder"
#include "CCharsetDecoder.h"
#include "voLog.h"
#include "voString.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
#include <locale.h>

#ifdef _LINUX_ANDROID
#include <jni.h>
#include<stdio.h>
#include    <stdlib.h>
#include "CJavaStringWrap.h"
#else
#if (defined _IOS || defined _MAC_OS) 
#include <stdio.h>
#include    <stdlib.h>
#endif
#endif

//namespace android {
CCharsetDecoder::CCharsetDecoder(void)
: m_nLanguage(SUBTITLE_LANGUAGE_UNKNOWN)
, m_nFileFormat(SUBTITLE_FILEFORMAT_NORMAL)
#if (defined _IOS || defined _MAC_OS)
, m_pFormatCovert(NULL)
, m_pTempBuffer(NULL)
#endif
{	
//	strcpy(m_szCharset, "windows-949");
#if(defined _IOS || defined _MAC_OS)
    m_pFormatCovert = voFormatConversion::instance();
    if(m_pFormatCovert == NULL)
        VOLOGI("get format conversion failed!");
#endif
}
CCharsetDecoder::~CCharsetDecoder(void)
{
#if (defined _IOS || defined _MAC_OS) 
	m_pFormatCovert->OnDestory();
#endif
}

//Ð¡¶Ë×Ö½ÚÐò
static unsigned short get_utf16(char * str, int * index)
{
	int i = *index;
	if((str[i] & 0x80) == 0)
	{
		*index = i + 1;
		return str[i];
	}
	else if((str[i] & 0x20) == 0)
	{
		*index = i + 2;
		return (unsigned short)((str[i]  & 0x1f)<< 6) | (str[i+1] & 0x003f);
	}
	else if((str[i] & 0x10) == 0)
	{
		*index = i + 3;
		return (unsigned short)((str[i] & 0xf) << 12) | ((str[i+1] & 0x3f)<< 6) | (str[i+2] & 0x003f);
	}
	return 0;
}


static void set_utf16(char * str, int * index, unsigned short u16c)
{
	int i = *index;
	if(u16c >= 0x800)
	{
		// 3 byte
		*index = i + 3;
		str[i] = (u16c >> 12) | 0xe0;
		str[i+1] = (u16c >> 6) & 0x3f | 0x80;
		str[i+2] = u16c & 0x3f | 0x80;
	}
	else if(u16c >= 0x80)
	{
		// 2 byte
		*index = i + 2;
		str[i] = (u16c >> 6) & 0x1f | 0xc0;
		str[i+1] = u16c & 0x3f | 0x80;
	}
	else
	{
		// 1 byte
		*index = i + 1;
		str[i] = (char)((unsigned char)u16c);
	}
}


int utf8_to_utf16(char * u8str, unsigned short * u16str)
{
	int i, j;
	for(i = 0, j = 0; (u16str[j] = get_utf16(u8str, &i)) != 0; j++);
	return j;
}


int utf16_to_utf8(unsigned short * u16str, char * u8str)
{
	int i, j;
	i = 0, j = 0;
	do 
	{
		set_utf16(u8str, &i, u16str[j]);
	} while (u16str[j++] != 0);
	return i;
}

#if (defined _IOS || defined _MAC_OS) 
void	CCharsetDecoder::ToUtf16(void* env, VO_CHAR* szTextIn, VO_BYTE* btTextOut, int* nOutLength)
{
	int nLenIn = strlen(szTextIn) ;

	if (szTextIn)
	{
		unsigned short* pText2 = NULL;
		if(strcmp(m_szCharset, "utf-8") ==0)
		{
			int dwMinSize=0;
			dwMinSize = nLenIn;
			if(dwMinSize<=0)
			{
				*nOutLength = 0;
				return;
			}

			pText2 = (unsigned short*)new unsigned short[dwMinSize+2];
			if(pText2)
			{
				memset(pText2, 0 , (dwMinSize+2)*sizeof(unsigned short));
				// Convert headers from ASCII to Unicode.
				int nOut = utf8_to_utf16(szTextIn, pText2);

				if(nOut*sizeof(unsigned short)<=*nOutLength)
					*nOutLength = nOut*sizeof(unsigned short);
				memcpy(btTextOut, pText2, *nOutLength);
				delete pText2;
				pText2 = NULL;
			}
			else
			{
				*nOutLength = 0;
				return;
			}
		}
		else
		{
			unsigned char* pText2 = NULL;
			int dwMinSize=0;
            if(strcmp(m_szCharset,"gb2312") == 0)
            {
                m_pFormatCovert->FormatConversion(szTextIn,&m_pTempBuffer,nOutLength,GB_2312_80);
              //  VOLOGI("Ferry gb2312 transfer,inlength is %d,outlength is %d!",nLenIn,*nOutLength);
                memcpy(btTextOut, m_pTempBuffer, *nOutLength);
            }
            else
            {
                if(strcmp(m_szCharset, "korean") ==0)
                    strcpy(m_szCharset, "windows-949");
                char * ret = setlocale(LC_ALL, m_szCharset);
              //  VOLOGI("return char is %s",ret);
                dwMinSize = mbstowcs (NULL, szTextIn, 0);
                if(dwMinSize<=0)
                {
                    *nOutLength = 0;
                    return;
                }
                pText2 = (unsigned char*)new wchar_t[dwMinSize+2];
		   if(pText2)  
		   {
			pText2[dwMinSize*sizeof(wchar_t)] = 0;
			pText2[(dwMinSize+1)*sizeof(wchar_t)] = 0;
			// Convert headers from ASCII to Unicode.
			mbstowcs ((wchar_t*)pText2, szTextIn, dwMinSize);  

			if(dwMinSize*(sizeof(wchar_t))<=*nOutLength)
			*nOutLength = dwMinSize*(sizeof(wchar_t));

			memcpy(btTextOut, pText2, *nOutLength);
			delete pText2;
			pText2 = NULL;
                }
		   else
		   {
		   	VOLOGE("malloc error!");
		   }
            }
			//VOLOGI("---> Subtitle CCharsetDecoder::ToUtf16 can't work in some case");
		}
	}
	else
		*nOutLength = 0;
}
VO_CHAR*	CCharsetDecoder::UnicodeToUtf8(void* env, VO_CHAR* szText, int* npLength)
{
	VO_CHAR* pText2 = NULL;
	if (szText)
	{
		int dwMinSize = (* npLength)*2;

		pText2 = new VO_CHAR[dwMinSize+2];
		if(!pText2)
			return NULL;
		memset(pText2, 0 , (dwMinSize+2)*sizeof(VO_CHAR));
		int destSize = 0;
		//dwMinSize = wcstombs(pText2,(wchar_t*)szText,dwMinSize);
		dwMinSize = utf16_to_utf8((unsigned short *) szText, pText2);

		*npLength = dwMinSize;
	}
	else
		*npLength = 0;
	return (VO_CHAR*)pText2;//need be deleted outside
}
#elif (defined _WIN32)

void	CCharsetDecoder::ToUtf16(void* env, VO_CHAR* szTextIn, VO_BYTE* btTextOut, int* nOutLength)
{
	int nLenIn = strlen(szTextIn) ;

	VO_TCHAR* pText2 = NULL;
	if (szTextIn)
	{
		DWORD dwMinSize=0;
		UINT codePage  = CP_ACP;
		if(strcmp(m_szCharset, "windows-949") ==0 || strcmp(m_szCharset, "korean") ==0)
			codePage  = 949;
		if(strcmp(m_szCharset, "gb2312") ==0)
			codePage  = 936;
		if(strcmp(m_szCharset, "shift_jis") ==0)
			codePage  = 932;
		if(strcmp(m_szCharset, "utf-8") ==0)
			codePage  = CP_UTF8;
		dwMinSize = MultiByteToWideChar (codePage, 0, szTextIn, -1, NULL, 0);
		if(dwMinSize<=0)
		{
			*nOutLength = 0;
			return;
		}

		pText2 = new VO_TCHAR[dwMinSize+2];
		if(pText2)
		{
			pText2[dwMinSize] = 0;
			pText2[dwMinSize+1] = 0;
			// Convert headers from ASCII to Unicode.
			MultiByteToWideChar (codePage, 0, szTextIn, -1, (VO_TCHAR*)pText2, dwMinSize);  

			if(dwMinSize*(sizeof(VO_TCHAR))<=*nOutLength)
			*nOutLength = (dwMinSize*sizeof(VO_TCHAR));

			memcpy(btTextOut, pText2, *nOutLength);
			delete pText2;
			pText2 = NULL;
		}
		else
		{
			VOLOGE("malloc error!");
		}
	}
	else
		*nOutLength = 0;
}
VO_CHAR*	CCharsetDecoder::UnicodeToUtf8(void* env, VO_CHAR* szText, int* npLength)
{
	VO_CHAR* pText2 = NULL;
	if (szText)
	{
		DWORD dwMinSize;
		dwMinSize = WideCharToMultiByte(CP_UTF8,NULL,(VO_TCHAR*)szText, (*npLength)/sizeof(VO_TCHAR),NULL,0,NULL,FALSE);

		pText2 = new VO_CHAR[dwMinSize+2];
		if(!pText2)
			return NULL;
		pText2[dwMinSize] = 0;
		pText2[dwMinSize+1] = 0;
		int destSize = 0;
		dwMinSize = WideCharToMultiByte(CP_UTF8,NULL,(VO_TCHAR*)szText, (*npLength)/sizeof(VO_TCHAR),pText2,dwMinSize,NULL,FALSE);

		*npLength = dwMinSize;
	}
	else
		*npLength = 0;
	return (VO_CHAR*)pText2;//need be deleted outside
}
 
#else
#ifdef _LINUX_ANDROID
VO_CHAR*	CCharsetDecoder::UnicodeToUtf8(void* env, VO_CHAR* szText, int* npLength)
{
	if(env == NULL)
	{
		VOLOGI("---> Subtitle env == NULL");
		return NULL;
	}

	JNIEnv* envJni = (JNIEnv*) env;
	unsigned char* pText2 = NULL;
	if (szText)
	{
		for(int i= 0;i<* npLength;i+=2)
		{
			VO_CHAR tmp = szText[i];
			szText[i] = szText[i+1];
			szText[i+1] = tmp;
		}
		//char charset[] = "utf-8";
		CJavaStringWrap jstr(envJni);
		//	bool Convert(unsigned char * sourceBuffer, int sourceSize, char* sourceCharsetName, unsigned char*& destBuffer, int& destSize, char* destCharsetName)
		int destSize = 0;
		//VOLOGI("---> Subtitle UnicodeToUtf8 begin");
		jstr.Convert((unsigned char *) szText, *npLength, "utf-16", (unsigned char*&) pText2, destSize, "utf-8");
		*npLength = destSize;
		if(pText2)
		{
			VOLOGI("---> Subtitle UnicodeToUtf8 count:%d :Text %s", destSize, pText2);
		}
		else
		{
			VOLOGI("---> Subtitle UnicodeToUtf8 count:%d :Text null", destSize);
		}
	}
	else
	{
		*npLength = 0;
		VOLOGI("---> Subtitle UnicodeToUtf8 :Text null 2");
	}
	return (VO_CHAR*)pText2;//need be deleted outside
}


void	CCharsetDecoder::ToUtf16(void* env, VO_CHAR* szTextIn, VO_BYTE* btTextOut, int* nOutLength)
{
	if(env == NULL)
		return;
	int nLenIn = vostrlen(szTextIn) ;

	JNIEnv* envJni = (JNIEnv*) env;
	unsigned char* pText2 = NULL;
	if (szTextIn)
	{
		CJavaStringWrap jstr(envJni);
		int destSize = 0;
		//VOLOGI("---> Subtitle ToUtf16 m_szCharset:Text =%s" , m_szCharset);
		jstr.Convert((unsigned char *) szTextIn, nLenIn, m_szCharset, (unsigned char*&) pText2, destSize, "utf-16");
		if(destSize<=*nOutLength + 2)
			*nOutLength = destSize;
		if(pText2)
		{
			memcpy(btTextOut, pText2, *nOutLength);
			delete pText2;
			pText2 = NULL;
		}
	}
	else
		*nOutLength = 0;
}


#endif
#endif

void	CCharsetDecoder::SetLanguage(Subtitle_Language nLanguage)
{
	m_nLanguage = nLanguage;
	if (nLanguage == SUBTITLE_LANGUAGE_KOREAN)
	{
		strcpy(m_szCharset, "windows-949");
	}
	else if (nLanguage == SUBTITLE_LANGUAGE_SCHINESE)
	{
		strcpy(m_szCharset, "gb2312");
	}
	else if (nLanguage == SUBTITLE_LANGUAGE_JAPANESE)
	{
		strcpy(m_szCharset, "shift_jis");
	}
	else if (nLanguage == SUBTITLE_LANGUAGE_UNICODE)
	{
		strcpy(m_szCharset, "unicode");
	}
	else if (nLanguage == SUBTITLE_LANGUAGE_ENGLISH)
	{
#ifndef WINCE
		strcpy(m_szCharset, "korean");
#endif
	}
	if (m_nFileFormat == SUBTITLE_FILEFORMAT_UTF8)
	{
		strcpy(m_szCharset, "utf-8");
	}	
	
}

void	CCharsetDecoder::SetFileFormat(Subtitle_File_Format nFileFormat)
{
	m_nFileFormat = nFileFormat;
}

