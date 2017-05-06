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
* @file CBaseSubtitleParser.h
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#ifndef __CBaseSubtitleParser_H__
#define __CBaseSubtitleParser_H__

#define   MAX_PARSER_TRACK_COUNT	6

#include "CBaseSubtitleTrack.h"

#include "voType.h"
#include "voString.h"
#include <stdio.h>
#include <stdlib.h>
#include "CPtrList.h"

#include "voSubtitleParser.h"
#ifdef _WIN32
#define VOUNICODE	_T
#ifdef _WIN32_WCE
#define subtitle_stricmp _stricmp
#else
#define subtitle_stricmp stricmp
#endif
#else
#define VOUNICODE	 
#define subtitle_stricmp strcasecmp
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define SCALE_FOR_FONT_EM_SIZE 100000
#define SCALE_FOR_FONT_PIXEL_SIZE 10000
#define SCALE_FOR_FONT_SIZE 1000  ///<pls refer the VO_U16	FontSize;///<0:STANDARD,1:SMALL, 2:LARGE ,1000:for support the percent,add the 1000 as the base,ex,1200 indicate the percent should be 1200-100=200%
typedef struct
{
	VO_CHAR strID[1024];
	VO_S32 xPos;
	VO_S32 yPos;
	VO_S32 width;
	VO_S32 height;
}ttRegion;


class TT_Style{

public:
	int fontSize;

	//fontWeight=0, normal;fontWeight=1, bold;
	int fontWeight;

	voSubtitleRGBAColor color ;
	voSubtitleRGBAColor backgroundColor ;
	voSubtitleRGBAColor windowRegionColor ;
	int textAlign;
	char id[64];
	char StyleId[64];
	int nFontFamily;
	char fontStyle[16];
	char textDecoration[16];
	bool	wrapEnable;
//	VO_U32  uRegionCnt;
	ttRegion region;
	VO_S32 ColumnCnt;
	VO_S32 RowCnt;
	VO_BOOL bBackGroundColorForce;
	
	TT_Style()
	{
		fontSize = -1;
		fontWeight = 0;
		nFontFamily = 0;
		color.nBlue=0xff;color.nGreen=0xff;color.nRed=0xff;color.nTransparency=0xff;
		backgroundColor.nBlue=0x00;backgroundColor.nGreen=0x00;backgroundColor.nRed=0x00;backgroundColor.nTransparency=0xff;
		windowRegionColor.nBlue=0xff;windowRegionColor.nGreen=0xff;windowRegionColor.nRed=0xff;windowRegionColor.nTransparency=0x0;
		textAlign = 0;
		wrapEnable = false;
		memset(id,0,64);
		memset(StyleId,0,64);
		memset(fontStyle,0,16);
		memset(textDecoration,0,16);
		memset(&region,0,sizeof(ttRegion));
		region.xPos = -1;
		region.yPos = -1;
		region.width = -1;
		region.height = -1;
		ColumnCnt = -1;
		RowCnt = -1;
		bBackGroundColorForce = VO_FALSE;
	}
	VO_VOID StyleReplace(TT_Style& ori)
	{
		if (ori.fontSize != -1)
		{
			fontSize = ori.fontSize;
		}
		if (ori.fontWeight != 0)
		{
			fontWeight = ori.fontWeight;
		}
		if (ori.color.nBlue != 0xff || ori.color.nGreen !=0xff || ori.color.nRed != 0xff )
		{
			color = ori.color;
		}
		if (ori.color.nTransparency != 0xff)
		{
			color.nTransparency = ori.color.nTransparency;
		}

		if (ori.bBackGroundColorForce || (ori.backgroundColor.nBlue != 0x00 || ori.backgroundColor.nGreen !=0x00 || ori.backgroundColor.nRed != 0x00) )
		{
			backgroundColor = ori.backgroundColor;
		}
		if (ori.backgroundColor.nTransparency != 0xff)
		{
			backgroundColor.nTransparency = ori.backgroundColor.nTransparency;
		}

		if (ori.windowRegionColor.nBlue != 0xff || ori.windowRegionColor.nGreen !=0xff || ori.windowRegionColor.nRed != 0xff )
		{
			windowRegionColor = ori.windowRegionColor;
		}
		if (ori.windowRegionColor.nTransparency != 0x0)
		{
			windowRegionColor.nTransparency = ori.windowRegionColor.nTransparency;
		}

		if (ori.textAlign != 0)
		{
			textAlign = ori.textAlign;
		}
		if (ori.nFontFamily != 0)
		{
			nFontFamily = ori.nFontFamily;
		}
		if (strlen(ori.fontStyle) > 0)
		{
			strcpy(fontStyle,ori.fontStyle);
		}
		if (strlen(ori.textDecoration) > 0)
		{
			strcpy(textDecoration,ori.textDecoration);
		}
		if (ori.wrapEnable != false)
		{
			wrapEnable = ori.wrapEnable;
		}
		if (strlen(ori.region.strID) > 0)
		{
			strcpy(region.strID,ori.region.strID);
		}
		if (ori.region.xPos != -1)
		{
			region.xPos = ori.region.xPos;
		}
		if (ori.region.yPos != -1)
		{
			region.yPos = ori.region.yPos;
		}
		if (ori.region.width != -1)
		{
			region.width = ori.region.width;
		}
		if (ori.region.height != -1)
		{
			region.height = ori.region.height;
		}
		if (ori.ColumnCnt != -1)
		{
			ColumnCnt = ori.ColumnCnt;
		}
		if (ori.RowCnt != -1)
		{
			RowCnt = ori.RowCnt;
		}
		
	}
	///<return value should be ///<0:STANDARD,1:SMALL, 2:LARGE
	int fontSizetoInt(char* str)
	{
		VO_CHAR* strTmp = new VO_CHAR[strlen(str)+1];
		strcpy(strTmp,str);
		VO_CHAR* pTmp = NULL;
		int size = 0;
		if(pTmp = strstr(strTmp,"px"))
		{
			*pTmp = 0;
			size = SCALE_FOR_FONT_PIXEL_SIZE - SCALE_FOR_FONT_SIZE + atoi(strTmp);
		}
		else if(pTmp = strstr(strTmp,"em"))
		{
			*pTmp = 0;
			size = atoi(strTmp)*100+SCALE_FOR_FONT_EM_SIZE;
		}
		else if(pTmp = strstr(strTmp,"c"))
		{
			*pTmp = 0;
			size = atoi(strTmp)*100;
		}
		else if(pTmp = strstr(strTmp,"%"))
		{
			*pTmp = 0;
			size = atoi(strTmp);
		}
		else
		{
			size = SCALE_FOR_FONT_PIXEL_SIZE - SCALE_FOR_FONT_SIZE + atoi(strTmp);
		}
		delete strTmp;
		return size;
	}
	int fontFamilyToInt(char* str)
	{


//<genericFamilyName>
//  : "default"
//  | "monospace"
//  | "sansSerif"
//  | "serif"
//  | "monospaceSansSerif"
//  | "monospaceSerif"
//  | "proportionalSansSerif"
//  | "proportionalSerif"
 
		int n = 0;
		if(subtitle_stricmp(str,"") == 0)
			return 0;
		
		if(subtitle_stricmp(str,"monospace") == 0)
			n = FontStyle_Monospaced;
		else if (subtitle_stricmp(str,"Courier") == 0)
			n = FontStyle_Courier;
		else if(subtitle_stricmp(str,"Times New Roman") == 0)
			n = FontStyle_Times_New_Roman;
		else if(subtitle_stricmp(str,"sansSerif") == 0)
			n = FontStyle_SansSerif;
		else if(subtitle_stricmp(str,"Helvetica") == 0)
			n = FontStyle_Helvetica;
		else if(subtitle_stricmp(str,"serif") == 0)
			n = FontStyle_Serif;
		else if(subtitle_stricmp(str,"Arial") == 0)
			n = FontStyle_Arial;
		else if(subtitle_stricmp(str,"monospaceSansSerif") == 0)
			n = FontStyle_MonospaceSansSerif;
		else if(subtitle_stricmp(str,"Dom") == 0)
			n = FontStyle_Dom;
		else if(subtitle_stricmp(str,"monospaceSerif") == 0)
			n = FontStyle_Monospaced_with_serifs;
		else if(subtitle_stricmp(str,"Coronet") == 0)
			n = FontStyle_Coronet;
		else if(subtitle_stricmp(str,"proportionalSansSerif") == 0)
			n = FontStyle_ProportionalSansSerif;
		else if(subtitle_stricmp(str,"Gothic") == 0)
			n = FontStyle_Gothic;
		else if(subtitle_stricmp(str,"proportionalSerif") == 0)
			n = FontStyle_ProportionalSerif;
		return n;
	}
	
	int textAlignToInt(char* str)
	{//0:Left; 1:Right; 2:Center
		int n = 0;
		if(subtitle_stricmp(str,"right") == 0)
			n = 1;
		if(subtitle_stricmp(str,"center") == 0)
			n = 2;
		textAlign = n;
		return n;
	}
	int fontWeightToInt(char* str)
	{//0:Left; 1:Right; 2:Center
		int n = 0;
		if(subtitle_stricmp(str,"normal") == 0)
			n = 0;
		else
			n = 1;
		fontWeight = n;
		return n;
	}
	int textWrapToBool(char* str)
	{
		if(subtitle_stricmp(str,"wrap") == 0)
			return true;
		else
			return false;
	}
	float opacityToFloat(char* str)
	{
		return atof(str);
	}
	voSubtitleRGBAColor colorStringToInt(char* str)
	{
		voSubtitleRGBAColor clr;
		memset(&clr,0,sizeof(voSubtitleRGBAColor));
		if(str == NULL || str[0] == '#')
		{
			clr.nBlue=0xff;clr.nGreen=0xff;clr.nRed=0xff;clr.nTransparency=0x60;
			if(str != NULL)
			{
				int r,g,b,a;
				if(strlen(str) == 7)
				{
					::sscanf (str, "#%2x%2x%2x", &r, &g, &b);	
					clr.nBlue=b;clr.nGreen=g;clr.nRed=r;clr.nTransparency=0xff;
				}
				if(strlen(str) == 9)
				{
					::sscanf (str, "#%2x%2x%2x%2x", &r, &g, &b,&a);	
					clr.nBlue=b;clr.nGreen=g;clr.nRed=r;clr.nTransparency=a;
				}
			}
			return clr;
		}
		else if(strstr(str,"rgb"))
		{
			clr.nBlue=0xff;clr.nGreen=0xff;clr.nRed=0xff;clr.nTransparency=0x60;
			if(strstr(str,"rgba"))
			{
				char* tmp;
				tmp = strstr(str,"(");
				if(tmp)
				{
					tmp++;
					clr.nRed = atoi(tmp);
					tmp = strstr(tmp,",");
					if(tmp)
					{
						tmp++;
						clr.nGreen = atoi(tmp);
						tmp = strstr(tmp,",");
						if(tmp)
						{
							tmp++;
							clr.nBlue = atoi(tmp);
							tmp = strstr(tmp,",");
							if(tmp)
							{
								tmp++;
								clr.nTransparency = atoi(tmp);
							}
						}
					}
				}
			}
			else
			{
				char* tmp;
				tmp = strstr(str,"(");
				if(tmp)
				{
					tmp++;
					clr.nRed = atoi(tmp);
					tmp = strstr(tmp,",");
					if(tmp)
					{
						tmp++;
						clr.nGreen = atoi(tmp);
						tmp = strstr(tmp,",");
						if(tmp)
						{
							tmp++;
							clr.nBlue = atoi(tmp);
						}
					}
				}
				clr.nTransparency = 0xff;
			}
			return clr;
		}
		while(true)
		{
			if (subtitle_stricmp(str,"transparent") == 0)
			{
				clr.nBlue=0xff;clr.nGreen=0xff;clr.nRed=0xff;clr.nTransparency=0x0;
				break;
			}
			if(subtitle_stricmp(str,"white") == 0)
			{
				clr.nBlue=0xff;clr.nGreen=0xff;clr.nRed=0xff;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"black") == 0)
			{
				clr.nBlue=0x00;clr.nGreen=0x00;clr.nRed=0x00;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"blue") == 0)
			{
				clr.nBlue=0xff;clr.nGreen=0;clr.nRed=0;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"green") == 0)
			{
				clr.nBlue=0;clr.nGreen=0xff;clr.nRed=0;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"red") == 0)
			{
				clr.nBlue=0;clr.nGreen=0;clr.nRed=0xff;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"yellow") == 0)
			{
				clr.nBlue=0;clr.nGreen=0xff;clr.nRed=0xff;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"purple") == 0)
			{
				clr.nBlue=0x80;clr.nGreen=0;clr.nRed=0x80;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"fuchsia") == 0 || subtitle_stricmp(str,"magenta") == 0)
			{
				clr.nBlue=0xff;clr.nGreen=0;clr.nRed=0xff;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"cyan") == 0 || subtitle_stricmp(str,"aqua") == 0  )
			{
				clr.nBlue=0xff;clr.nGreen=0xff;clr.nRed=0;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"silver") == 0)
			{
				clr.nBlue=0xc0;clr.nGreen=0xc0;clr.nRed=0xc0;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"gray") == 0)
			{
				clr.nBlue=0x80;clr.nGreen=0x80;clr.nRed=0x80;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"maroon") == 0)
			{
				clr.nBlue=0x0;clr.nGreen=0x0;clr.nRed=0x80;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"lime") == 0)
			{
				clr.nBlue=0x0;clr.nGreen=0xff;clr.nRed=0x0;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"olive") == 0)
			{
				clr.nBlue=0x0;clr.nGreen=0x80;clr.nRed=0x80;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"navy") == 0)
			{
				clr.nBlue=0x80;clr.nGreen=0x0;clr.nRed=0x0;clr.nTransparency=0xff;
				break;
			}
			if(subtitle_stricmp(str,"teal") == 0)
			{
				clr.nBlue=0x80;clr.nGreen=0x80;clr.nRed=0x0;clr.nTransparency=0xff;
				break;
			}
			break;
		}
/*  : "transparent"                           // #00000000
  | "black"                                 // #000000ff
  | "silver"                                // #c0c0c0ff
  | "gray"                                  // #808080ff
  | "white"                                 // #ffffffff
  | "maroon"                                // #800000ff
  | "red"                                   // #ff0000ff
  | "purple"                                // #800080ff
  | "fuchsia"                               // #ff00ffff
  | "magenta"                               // #ff00ffff (= fuchsia)
  | "green"                                 // #008000ff
  | "lime"                                  // #00ff00ff
  | "olive"                                 // #808000ff
  | "yellow"                                // #ffff00ff
  | "navy"                                  // #000080ff
  | "blue"                                  // #0000ffff
  | "teal"                                  // #008080ff
  | "aqua"                                  // #00ffffff
  | "cyan"                                  // #00ffffff (= aqua)
 */
		
		return clr;
	}
	static TT_Style* GetTTStyle(CObjectList<TT_Style>  *styleList, char* styleName,bool bExact = false)
	{
		TT_Style* tt = NULL;//m_styleList.GetHead();
		bool bFind = false;
		for (POSITION cursor = styleList->GetHeadPosition()           
			; cursor!=NULL                               
			; cursor = styleList->Next(cursor)                
			)
		{
			tt = styleList->Get(cursor);
			if(tt)
			{
				if (bExact == true)
				{
					if (0 == strcmp(tt->id,styleName))
					{
						bFind = true;
						break;
					}
				}
				else
				{
					bFind = true;
					break;
				}
			}
		}
		if(!bFind)
			tt = NULL;
		return tt;
	}
	static void DeleteStyles(CObjectList<TT_Style>  *styleList)
	{
		TT_Style* tt = NULL;//m_styleList.GetHead();
		bool bFind = false;
		for (POSITION cursor = styleList->GetHeadPosition()           
			; cursor!=NULL                               
			; cursor = styleList->Next(cursor)                
			)
		{
			tt = styleList->Get(cursor);
			delete tt;
		}
		styleList->RemoveAll();
	}

	//<style id="defaultSpeaker" tts:fontSize="24" tts:fontFamily="Arial" 
	//tts:fontWeight="normal" tts:fontStyle="normal" tts:textDecoration="none" tts:color="white" tts:backgroundColor="black" tts:textAlign="center" />
};

class	CXmlParserWraper
{
public:
	static bool GetAttribute(char* xmlBuffer, char* attributeName, char* outAttributeVal, int nAttributeVal)
	{
		bool b = false;
		bool bSingleQuotes = false;
		char attrName[128];
		strcpy(attrName, attributeName);
		strcat(attrName, "=\"");

		char attrNamebak[128];
		strcpy(attrNamebak, attributeName);
		strcat(attrNamebak, "=\'");

		memset(outAttributeVal,0,nAttributeVal);

		char* pEnd = strstr(xmlBuffer,">");
		if(pEnd == NULL)
			return b;
		char* pAttr = strstr(xmlBuffer,attrName);
		if (NULL == pAttr)
		{
			pAttr = strstr(xmlBuffer,attrNamebak);
			bSingleQuotes = true;
		}
		
		if(pAttr!=NULL && pAttr<pEnd)//to get body style
		{
			int nLen = strlen(attrName);
			pAttr+=nLen;
			char* pAttrEnd = strstr(pAttr,"\"");
			if (NULL == pAttrEnd || true == bSingleQuotes)
			{
				pAttrEnd = strstr(pAttr,"\'");
			}
			if(pAttrEnd)
			{
				if(pAttrEnd<pEnd && pAttrEnd - pAttr< nAttributeVal )
				{
					memcpy(outAttributeVal,pAttr,pAttrEnd - pAttr);
					outAttributeVal[pAttrEnd - pAttr] = 0;
					b = true;
				}
			}
		}

		return b;
	};
	static bool GetSubNode(char* xmlBuffer, char* subNodeName, char*& subNodeBegin, char*& subNodeEnd)
	{
		bool b = false;
		char subName[128];
		strcpy(subName, "<");
		strcat(subName, subNodeName);

		subNodeBegin = strstr(xmlBuffer,subName);
		if(subNodeBegin!=NULL)//to get body style
		{
			int nLen = strlen(subNodeName);
			char* pEnd = strstr(subNodeBegin,">");
			if(pEnd == NULL)
			{
				subNodeEnd = NULL;
				return b;
			}
			if(*(pEnd-1) == '/')
			{
				pEnd++;
				subNodeEnd = pEnd;
				return true;
			}
			strcpy(subName, "</");
			strcat(subName, subNodeName);
			strcat(subName, ">");
			pEnd = strstr(pEnd,subName);
			if(pEnd == NULL)
			{
				subNodeEnd = NULL;
				return b;
			}
			{
				pEnd+=strlen(subName);
				subNodeEnd = pEnd;
				return true;
			}
		}

		return b;
	};
	static void RemoveSubNode(char* xmlBuffer, char* subNodeBegin, char* subNodeEnd)
	{
		if(subNodeBegin == NULL || subNodeEnd == NULL)
			return ;

		int nLen = strlen(xmlBuffer);
		memmove(subNodeBegin,subNodeEnd,nLen-(subNodeEnd-xmlBuffer));
		xmlBuffer[nLen-(subNodeEnd-subNodeBegin)] = 0;

	};
	static void RemoveNodeHead(char* xmlBuffer)
	{
		if(xmlBuffer == NULL)
			return ;

		int nLen = strlen(xmlBuffer);
		char* pEnd = strstr(xmlBuffer,">");
		if(pEnd == NULL)
			return ;
		pEnd++;
		memmove(xmlBuffer,pEnd,nLen-(pEnd-xmlBuffer));
		xmlBuffer[nLen-(pEnd-xmlBuffer)] = 0;

	};
	static void RemoveNodeTail(char* xmlBuffer)
	{
		if(xmlBuffer == NULL)
			return ;

		int nLen = strlen(xmlBuffer);
		int ntimes = 0;
		bool bFind =false;// find / char
		while(nLen>0 && ntimes<20)
		{
			if(xmlBuffer[nLen-1] == '/')
				bFind = true;
			else{
				if(bFind)
				{
					if(xmlBuffer[nLen-1] == ' '||xmlBuffer[nLen-1] == '<'
						||xmlBuffer[nLen-1] == '\t' || xmlBuffer[nLen-1] == '\n')
					{
						xmlBuffer[nLen-1]=0;
						break;
					}
					else
					{
						xmlBuffer[nLen]=0;
						break;
					}

				}
			}
			ntimes++;
			nLen--;
		}

	};
	static char* CheckNodeHead(char* xmlBuffer, char* nodeHead)
	{
		if(xmlBuffer == NULL)
			return NULL;

		int nLen = strlen(nodeHead);
		char* pHead = strstr(xmlBuffer, nodeHead);
		if(pHead==NULL)
			return NULL;
		if(pHead[nLen] == ' '
			|| pHead[nLen] == '>'
			|| pHead[nLen] == '\t'
			|| pHead[nLen] == '\n'
			|| pHead[nLen] == '\r')
			return pHead;
		return NULL;

	};
};


class	CBaseSubtitleParser
{
public:
	CBaseSubtitleParser(void);
	virtual ~CBaseSubtitleParser(void);
	void SetJniEnv(void* env){m_jniEnv = env;};

	static voSubtitleCodingType GetParserType(VO_TCHAR * pFilePath);
	virtual VO_VOID SetBaseTime(VO_S64 llBaseTime){}
protected:
	int				m_nTrackCount;
	unsigned int	m_TrackList[64];

	int				m_nCurrTrackIndex;
	CBaseSubtitleTrack* m_pCurrTrack;

	int			m_nFileSize;
	VO_BYTE*	m_pFileData;
	VO_BYTE*	m_pCurPos;

	void*					m_jniEnv;

	CObjectList<TT_Style>	*m_styleList;
	char					m_strBodyStyle[64]; //for task 23934,avoid overflow
	char					m_strDivStyle[64]; 
	Subtitle_File_Format	m_nFileFormat;

	bool					m_bParseFragmentStreaming;			



public:
	virtual bool	SetSubtitleFile(VO_TCHAR * pFilePath);
	virtual bool	SetSubtitleData(VO_CHAR * pData, int len);
	virtual	bool	Parse (void) = 0;
	
	virtual int		GetTrackCount() {return m_nTrackCount;}
	virtual CBaseSubtitleTrack*		GetTrack(int nTrackIndex = 0);

	virtual	CBaseSubtitleTrack*		CreateSubtitleTrack();
	virtual	CBaseSubtitleTrack*		CreateTextTrack();

	virtual bool	SetTag(VO_CHAR* pText);
	virtual void	SetStyleList(CObjectList<TT_Style>  * pList){ m_styleList = pList;};
	char*			GetBodyStyle(){return m_strBodyStyle;};
	char*			GetDivStyle(){return m_strDivStyle;};
	Subtitle_File_Format GetFileFormat(){return m_nFileFormat;};

	void			SetParseFragmentStreaming(bool parseFragmentStreaming){m_bParseFragmentStreaming = parseFragmentStreaming;};
	bool			GetParseFragmentStreaming(){return m_bParseFragmentStreaming;};
	

protected:
	virtual bool	ReadNextLine (char ** ppNextLine, int & nSize);

	static bool		CheckStr(VO_TCHAR* szText1, VO_CHAR* szText2);
		   bool		FilterText (VO_CHAR* pText);

	virtual	void	SwitchTrack(int nTrackIndex);
	virtual Subtitle_Language  FindCharsetInCharsetTable(char* pText) {return SUBTITLE_LANGUAGE_UNKNOWN;}

	virtual Subtitle_Language GetLangFromTag(VO_CHAR* pText);
//	virtual bool	SetLanguage(VO_CHAR* pText);

};

#endif // __CBaseSubtitleParser_H__
