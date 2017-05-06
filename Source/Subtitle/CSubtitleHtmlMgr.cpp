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
* @file CSubtitleHtmlMgr.cpp
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#define LOG_TAG "CSubtitleHtmlMgr"
#include "CSubtitleHtmlMgr.h"
#include "CTimedTextMLParser.h"
#include "CSMPTETTMLParser.h"
#include "voLog.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CSubtitleHtmlMgr::CSubtitleHtmlMgr(void)
: m_nCurrStartTime(0)
, m_nCurrEndTime(0)
, m_bEnd(false)
//, m_dataBuffer(NULL)
//, m_dataBufferMaxSize(0)
, m_nStartTimeLastTime(-1)
, m_nEndTimeLastTime(-1)
, m_nCount(0)
{	
	memset(m_szBreak, '\n', 1);
	memset(m_szBreak+1, 0, 3);
	//m_dataBufferMaxSize  = 2048;
	//m_dataBuffer = new VO_BYTE[m_dataBufferMaxSize];

	m_subtitleInfo.nTimeStamp = 0;
	memset(&m_subtitleInfoEntry,0,sizeof(voSubtitleInfoEntry));
	m_subtitleInfo.pSubtitleEntry = &m_subtitleInfoEntry;
	m_subtitleInfoEntry.pNext = NULL;
	m_subtitleInfoEntry.nDuration = -1;

	m_subNULL.nTimeStamp = 0;
	memset(&m_subNULLEntry,0,sizeof(voSubtitleInfoEntry));
	m_subNULL.pSubtitleEntry = &m_subNULLEntry;
	m_subNULLEntry.pNext = NULL;
	m_subNULLEntry.nDuration = -1;

}


CSubtitleHtmlMgr::~CSubtitleHtmlMgr(void)
{
	//if(m_dataBuffer)
	//	delete [] m_dataBuffer;
	CleareSubtitleInfo();
	
}

void BubbleSort(char**a,int n)
{
	int i,j;
	char* tmp;
	for(i = 0;i<n-1;i++)
	{
		for(j=i+1;j<n;j++)
		{
			if(a[i]>a[j])
			{
				tmp=a[i];
				a[i]=a[j];
				a[j]=tmp;
			}
		}
	}
}
voSubtitleTextRowInfo* CSubtitleHtmlMgr::AddNewRow()
{

	voSubtitleTextRowInfo* cur = m_subtitleInfoEntry.stSubtitleDispInfo.pTextRowInfo;
	while (cur)
	{
		if(cur->pNext == NULL)
			break;
		cur = cur->pNext;
	}

	voSubtitleTextRowInfo* row = new voSubtitleTextRowInfo();
	if(!row)
		return NULL;
	row->pNext =	NULL;
	memset(row,0,sizeof(voSubtitleTextRowInfo));
	row->pTextInfoEntry =	NULL;
	if(cur==NULL)
		m_subtitleInfoEntry.stSubtitleDispInfo.pTextRowInfo = row;
	else
		cur->pNext = row;
	return row;

}
voSubtitleTextInfoEntry* CSubtitleHtmlMgr::AddNewTextInfoEntry(voSubtitleTextRowInfo* row)
{
	if(row == NULL )
		return NULL;
	voSubtitleTextInfoEntry* cur = row->pTextInfoEntry;
	while(cur)
	{
		if(cur->pNext==NULL)
		{
			break;
		}
		cur  = cur->pNext;
	}

	voSubtitleTextInfoEntry* textInfoEnt = new voSubtitleTextInfoEntry();
	if(!textInfoEnt)
		return NULL;
	memset(textInfoEnt,0,sizeof(voSubtitleTextInfoEntry));
	textInfoEnt->pString = NULL;
	//memset(textInfoEnt->pString, 0,128);
	textInfoEnt->nSize = 0;
	if(cur == NULL )
		row->pTextInfoEntry = textInfoEnt;
	else
		cur->pNext = textInfoEnt;
	textInfoEnt->pNext = NULL;

	return textInfoEnt;
}

void CSubtitleHtmlMgr::CleareSubtitleInfo()
{
	while (m_subtitleInfoEntry.stSubtitleDispInfo.pTextRowInfo!=NULL)
	{
		voSubtitleTextRowInfo* pCur =  m_subtitleInfoEntry.stSubtitleDispInfo.pTextRowInfo;//= m_rowinfo.pNext;
		m_subtitleInfoEntry.stSubtitleDispInfo.pTextRowInfo = pCur->pNext;
		while (pCur->pTextInfoEntry!=NULL)
		{
			voSubtitleTextInfoEntry* old2 = pCur->pTextInfoEntry;
			pCur->pTextInfoEntry = pCur->pTextInfoEntry->pNext;
			if(old2->pString!=NULL)
			{
				delete [] old2->pString;
				old2->pString = NULL;
			}
			delete old2;
			old2 = NULL;
		}
		
		delete pCur;
		pCur = NULL;
	}
}
TT_Style* CSubtitleHtmlMgr::GetTTStyle(char* styleName,bool bExact)
{
	return TT_Style::GetTTStyle(&m_styleList, styleName ,bExact);
}

bool CSubtitleHtmlMgr::IsInRange(int nPos, int nStartTime, int nEndTime)
{
	if (nPos >= nStartTime && nPos <= nEndTime)
		return true;
	else //if (nPos == nStartTime) return true;
	  return false;
}

bool CSubtitleHtmlMgr::IsChanged(int nPos)
{
	return !IsInRange(nPos, m_nCurrStartTime, m_nCurrEndTime);
}
voSubtitleInfo* CSubtitleHtmlMgr::GetCurrentWebVTTText(CBaseSubtitleTrack* pTrack, CTextItem * pTextItem)
{
	char 	ch2[1024];
	char		text[1024];
	char* 	addrList[32];
	char*	tmpPointer[2];
	int		listnode 		= 0;
	int		ch2Length 	= 0;
	int		nLine		= 0;
	int 		copyLength	= 0;
	int 		tagVertical 	= -1;
	int 		tagLine		= -1;
	int		tagPosition	= -1;
	int		tagSize 		= -1;
	int		tagAlign		= -1;
	int		rowNum 	= 0;
	
	bool		isItalic		= false;
	bool		isBold		= false;
	bool		isUnderLine	= false;
	bool		isNewRow	= false;
	for(int i = 0;i < TEXT_ITEM_COUNT;i++)
	{
		listnode = 0;
		if(pTextItem->m_aText[i] == NULL)
		{
			break;
		}
		memset(ch2,0,sizeof(char)* 1024);
		memset(addrList,0,sizeof(char*) * 32);
		if(strlen(pTextItem->m_aText[i]) < 1024)
		{
			strcpy(ch2,pTextItem->m_aText[i]);
			ch2Length = strlen(ch2);
			addrList[listnode++] = ch2;
		}
		else
		{
			VOLOGE("Wrong:Text length is more than 1k");
			return NULL;
		}
		tagVertical 	= pTextItem->m_nVertical;
		tagLine		= pTextItem->m_nLine;
		tagPosition	= pTextItem->m_nPosition;
		tagSize 		= pTextItem->m_nSize;
		tagAlign		= pTextItem->m_nAlign;
		rowNum 		= 0;
		
		tmpPointer[0] = strstr(ch2,"<");
		while(tmpPointer[0])
		{
			addrList[listnode++] = tmpPointer[0];
			if(listnode > 32)
			{
				VOLOGE("listnode overflow,parse error");
				return NULL;
			}
			tmpPointer[1]= tmpPointer[0] + 1;
			tmpPointer[0] = NULL;
			tmpPointer[0] = strstr(tmpPointer[1],"<");
		}
		tmpPointer[0] = strstr(ch2,"\n");
		while(tmpPointer[0])
		{
			rowNum++;
			addrList[listnode++] = tmpPointer[0];
			if(listnode > 32)
			{
				VOLOGE("listnode overflow,parse error");
				return NULL;
			}
			tmpPointer[1]= tmpPointer[0] + 1;
			tmpPointer[0] = NULL;
			tmpPointer[0] = strstr(tmpPointer[1],"\n");
		}
		BubbleSort(addrList, 32);
		listnode = 0;

		voSubtitleTextRowInfo* pRow = NULL;
		int nRow = 0;
		if(tagLine >= 0)
		{
			nRow = (16 - rowNum) * tagLine /100;
		}
		else
		{
			if(rowNum > 2)
				nRow = 16 - rowNum;
			else
				nRow = 14;
		}
		int nCol = 0;
	/*	if(tagPosition >= 0)
			nCol = tagPosition;
*/
		int rectTop = 0;
		int rectLeft = 0;
		rectTop = (nRow + nLine)*6;
		rectLeft = nCol;
		pRow = AddNewRow();
		if(!pRow)
			return NULL;
		pRow->pNext =	NULL;
		pRow->stTextRowDes.stDataBox.stRect.nTop = rectTop ;
		pRow->stTextRowDes.stDataBox.stRect.nLeft = rectLeft;
		pRow->stTextRowDes.stDataBox.stRect.nBottom = rectTop + 6 ;
		pRow->stTextRowDes.stDataBox.stRect.nRight = 100 - rectLeft;
		pRow->stTextRowDes.stDataBox.stRectBorderColor.nRed = 255;
		pRow->stTextRowDes.stDataBox.stRectBorderColor.nGreen = 255;
		pRow->stTextRowDes.stDataBox.stRectBorderColor.nBlue = 255;
		pRow->stTextRowDes.stDataBox.stRectBorderColor.nTransparency = 255;
		pRow->stTextRowDes.stDataBox.stRectFillColor.nRed = 0;
		pRow->stTextRowDes.stDataBox.stRectFillColor.nGreen = 0;
		pRow->stTextRowDes.stDataBox.stRectFillColor.nBlue = 0;
		pRow->stTextRowDes.stDataBox.stRectFillColor.nTransparency = 255;
		pRow->stTextRowDes.stDataBox.nRectZOrder=0 ;
		pRow->stTextRowDes.stDataBox.nRectBorderType=0 ;	
		pRow->stTextRowDes.nHorizontalJustification = 2;
		if(tagAlign >= 0)
		{
			if(tagAlign == 0 || tagAlign == 3 )
			{
				pRow->stTextRowDes.nHorizontalJustification = 0;
			}
			else if(tagAlign == 1 || tagAlign == 4)
			{	
				pRow->stTextRowDes.nHorizontalJustification = 1;
			}
			else if(tagAlign == 2)
			{
				pRow->stTextRowDes.nHorizontalJustification = 2;
			}
		}
	//	VOLOGI("Ferry align is %d",pRow->stTextRowDes.nHorizontalJustification);
		pRow->stTextRowDes.nPrintDirection=0 ;
		pRow->pTextInfoEntry =	NULL;//&m_textInfo;
		m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nTop = pRow->stTextRowDes.stDataBox.stRect.nTop;
		m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nLeft = pRow->stTextRowDes.stDataBox.stRect.nLeft;
		m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nBottom = pRow->stTextRowDes.stDataBox.stRect.nBottom;
		m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nRight = pRow->stTextRowDes.stDataBox.stRect.nRight;
		nLine++;
		while(true)
		{
			while(addrList[listnode] == 0)
			{
				listnode++;
			}
			tmpPointer[0] = addrList[listnode];
			if(tmpPointer[0][0] == '\n')
			{
				if(strlen(tmpPointer[0]) == 1)
					break;
				else
					isNewRow = true;
			}
			else if(tmpPointer[0][0] == '<')
			{
				
				if(tmpPointer[0][1] == '/')
				{
					if(tmpPointer[0][2] == 'i')
						isItalic = false;
					else if(tmpPointer[0][2] == 'b')
						isBold = false;
					else if(tmpPointer[0][2] == 'u')
						isUnderLine = false;
				}
				else
				{
					if(tmpPointer[0][1] == 'i')
						isItalic = true;
					else if(tmpPointer[0][1] == 'b')
						isBold = true;
					else if(tmpPointer[0][1] == 'u')
						isUnderLine = true;
				}
			}
			if(isNewRow)
			{	
				rectTop = (nRow + nLine)*6;
				rectLeft = nCol;
				pRow = AddNewRow();
				if(!pRow)
					return NULL;
				pRow->pNext =	NULL;
				pRow->stTextRowDes.stDataBox.stRect.nTop = rectTop ;
				pRow->stTextRowDes.stDataBox.stRect.nLeft = rectLeft;
				pRow->stTextRowDes.stDataBox.stRect.nBottom = rectTop + 6 ;
				pRow->stTextRowDes.stDataBox.stRect.nRight = 100 - rectLeft;
				pRow->stTextRowDes.stDataBox.stRectBorderColor.nRed = 255;
				pRow->stTextRowDes.stDataBox.stRectBorderColor.nGreen = 255;
				pRow->stTextRowDes.stDataBox.stRectBorderColor.nBlue = 255;
				pRow->stTextRowDes.stDataBox.stRectBorderColor.nTransparency = 255;
				pRow->stTextRowDes.stDataBox.stRectFillColor.nRed = 0;
				pRow->stTextRowDes.stDataBox.stRectFillColor.nGreen = 0;
				pRow->stTextRowDes.stDataBox.stRectFillColor.nBlue = 0;
				pRow->stTextRowDes.stDataBox.stRectFillColor.nTransparency = 255;
				pRow->stTextRowDes.stDataBox.nRectZOrder=0 ;
				pRow->stTextRowDes.stDataBox.nRectBorderType=0 ;
			
				pRow->stTextRowDes.nHorizontalJustification = 2;
				if(tagAlign >= 0)
				{
					if(tagAlign == 0 || tagAlign == 3 )
					{
						pRow->stTextRowDes.nHorizontalJustification = 0;
					}
					else if(tagAlign == 1 || tagAlign == 4)
					{	
						pRow->stTextRowDes.nHorizontalJustification = 1;
					}
					else if(tagAlign == 2)
					{
						pRow->stTextRowDes.nHorizontalJustification = 2;
					}
				}
				pRow->stTextRowDes.nPrintDirection=0;
				pRow->pTextInfoEntry =	NULL;//&m_textInfo;
				m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nTop = pRow->stTextRowDes.stDataBox.stRect.nTop;
				m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nLeft = pRow->stTextRowDes.stDataBox.stRect.nLeft;
				m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nBottom = pRow->stTextRowDes.stDataBox.stRect.nBottom;
				m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nRight = pRow->stTextRowDes.stDataBox.stRect.nRight;
				nLine++;
				isNewRow = false;
			}
			tmpPointer[1] = addrList[listnode + 1];
			copyLength = strlen(tmpPointer[0]) - strlen(tmpPointer[1]);
			memset(text,0,sizeof(char)* 1024);
			memcpy(text,tmpPointer[0],copyLength);
			CCharsetDecoder dec;
			dec.SetFileFormat(pTrack->GetFileFormat());
			dec.SetLanguage(pTrack->GetLanguage());
			int nWClen = 2;
#if (defined _IOS || defined _MAC_OS) 
			nWClen = sizeof(wchar_t);
#endif
			FilterSpecialCharacter(text, NULL);
			int nOut = strlen(text) *nWClen;
			char* chOut1 = new char[nOut+8];
			if(!chOut1)
				return NULL;
			memset(chOut1,0,nOut+8);
			
			VOLOGI("text is %s",text);
			VOLOGI("Out is %d",nOut);
			dec.ToUtf16(m_jniEnv, text, (VO_PBYTE)chOut1, &nOut);
			VOLOGI("afterToUtf16 Out is %d",nOut);
			voSubtitleTextInfoEntry* pTextInfo = AddNewTextInfoEntry(pRow);
			pTextInfo->pNext = NULL;
			pTextInfo->nSize = nOut;
			if(pTextInfo->pString!= NULL)
			{
				delete [] pTextInfo->pString;
				pTextInfo->pString = NULL;
			}
			pTextInfo->pString = (VO_PBYTE)chOut1;
			if(isBold)
				pTextInfo->stStringInfo.stFontInfo.FontSize = 102;
			else
				pTextInfo->stStringInfo.stFontInfo.FontSize = 103;
			pTextInfo->stStringInfo.stFontInfo.FontStyle = 0;
			pTextInfo->stStringInfo.stFontInfo.stFontColor.nRed = 255;
			pTextInfo->stStringInfo.stFontInfo.stFontColor.nGreen = 255;
			pTextInfo->stStringInfo.stFontInfo.stFontColor.nBlue = 255;
			pTextInfo->stStringInfo.stFontInfo.stFontColor.nTransparency = 255;
			
			pTextInfo->stStringInfo.stCharEffect.TextTag = 0;
			if(isItalic)
				pTextInfo->stStringInfo.stCharEffect.Italic = 1;
			else
				pTextInfo->stStringInfo.stCharEffect.Italic = 0;
			
			if(isUnderLine)
				pTextInfo->stStringInfo.stCharEffect.Underline = 1;
			else
				pTextInfo->stStringInfo.stCharEffect.Underline = 0;
	//		VOLOGI("Ferry text is %s ,bold is %d,Italic is %d,underline is %d",text,pTextInfo->stStringInfo.stFontInfo.FontSize,pTextInfo->stStringInfo.stCharEffect.Italic,pTextInfo->stStringInfo.stCharEffect.Underline);
			pTextInfo->stStringInfo.stCharEffect.EdgeType = 0;
			pTextInfo->stStringInfo.stCharEffect.Offset = 0;
			pTextInfo->stStringInfo.stCharEffect.EdgeColor.nRed = 255;
			pTextInfo->stStringInfo.stCharEffect.EdgeColor.nGreen = 255;
			pTextInfo->stStringInfo.stCharEffect.EdgeColor.nBlue = 255;
			pTextInfo->stStringInfo.stCharEffect.EdgeColor.nTransparency = 255;

			m_subtitleInfo.nTimeStamp = pTextItem->m_nStartTime;
			m_subtitleInfo.pSubtitleEntry->nDuration = pTextItem->m_nEndTime - pTextItem->m_nStartTime;

			listnode++;
			if(listnode >= 32)
				break;
		}
	}
	m_nStartTimeLastTime = pTextItem->m_nStartTime;
	m_nEndTimeLastTime = pTextItem->m_nEndTime;

	return &m_subtitleInfo;
}
voSubtitleInfo* CSubtitleHtmlMgr::GetCurrentTtmlText(CBaseSubtitleTrack* pTrack, CTextItem * pTextItem,VO_S64 uPos)
{
	char ch2[2048];
	//SUBTITLE_FILEFORMAT_UTF8
	int nAll = 0;

	VO_U32 uRowNum = 0;
	while(1)
	{
		//AddNewRow
		voSubtitleTextRowInfo* rowInfo = NULL;
		voSubtitleTextInfoEntry* textInfoEntry = NULL;
		VO_S64 llSpanEndTime = -1;
		for(int i = 0;i<TEXT_ITEM_COUNT;i++)
		{
			VO_S32 RowPercent = -1;
			VO_S32 ColumnPercent = -1;
			memset(ch2,0,2048);
			if(pTextItem->m_aText[i]==NULL)
			{
				break;
			}
			int topMin = -1;
			int bottomMax = -1;
			int rightMax = -1;
			int leftMin = -1;
			if(strlen(pTextItem->m_aText[i])<1022 && nAll+strlen(pTextItem->m_aText[i])<2040)
			{
				
				//to set style of <p>;
				TT_Style stl;
				TT_Style* pStl = this->GetTTStyle(m_strBodyStyle,true);
				if(pStl)
					stl = *pStl;

				strcpy(ch2,pTextItem->m_aText[i]);
				char outAttributeVal[64];
				memset(outAttributeVal, 0, 64);
				if(CXmlParserWraper::GetAttribute(ch2, "style", outAttributeVal, 60))
				{
					pStl = this->GetTTStyle(outAttributeVal,true);
					if(pStl)
						stl = *pStl;
				}
				memset(outAttributeVal, 0, 64);
				if(CXmlParserWraper::GetAttribute(ch2, "tts:textAlign", outAttributeVal, 60))
				{
					stl.textAlign = stl.textAlignToInt(outAttributeVal);
				}
				pStl = this->GetTTStyle(m_strDivStyle,true);
				if (NULL != pStl)
				{
					if (strlen(pStl->region.strID))
					{
						TT_Style* pRegionStyle = NULL;
						if(NULL != (pRegionStyle = this->GetTTStyle(pStl->region.strID,true))
							&& pRegionStyle->ColumnCnt > 0 && pRegionStyle->RowCnt > 0
							&& pRegionStyle->region.xPos >= 0
							&& pRegionStyle->region.yPos >= 0)
						{
							RowPercent = (pRegionStyle->region.xPos)*100 / (pRegionStyle->ColumnCnt);
							ColumnPercent = (pRegionStyle->region.yPos)*100/(pRegionStyle->RowCnt) + 6*uRowNum;
						}
					}
					if (strlen(pStl->StyleId))
					{
						TT_Style* pGolStyle = NULL;
						if(NULL != (pGolStyle = this->GetTTStyle(pStl->StyleId,true)))
						{
							stl.StyleReplace(*pGolStyle);
						}
					}
					stl.StyleReplace(*pStl);
				}
				char* subNodeBegin = NULL;
				char* subNodeEnd = NULL;
				int nRow = 12;
				int nCol = 0;
				if(CXmlParserWraper::GetSubNode(ch2, "metadata", subNodeBegin, subNodeEnd))
				{
					if(CXmlParserWraper::GetAttribute(subNodeBegin, "ccrow", outAttributeVal, 60))
					{
						nRow = atoi(outAttributeVal);
					}
					if(CXmlParserWraper::GetAttribute(subNodeBegin, "cccol", outAttributeVal, 60))
					{
						nCol = atoi(outAttributeVal);
					}
					CXmlParserWraper::RemoveSubNode(ch2,subNodeBegin, subNodeEnd);
				}

				if(CXmlParserWraper::GetAttribute(ch2, "region", outAttributeVal, 60))
				{
					pStl = this->GetTTStyle(outAttributeVal,true);
					if (NULL != pStl && pStl->ColumnCnt > 0 && pStl->RowCnt > 0)
					{
						RowPercent = (pStl->region.xPos)*100 / (pStl->ColumnCnt);
						ColumnPercent = (pStl->region.yPos)*100/(pStl->RowCnt);
						stl.StyleReplace(*pStl);
					}
				}

				CTimedTextMLParser::FillAttribute(&stl, ch2);
				TT_Style paragraphStyle = stl;
				paragraphStyle.fontSize = TranslateFontSizeTTML(stl.fontSize);

				CXmlParserWraper::RemoveNodeHead(ch2);

				subNodeBegin = NULL;
				subNodeEnd = NULL;
				char chOldBR = 0;
				bool bhasBR = true;
				
				m_subtitleInfo.nTimeStamp = pTextItem->m_nStartTime;
				m_subtitleInfo.pSubtitleEntry->nDuration = pTextItem->m_nEndTime - pTextItem->m_nStartTime;
				
				while(bhasBR){
					if(CXmlParserWraper::GetSubNode(ch2, "br", subNodeBegin, subNodeEnd))
					{
						chOldBR = subNodeBegin[0];
						subNodeBegin[0]=0;
					}
					else
						bhasBR = false;
					//FilterSpecialCharacter( ch2, &nTextLength);
					if(!bhasBR)
					{//remove tail
						CXmlParserWraper::RemoveNodeTail(ch2);
					}

					nAll=strlen(ch2)+2;
					if(nAll>2046)
						break;
					if(nAll>0)
					{
						rowInfo = this->AddNewRow();
						if(rowInfo == NULL)
							break;
						textInfoEntry = this->AddNewTextInfoEntry(rowInfo);
						if(textInfoEntry == NULL)
							break;

						int rectTop = 2;
						int rectLeft = 2;
						if(nRow!=-1)
							rectTop = 2+nRow*6;
						if(nCol!=-1)
							rectLeft = 2+nCol*3;
						rowInfo->stTextRowDes.nHorizontalJustification=stl.textAlign;
						rowInfo->stTextRowDes.stDataBox.stRect.nLeft=rectLeft;
						rowInfo->stTextRowDes.stDataBox.stRect.nRight=100-rectLeft;
						rowInfo->stTextRowDes.stDataBox.stRect.nTop=rectTop;
						rowInfo->stTextRowDes.stDataBox.stRect.nBottom=rectTop+6;
						if (RowPercent >= 0 && ColumnPercent >= 0)
						{
							rowInfo->stTextRowDes.stDataBox.stRect.nLeft=RowPercent;
							rowInfo->stTextRowDes.stDataBox.stRect.nRight=100-RowPercent;
							rowInfo->stTextRowDes.stDataBox.stRect.nTop=ColumnPercent;
							rowInfo->stTextRowDes.stDataBox.stRect.nBottom=ColumnPercent+6;
						}

						if(topMin == -1 || topMin > rowInfo->stTextRowDes.stDataBox.stRect.nTop)
							topMin = rowInfo->stTextRowDes.stDataBox.stRect.nTop;
						if(leftMin == -1 || leftMin > rowInfo->stTextRowDes.stDataBox.stRect.nLeft)
							leftMin = rowInfo->stTextRowDes.stDataBox.stRect.nLeft;
						if(bottomMax == -1 || bottomMax < rowInfo->stTextRowDes.stDataBox.stRect.nBottom)
							bottomMax = rowInfo->stTextRowDes.stDataBox.stRect.nBottom;
						if(rightMax == -1 || rightMax < rowInfo->stTextRowDes.stDataBox.stRect.nRight)
							rightMax = rowInfo->stTextRowDes.stDataBox.stRect.nRight;


						CCharsetDecoder dec;
						dec.SetFileFormat(pTrack->GetFileFormat());
						dec.SetLanguage(pTrack->GetLanguage());

						//to check span  node
						char* subSpanNodeBegin = NULL;
						char* subSpanNodeEnd = NULL;
						bool bAddTop = false;
						TT_Style stlTmp;
						stlTmp = stl;
						while(true)
						{
							//bUseSpanColor = false;
							char chOld = 0;
							stl = stlTmp;
							if(subSpanNodeBegin == NULL && subSpanNodeEnd == NULL)
							{
								CXmlParserWraper::GetSubNode(ch2, "span", subSpanNodeBegin, subSpanNodeEnd);
								if(subSpanNodeBegin != NULL && subSpanNodeEnd != NULL)
								{
									subSpanNodeBegin[0] = 0;//TO GET TEXT BEFORE SPAN
								}
								else
								{
									subSpanNodeBegin = NULL;
									subSpanNodeEnd = NULL;
								}
							}
							else if(subSpanNodeBegin != NULL && subSpanNodeEnd != NULL)
							{
								subSpanNodeBegin[0] = '<';
								CTimedTextMLParser::FillAttribute(&stl, subSpanNodeBegin);
								{
									char pVal[64];// = NULL;
									memset(pVal,0,64);
									int nHour1=0, nMin1=0, nSec1=0, nMS1=0;
									VO_S64 llBeginTimeSpan = -1;
									if(CXmlParserWraper::GetAttribute(subSpanNodeBegin, "begin", pVal, 60))
									{
										CTimedTextMLParser::StrToTime (pVal,  nHour1, nMin1, nSec1, nMS1);
										llBeginTimeSpan = (nHour1 * 3600 + nMin1 * 60 + nSec1) * 1000 + nMS1*10 + m_llBaseTime;
									}
									if (llBeginTimeSpan > uPos)
									{
										llSpanEndTime = llBeginTimeSpan;
										break;
									}

								}
								memcpy(ch2, subSpanNodeBegin, subSpanNodeEnd-subSpanNodeBegin);
								chOld = subSpanNodeEnd[0];
								ch2[subSpanNodeEnd-subSpanNodeBegin] = 0;
								subSpanNodeBegin = NULL;
							}
							else
								break;

							FilterSpecialCharacter(ch2, NULL);
							int nWClen = 2;
#if (defined _IOS || defined _MAC_OS) 
							nWClen = sizeof(wchar_t);
#endif
							int nOut = strlen(ch2)*nWClen+8;
							char* chOut1 = new char[nOut+2];
							if(!chOut1)
								return NULL;
							memset(chOut1,0,nOut+2);
							dec.ToUtf16(m_jniEnv, ch2, (VO_PBYTE)chOut1, &nOut);
							textInfoEntry->nSize = nOut;
							//	VOLOGI("---> Subtitle count:%d :Text %s",nOut, ch2);
							if(textInfoEntry->pString!= NULL)
							{
								delete [] textInfoEntry->pString;
								textInfoEntry->pString = NULL;
							}
							textInfoEntry->pString = (VO_PBYTE)chOut1;
							textInfoEntry->stStringInfo.stFontInfo.stFontColor = stl.color;
							rowInfo->stTextRowDes.stDataBox.stRectFillColor=stl.backgroundColor;
							if(subtitle_stricmp(stl.fontStyle,"Italic")==0)
								textInfoEntry->stStringInfo.stCharEffect.Italic = 1;
							if(subtitle_stricmp(stl.fontStyle,"Underline")==0 || subtitle_stricmp(stl.textDecoration,"Underline")==0)
								textInfoEntry->stStringInfo.stCharEffect.Underline = 1;
							textInfoEntry->stStringInfo.stFontInfo.FontStyle = stl.nFontFamily;
							stl.fontSize = TranslateFontSizeTTML(stl.fontSize,paragraphStyle.fontSize);
							if (stl.fontSize >= 0)
							{
								textInfoEntry->stStringInfo.stFontInfo.FontSize = stl.fontSize+SCALE_FOR_FONT_SIZE;
							}
							if (textInfoEntry->stStringInfo.stFontInfo.FontSize == 0)
							{
								textInfoEntry->stStringInfo.stFontInfo.FontSize = 100 + SCALE_FOR_FONT_SIZE;
							}
							else if (textInfoEntry->stStringInfo.stFontInfo.FontSize == 1)
							{
								textInfoEntry->stStringInfo.stFontInfo.FontSize = 50 + SCALE_FOR_FONT_SIZE;
							}
							else if (textInfoEntry->stStringInfo.stFontInfo.FontSize == 2)
							{
								textInfoEntry->stStringInfo.stFontInfo.FontSize = 200 + SCALE_FOR_FONT_SIZE;
							}

							if(subSpanNodeBegin == NULL && subSpanNodeEnd == NULL)
								break;
							if(subSpanNodeBegin == NULL && subSpanNodeEnd != NULL)
							{
								subSpanNodeEnd[0] = chOld;
								strcpy(ch2, subSpanNodeEnd);
								subSpanNodeEnd = NULL;
							}
							textInfoEntry = this->AddNewTextInfoEntry(rowInfo);
							if(textInfoEntry == NULL)
								break;
						}
					}

					if(bhasBR && subNodeEnd)
						strcpy(ch2,subNodeEnd);
					subNodeBegin = NULL;
					subNodeEnd = NULL;
					nRow++;
				}
				m_subtitleInfoEntry.stSubtitleDispInfo.stDispDescriptor.stWrap = stl.wrapEnable;
			}

			this->m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nTop = topMin;
			this->m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nLeft = leftMin;
			this->m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nBottom = bottomMax;
			this->m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nRight = rightMax;

		}
		m_nStartTimeLastTime = pTextItem->m_nStartTime;
		if (llSpanEndTime != -1)
		{
			m_nEndTimeLastTime = llSpanEndTime;
		}
		else if(uRowNum == 0/* || m_nEndTimeLastTime > pTextItem->m_nEndTime*/)
		{
			m_nEndTimeLastTime = pTextItem->m_nEndTime;
		}

		if (NULL != pTextItem->m_pNext)
		{
			if (pTextItem->m_pNext->m_nStartTime <= uPos)
			{
				pTextItem = pTextItem->m_pNext;
				uRowNum++;	
			}
			else
			{
				if (pTextItem->m_pNext->m_nStartTime <  m_nEndTimeLastTime)
				{
					m_nEndTimeLastTime = pTextItem->m_pNext->m_nStartTime;
				}
				break;
			}
		}
		else
			break;
	}
	m_subtitleInfo.pSubtitleEntry->nDuration = m_nEndTimeLastTime - m_nStartTimeLastTime;
	this->m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nBottom += uRowNum*6;
	return &m_subtitleInfo;
}

voSubtitleInfo* CSubtitleHtmlMgr::GetCurrentSMPTETtmlText(CBaseSubtitleTrack* pTrack, CTextItem * pTextItem,VO_S64 uPos)
{
	ImageInfo* pInfo = (ImageInfo*)pTextItem->m_pImage;
	if (pInfo)
	{
		m_subtitleInfo.nTimeStamp = pTextItem->m_nStartTime;
		m_subtitleInfo.pSubtitleEntry->nDuration = pTextItem->m_nEndTime - pTextItem->m_nStartTime;
		m_subtitleInfo.pSubtitleEntry->stSubtitleRectInfo.stRect.nLeft = pInfo->uXco;
		m_subtitleInfo.pSubtitleEntry->stSubtitleRectInfo.stRect.nRight = pInfo->uXco + pInfo->uImaWidth;
		m_subtitleInfo.pSubtitleEntry->stSubtitleRectInfo.stRect.nTop = pInfo->uYco;
		m_subtitleInfo.pSubtitleEntry->stSubtitleRectInfo.stRect.nBottom = pInfo->uYco + pInfo->uImaHeight;

		m_subtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo = NULL;
		m_subtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo = new voSubtitleImageInfo;
		memset(m_subtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo,0,sizeof(voSubtitleImageInfo));
		m_subtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo->stImageData.nHeight = pInfo->uImaHeight;
		m_subtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo->stImageData.nWidth = pInfo->uImaWidth;
		m_subtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo->stImageData.nSize = pInfo->uBufSize;
		m_subtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo->stImageData.pPicData = (VO_PBYTE)pInfo->pBuf;

		m_subtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo->stImageDesp.stImageRectInfo.stRect.nLeft = pInfo->uXco;
		m_subtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo->stImageDesp.stImageRectInfo.stRect.nRight = pInfo->uXco + pInfo->uImaWidth;
		m_subtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo->stImageDesp.stImageRectInfo.stRect.nTop = pInfo->uYco;
		m_subtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo->stImageDesp.stImageRectInfo.stRect.nBottom = pInfo->uYco + pInfo->uImaHeight;

		VOLOGI("SMPTE_TTML:nTimeStamp=%lld,nDuration=%d",m_subtitleInfo.nTimeStamp,m_subtitleInfo.pSubtitleEntry->nDuration);
		VOLOGI("SMPTE_TTML:nLeft=%d,nRight=%d,nTop=%d,nBottom=%d",m_subtitleInfo.pSubtitleEntry->stSubtitleRectInfo.stRect.nLeft
			,m_subtitleInfo.pSubtitleEntry->stSubtitleRectInfo.stRect.nRight
			,m_subtitleInfo.pSubtitleEntry->stSubtitleRectInfo.stRect.nTop
			,m_subtitleInfo.pSubtitleEntry->stSubtitleRectInfo.stRect.nBottom);
		VOLOGI("SMPTE_TTML:pBuf[0~3]=%x:%x:%x,pInfo->uBufSize=%d",(pInfo->pBuf[0]&0xFF),(pInfo->pBuf[1]&0xFF),(pInfo->pBuf[2]&0xFF),pInfo->uBufSize);
		m_subtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo->stImageData.nType = VO_IMAGE_PNG;

		m_nStartTimeLastTime = pTextItem->m_nStartTime;
		m_nEndTimeLastTime = pTextItem->m_nEndTime;

		return &m_subtitleInfo;
	}
	else
	{
		return GetCurrentTtmlText(pTrack,pTextItem,uPos);
	}
}
voSubtitleInfo* CSubtitleHtmlMgr::GetCurrentNormalText(CBaseSubtitleTrack* pTrack, CTextItem * pTextItem)
{
	char ch2[1024];
	int nLine = 0;
	voSubtitleTextRowInfo* pRow = NULL;
	voSubtitleTextInfoEntry* pTextInfo = NULL;

	for(int i = 0;i < TEXT_ITEM_COUNT;i++)
	{
		if(pTextItem->m_aText[i] == NULL)
		{
			//VOLOGE("Wrong:NULL text pointer");
			break;
		}
		memset(ch2,0,sizeof(char)* 1024);
		if(strlen(pTextItem->m_aText[i]) < 1022)
		{
			strcpy(ch2,pTextItem->m_aText[i]);
			strcat(ch2,"\r\n");
		}
		else
		{
			VOLOGE("Wrong:Text length is more than 1k");
			return NULL;
		}
		
		int nRow = 12;
		int nCol = 0;
		int rectTop = 0;
		int rectLeft = 0;
	
		CCharsetDecoder dec;
		dec.SetFileFormat(pTrack->GetFileFormat());
		dec.SetLanguage(pTrack->GetLanguage());

		int nWClen = 2;
#if (defined _IOS || defined _MAC_OS) 
		nWClen = sizeof(wchar_t);
#endif
		int nOut = strlen(ch2)*nWClen;
		char* chOut1 = new char[nOut+8];
		if(!chOut1)
			return NULL;
		memset(chOut1,0,nOut+8);
		FilterSpecialCharacter(ch2, NULL);
		dec.ToUtf16(m_jniEnv, ch2, (VO_PBYTE)chOut1, &nOut);

		rectTop = (nRow + nLine)*6;
		rectLeft = 2 + nCol*3;
		pRow = AddNewRow();
		if(!pRow)
			return NULL;
		pRow->pNext =	NULL;
		pRow->stTextRowDes.stDataBox.stRect.nTop = rectTop ;
		pRow->stTextRowDes.stDataBox.stRect.nLeft = rectLeft;
		pRow->stTextRowDes.stDataBox.stRect.nBottom = rectTop + 6 ;
		pRow->stTextRowDes.stDataBox.stRect.nRight = 100 - rectLeft;
		pRow->stTextRowDes.stDataBox.stRectBorderColor.nRed = 255;
		pRow->stTextRowDes.stDataBox.stRectBorderColor.nGreen = 255;
		pRow->stTextRowDes.stDataBox.stRectBorderColor.nBlue = 255;
		pRow->stTextRowDes.stDataBox.stRectBorderColor.nTransparency = 255;
		pRow->stTextRowDes.stDataBox.stRectFillColor.nRed = 0;
		pRow->stTextRowDes.stDataBox.stRectFillColor.nGreen = 0;
		pRow->stTextRowDes.stDataBox.stRectFillColor.nBlue = 0;
		pRow->stTextRowDes.stDataBox.stRectFillColor.nTransparency = 255;
		pRow->stTextRowDes.stDataBox.nRectZOrder=0 ;
		pRow->stTextRowDes.stDataBox.nRectBorderType=0 ;
		pRow->stTextRowDes.nPrintDirection=0 ;
		pRow->pTextInfoEntry =	NULL;//&m_textInfo;
		m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nTop = pRow->stTextRowDes.stDataBox.stRect.nTop;
		m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nLeft = pRow->stTextRowDes.stDataBox.stRect.nLeft;
		m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nBottom = pRow->stTextRowDes.stDataBox.stRect.nBottom;
		m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nRight = pRow->stTextRowDes.stDataBox.stRect.nRight;
     //   VOLOGI("top=%d,left=%d,bottom=%d,right=%d",m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nTop,m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nLeft,\
       //        m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nBottom,m_subtitleInfoEntry.stSubtitleRectInfo.stRect.nRight);
		pTextInfo = AddNewTextInfoEntry(pRow);
		pTextInfo->pNext = NULL;
		pTextInfo->nSize = nOut;
		if(pTextInfo->pString!= NULL)
		{
			delete [] pTextInfo->pString;
			pTextInfo->pString = NULL;
		}
		pTextInfo->pString = (VO_PBYTE)chOut1;
		pTextInfo->stStringInfo.stFontInfo.FontSize = 103;
		pTextInfo->stStringInfo.stFontInfo.FontStyle = 0;
		pTextInfo->stStringInfo.stFontInfo.stFontColor.nRed = 255;
		pTextInfo->stStringInfo.stFontInfo.stFontColor.nGreen = 255;
		pTextInfo->stStringInfo.stFontInfo.stFontColor.nBlue = 255;
		pTextInfo->stStringInfo.stFontInfo.stFontColor.nTransparency = 255;
		
		pTextInfo->stStringInfo.stCharEffect.TextTag = 0;
		pTextInfo->stStringInfo.stCharEffect.Italic = 0;
		pTextInfo->stStringInfo.stCharEffect.Underline = 0;
		pTextInfo->stStringInfo.stCharEffect.EdgeType = 0;
		pTextInfo->stStringInfo.stCharEffect.Offset = 0;
		pTextInfo->stStringInfo.stCharEffect.EdgeColor .nRed = 255;
		pTextInfo->stStringInfo.stCharEffect.EdgeColor .nGreen = 255;
		pTextInfo->stStringInfo.stCharEffect.EdgeColor .nBlue = 255;
		pTextInfo->stStringInfo.stCharEffect.EdgeColor .nTransparency = 255;

		m_subtitleInfo.nTimeStamp = pTextItem->m_nStartTime;
		m_subtitleInfo.pSubtitleEntry->nDuration = pTextItem->m_nEndTime - pTextItem->m_nStartTime;
		nLine++;
	}
	m_nStartTimeLastTime = pTextItem->m_nStartTime;
	m_nEndTimeLastTime = pTextItem->m_nEndTime;
	return &m_subtitleInfo;
}
voSubtitleInfo* CSubtitleHtmlMgr::GetCurrentText(int nPos)
{
	if(m_bIsFounded != true)
	{
		return NULL;
	}
	int nTrackIndex = this->n_nTrackIndex;
	if(nPos <0)
	{
		//m_subtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->nSize = 0;
		return NULL;//&m_subtitleInfo;
	}

	if (nTrackIndex >= m_nTrackCount || m_nTrackCount == 0 || m_TrackList==NULL)
	{
		return NULL;//(VO_TCHAR*)m_szTextAll;
	}
	
	CBaseSubtitleTrack* pTrack = (CBaseSubtitleTrack*)m_TrackList[nTrackIndex];
	if(pTrack)
	{
		VO_S64 ullNextRowTime = -1;
		int o_Count = 0;
		CTextItem * pTextItem = (CTextItem *)pTrack->GetCurSubtitleItem();
		//VOLOGI ("GetCurrentText m_nCurrStartTime = %d",m_nCurrStartTime);
		if(pTextItem != NULL && pTextItem->m_pNext)
		{
			ullNextRowTime = pTextItem->m_pNext->m_nStartTime;
		}

		if(nPos>m_nStartTimeLastTime && nPos<= m_nEndTimeLastTime/* && nPos <= ullNextRowTime*/)
			return NULL;		//still in display time,do not clean it
		if(CSubtitleMgr::GetParserType() == VO_SUBTITLE_CodingSMI ||
			CSubtitleMgr::GetParserType() == VO_SUBTITLE_CodingLRC)
		{
			if(nPos > pTrack->GetLastBegin())		//cause SMI and Lrc has no endtime,only start time for reference
				return &m_subNULL;
		}
		else if(CSubtitleMgr::GetParserType() == VO_SUBTITLE_CodingSSTTML)
		{
			if(nPos > pTrack->GetDuration())
				return &m_subNULL;
		}
		else
		{
			VO_S64 ullDuration = pTrack->GetDuration();
			if(nPos > pTrack->GetDuration())
				return &m_subNULL;
		}
		if(nPos<m_nCurrEndTime)
			pTrack->SetPos(nPos);
	}
	else
	{
		return NULL;
	}
	
	int  o_Count;
	m_bEnd = false;
	CTextItem * pTextItem = (CTextItem *)pTrack->GetSubtitleItem(NULL, NULL, NULL, NULL,NULL, NULL,NULL,NULL, NULL,&m_nCurrStartTime, &m_nCurrEndTime, &o_Count, &m_bEnd);
	//VOLOGI ("GetCurrentText m_nCurrStartTime = %d",m_nCurrStartTime);
	if(pTextItem == NULL)
	{
		return NULL;
	}
	
	int nMaxtimes = 0;
	while(m_nCurrEndTime<nPos && !m_bEnd && pTextItem!=NULL)
	{
		nMaxtimes++;
		if(nMaxtimes>100000)
		{
			pTextItem = NULL;
			break;
		}
		pTextItem = (CTextItem *)pTrack->GetSubtitleItem(NULL, NULL, NULL, NULL,NULL, NULL,NULL,NULL, NULL,&m_nCurrStartTime, &m_nCurrEndTime, &o_Count, &m_bEnd);
	}
	if(pTextItem->m_nStartTime>nPos)
	{
		return &m_subNULL;
	}

	CleareSubtitleInfo();
	if(VO_SUBTITLE_CodingWEBVTT== CSubtitleMgr::GetParserType())
	{
		return GetCurrentWebVTTText(pTrack, pTextItem);
	}
//	if(m_styleList.GetCount()>0)
	if(VO_SUBTITLE_CodingTTML == CSubtitleMgr::GetParserType() || VO_SUBTITLE_CodingSSTTML == CSubtitleMgr::GetParserType())
      {
		return GetCurrentTtmlText(pTrack, pTextItem,nPos);
	}
	if(VO_SUBTITLE_CodingSMPTETT == CSubtitleMgr::GetParserType())
	{
		return GetCurrentSMPTETtmlText(pTrack, pTextItem,nPos);
	}
	
	return GetCurrentNormalText(pTrack, pTextItem);
}

void CSubtitleHtmlMgr::Init(int nWidth, int nHeight)
{
}

int  CSubtitleHtmlMgr::SetMediaFile(VO_TCHAR* pFilePath)
{
	int nReturn = CSubtitleMgr::SetMediaFile(pFilePath);
	//VOLOGI("Set Media File return : %d", nReturn);
	return nReturn;
}

bool CSubtitleHtmlMgr::GetTrackInfo( VOOSMP_SUBTITLE_LANGUAGE * pTrackInfo, int nIndex)
{
	if(nIndex>=this->GetTrackCount() || nIndex<0)
		return false;
	CBaseSubtitleTrack* pTrack = (CBaseSubtitleTrack*)m_TrackList[nIndex];
	if(pTrack ==NULL)
		return false;

	memset(pTrackInfo->szLangName,0,sizeof(pTrackInfo->szLangName));
	pTrackInfo->nLanguageType = VOOSMP_LANG_TYPE_UNKNOWN;
	if(pTrack->GetLanguage() == SUBTITLE_LANGUAGE_ENGLISH)
	{
		pTrackInfo->nLanguageType = VOOSMP_LANG_TYPE_ENGLISH;
		strcpy((char*)pTrackInfo->szLangName, "en");
	}
	else if(pTrack->GetLanguage() == SUBTITLE_LANGUAGE_SCHINESE)
	{
		pTrackInfo->nLanguageType = VOOSMP_LANG_TYPE_SIMPLE_CHINESE;
		strcpy((char*)pTrackInfo->szLangName, "sc");
	}
	else if(pTrack->GetLanguage() == SUBTITLE_LANGUAGE_TCHINESE)
	{
		pTrackInfo->nLanguageType = VOOSMP_LANG_TYPE_TRADITIONAL_CHINESE;
		strcpy((char*)pTrackInfo->szLangName, "tc");
	}
	else if(pTrack->GetLanguage() == SUBTITLE_LANGUAGE_KOREAN)
	{
		pTrackInfo->nLanguageType = VOOSMP_LANG_TYPE_KOREAN;
		strcpy((char*)pTrackInfo->szLangName, "kr");
	}
	else if(pTrack->GetLanguage() == SUBTITLE_LANGUAGE_JAPANESE)
	{
		pTrackInfo->nLanguageType = VOOSMP_LANG_TYPE_JAPANESE;
		strcpy((char*)pTrackInfo->szLangName, "jp");
	}
	else if(pTrack->GetLanguage() == SUBTITLE_LANGUAGE_SPANISH)
	{
		pTrackInfo->nLanguageType = VOOSMP_LANG_TYPE_SPANISH;
		strcpy((char*)pTrackInfo->szLangName, "es");
	}
	else if(pTrack->GetLanguage() == SUBTITLE_LANGUAGE_GERMAN)
	{
		pTrackInfo->nLanguageType = VOOSMP_LANG_TYPE_GERMANY;
		strcpy((char*)pTrackInfo->szLangName, "de");
	}
	else
	{
		if (strlen(pTrack->GetLanguageStr()) > 0)
		{
			strcpy((char*)pTrackInfo->szLangName,pTrack->GetLanguageStr());
		}
		else
			VOLOGE("unkonwn language type");
	}
//	strcpy((char*)pTrackInfo->szLangName, pTrack->GetTag());
	VOLOGI("lang is %s,langType is %d",pTrackInfo->szLangName,pTrackInfo->nLanguageType);
	return true;
}

void CSubtitleHtmlMgr::FilterSpecialCharacter(VO_CHAR* pText, int* pnLen)
{
	VO_CHAR* pCur2 = strstr(pText,"<");
	if(pCur2)
	{
		VO_CHAR* pEnd = strstr(pCur2,">");
		while(pCur2 && pEnd)
		{
			strcpy(pCur2,pEnd+1);
			pCur2 = strstr(pText,"<");
			if(pCur2)
				pEnd = strstr(pCur2,">");
		}
	}
	char chSpecial[][32]={"&gt;","&lt;","&apos;","&quot;","&amp;","&#62;","&#60;","&#39;","&#34;","&#38;"};
	char chTarget[] ={'>','<','\'','\"','&','>','<','\'','\"','&'};
	for(int i=0;i<sizeof(chTarget);i++)
	{
		VO_CHAR* pCur = pText;
		while(pCur)
		{
			pCur = strstr(pCur,chSpecial[i]);
			if(pCur == NULL)
				break;
			pCur[0]=chTarget[i];
			strcpy(pCur+1, pCur+strlen(chSpecial[i]));
		}
	}
}

bool	CSubtitleHtmlMgr::IsNumberChar(VO_CHAR cChar)
{
	if (cChar >= 48 && cChar <=57)
		return true;
	else
		return false;
}

VO_S32 CSubtitleHtmlMgr::TranslateFontSizeTTML(VO_S32 uFontSize,VO_S32 ParentSize)
{
	VO_S32 uNewFontSize = uFontSize;
	if (uFontSize> 0 && uFontSize > SCALE_FOR_FONT_EM_SIZE )
	{
		uNewFontSize = uFontSize - SCALE_FOR_FONT_EM_SIZE;
		if (ParentSize > 0)
		{
			uNewFontSize = ParentSize * uNewFontSize /100;
		}
	}
	return uNewFontSize;
}