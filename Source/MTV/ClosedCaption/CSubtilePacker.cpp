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
* @file CSubtitlePacker.cpp
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#include "CSubtilePacker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "voLog.h"
#include "CharacterCode.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

FILE * fDumpFile = NULL;

VO_U32 TranslateColNoToPos(VO_U32 nColumnNo,VO_BOOL bSafeArea)
{
	if (nColumnNo > 33)  //to match isColumnNo is 32,and right col is 32+1;task 19956
	{
		VOLOGE("ClosedCaption Error:Error nColumnNo");
		return 0;
	}
	VO_U32 Pos = (nColumnNo * 25)/10;

	if (bSafeArea)
	{
		return Pos + 10;
	}
	return Pos;
}

VO_U32 TranslatePosToColNo(VO_U32 nPos,VO_BOOL bSafeArea)
{
	VO_U32 nColNo = 0;
	if (bSafeArea)
	{
		nPos -= 10;
	}
	if (nPos == 0)
	{
		return 0;
	}
	
	nColNo = (nPos*10/25) + (((nPos*10) % 25 == 0) ? 0 : 1);
	return nColNo;
}

VO_U32 TranslateRowNoToPos(VO_U32 nRowNo,VO_BOOL bSafeArea)
{
	if (nRowNo > MAX_ROW_COUNT)
	{
		VOLOGE("ClosedCaption Error:Error nRowNo");
		return 0;
	}
	VO_U32 Pos = nRowNo * 5;

	if (bSafeArea)
	{
		return Pos + 10;
	}
	return Pos;
}

VO_U32 TranslatePosToRowNo(VO_U32 nPos,VO_BOOL bSafeArea)
{
	VO_U32 nRowNo = 0;
	if (bSafeArea)
	{
		nPos -= 10;
	}
	if (nPos == 0)
	{
		return 0;
	}

	nRowNo = (nPos/5);
	return nRowNo;
}
VO_VOID SetRectInfo(voSubtitleRect * RectInfo,VO_U32 nMinRow,VO_U32 nMaxRow,VO_U32 nMinCol,VO_U32 nMaxCol,VO_BOOL bSafeArea)
{
	if (!RectInfo)
	{
		return;
	}
	if (nMinRow < 16)
	{
		RectInfo->nTop = TranslateRowNoToPos(nMinRow,bSafeArea);
	}
	if (nMaxRow < 16)
	{
		RectInfo->nBottom = TranslateRowNoToPos(nMaxRow + 1,bSafeArea);
	}

	if (nMinCol < 33)
	{
		RectInfo->nLeft = TranslateColNoToPos(nMinCol,bSafeArea);
	}

	if (nMaxCol < 33)
	{
		RectInfo->nRight = TranslateColNoToPos(nMaxCol + 1,bSafeArea);
	}
}


VO_U32 GetSubtitleEntrySize(VO_PTR pIns,Struct_Type eType)
{
	if(!pIns)
		return 0;
	VO_U32 nSize = 0;
	switch(eType)
	{
	case Struct_Type_voSubtitleInfo:
		{
			voSubtitleInfo * pInfo = (voSubtitleInfo *)pIns;
			nSize += sizeof(voSubtitleInfo);

			pvoSubtitleInfoEntry pTmpEntry = pInfo->pSubtitleEntry;
			while(pTmpEntry)
			{
				nSize += sizeof(voSubtitleInfoEntry);
				pvoSubtitleImageInfo pImageInfo = pInfo->pSubtitleEntry->stSubtitleDispInfo.pImageInfo;
				while(pImageInfo)
				{
					nSize += sizeof(voSubtitleImageInfo);
					pImageInfo = pImageInfo->pNext;
				}

				pvoSubtitleTextRowInfo pRowInfo = pInfo->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo;
				while(pRowInfo)
				{
					nSize += sizeof(voSubtitleTextRowInfo);
					
					pvoSubtitleTextInfoEntry pTextInfoEntry = pRowInfo->pTextInfoEntry;
					while(pTextInfoEntry)
					{
						nSize += sizeof(voSubtitleTextInfoEntry);
						nSize += pTextInfoEntry->nSize;
						pTextInfoEntry = pTextInfoEntry->pNext;
					}
					pRowInfo = pRowInfo->pNext;
				}

				pTmpEntry = pTmpEntry->pNext;
			}
		}
		break;
	default:
		break;
	}
	return nSize;
}

VO_VOID ResetStructEntry(VO_PTR pIns,Struct_Type eType)
{
	if(!pIns)
		return;
	switch (eType)
	{
	case Struct_Type_voSubtitleInfo:
		{

		}
		break;
	case Struct_Type_voSubtitleInfoEntry:
		{
			voSubtitleInfoEntry *pInfoEntry = (voSubtitleInfoEntry *)pIns;
			pInfoEntry->nDuration = 0xFFFFFFFF;
			InitializeStruct(&pInfoEntry->stSubtitleRectInfo,Struct_Type_voSubtitleRectInfo);
			ResetStructEntry(&pInfoEntry->stSubtitleDispInfo,Struct_Type_voSubtitleDisplayInfo);
		}
		break;
	case Struct_Type_voSubtitleRectInfo:
		{
		}
		break;
	case Struct_Type_voSubtitleDisplayInfo:
		{
			voSubtitleDisplayInfo *pDisplayInfo = (voSubtitleDisplayInfo *)pIns;
			InitializeStruct(&pDisplayInfo->stDispDescriptor,Struct_Type_voSubtitleTextDisplayDescriptor);

			pvoSubtitleTextRowInfo pTmpTextRowInfo = (pvoSubtitleTextRowInfo)pDisplayInfo->pTextRowInfo;
			while(pTmpTextRowInfo)
			{
				ResetStructEntry(pTmpTextRowInfo,Struct_Type_voSubtitleTextRowInfo);
				pTmpTextRowInfo = pTmpTextRowInfo->pNext; 
			}

			pvoSubtitleImageInfo pImageInfo = (pvoSubtitleImageInfo)pDisplayInfo->pImageInfo;
			while(pImageInfo)
			{
				InitializeStruct(&pImageInfo->stImageData,Struct_Type_voSubtitleImageInfoData);
				InitializeStruct(&pImageInfo->stImageDesp,Struct_Type_voSubtitleImageInfoDescriptor);
				pImageInfo = pImageInfo->pNext;
			}
		}
		break;
	case Struct_Type_voSubtitleRect:
		{
			
		}
		break;
	case Struct_Type_voSubtitleRGBAColor:
		{
			
		}
		break;
	case Struct_Type_voSubtitleDisPlayEffect:
		{
			
		}
		break;
	case Struct_Type_voSubtitleTextRowInfo:
		{
			pvoSubtitleTextRowInfo pTextRowInfo = (pvoSubtitleTextRowInfo)pIns;

			pvoSubtitleTextInfoEntry pInfoEntry = pTextRowInfo->pTextInfoEntry;
			while(pInfoEntry)
			{
				pInfoEntry->nSize = 0;
				pInfoEntry->pString = NULL;
				InitializeStruct(&pInfoEntry->stStringInfo,Struct_Type_voSubtitleStringInfo);
				
				pInfoEntry = pInfoEntry->pNext;
			}	
			InitializeStruct(&pTextRowInfo->stTextRowDes,Struct_Type_voSubtitleTextRowDescriptor);
		}
		break;
	case Struct_Type_voSubtitleTextDisplayDescriptor:
		{
		
		}
		break;
	case Struct_Type_voSubtitleImageInfo:
		{
		}
		break;
	case Struct_Type_voSubtitleTextInfoEntry:
		{
		
		}
		break;
	case Struct_Type_voSubtitleTextRowDescriptor:
		{
		
		}
		break;
	case Struct_Type_voSubtitleImageInfoData:
		{

		}
		break;
	case Struct_Type_voSubtitleImageInfoDescriptor:
		{

		}
		break;
	case Struct_Type_voSubtitleStringInfo:
		{

		}
		break;
	case Struct_Type_voSubtitleFontInfo:
		{

		}
		break;
	case Struct_Type_voSubtitleFontEffect:
		{
	
		}
		break;
	}
}
VO_VOID InitializeStruct(VO_PTR pIns,Struct_Type eType)
{
	if(!pIns)
		return;
	switch(eType)
	{
	case Struct_Type_voSubtitleInfo:
		{
			voSubtitleInfo * pInfo = (voSubtitleInfo *)pIns;
			pInfo->nTimeStamp = 0xFFFFFFFFFFFFFFFFLL;
			pInfo->pSubtitleEntry = NULL;
		}
		break;
	case Struct_Type_voSubtitleInfoEntry:
		{
			pvoSubtitleInfoEntry pInfoEntry = (pvoSubtitleInfoEntry)pIns;
			pInfoEntry->nDuration = 0xFFFFFFFF;
			InitializeStruct(&pInfoEntry->stSubtitleRectInfo,Struct_Type_voSubtitleRectInfo);
			InitializeStruct(&pInfoEntry->stSubtitleDispInfo,Struct_Type_voSubtitleDisplayInfo);
			pInfoEntry->pNext = NULL;
		}
		break;
	case Struct_Type_voSubtitleRectInfo:
		{
			voSubtitleRectInfo *pRectInfo = (voSubtitleRectInfo *)pIns;
			InitializeStruct(&pRectInfo->stRect,Struct_Type_voSubtitleRect);
			pRectInfo->nRectBorderType = 0;
			InitializeStruct(&pRectInfo->stRectBorderColor,Struct_Type_voSubtitleRGBAColor);
			InitializeStruct(&pRectInfo->stRectFillColor,Struct_Type_voSubtitleRGBAColor);
			pRectInfo->nRectZOrder = 0;
			InitializeStruct(&pRectInfo->stRectDisplayEffct,Struct_Type_voSubtitleDisPlayEffect);
		}
		break;
	case Struct_Type_voSubtitleDisplayInfo:
		{
			voSubtitleDisplayInfo *pDisplayInfo = (voSubtitleDisplayInfo *)pIns;
			pDisplayInfo->pTextRowInfo = NULL;
			InitializeStruct(&pDisplayInfo->stDispDescriptor,Struct_Type_voSubtitleTextDisplayDescriptor);
			pDisplayInfo->pImageInfo = NULL;
		}
		break;
	case Struct_Type_voSubtitleRect:
		{
			voSubtitleRect *pRect = (voSubtitleRect *)pIns;
			pRect->nTop = 0xFFFFFFFF;
			pRect->nLeft = 0xFFFFFFFF;
			pRect->nBottom = 0xFFFFFFFF;
			pRect->nRight = 0xFFFFFFFF;
		}
		break;
	case Struct_Type_voSubtitleRGBAColor:
		{
			voSubtitleRGBAColor *pRGBAColor = (voSubtitleRGBAColor *)pIns;
			pRGBAColor->nRed = 0xFF;
			pRGBAColor->nGreen = 0xFF;
			pRGBAColor->nBlue = 0xFF;
			pRGBAColor->nTransparency = 0xFF;
		}
		break;
	case Struct_Type_voSubtitleDisPlayEffect:
		{
			voSubtitleDisPlayEffect *pDisPlayEffect = (voSubtitleDisPlayEffect *)pIns;
			pDisPlayEffect->nEffectType = 0;
			pDisPlayEffect->nEffectDirection = 0;
			pDisPlayEffect->nEffectSpeed = 0;
		}
		break;
	case Struct_Type_voSubtitleTextRowInfo:
		{
			pvoSubtitleTextRowInfo pTextRowInfo = (pvoSubtitleTextRowInfo)pIns;
			pTextRowInfo->pTextInfoEntry = NULL;
			InitializeStruct(&pTextRowInfo->stTextRowDes,Struct_Type_voSubtitleTextRowDescriptor);
			pTextRowInfo->pNext = NULL;
		}
		break;
	case Struct_Type_voSubtitleTextDisplayDescriptor:
		{
			voSubtitleTextDisplayDescriptor *pTextDisplayDescriptor = (voSubtitleTextDisplayDescriptor *)pIns;
			pTextDisplayDescriptor->stWrap = 0;
			pTextDisplayDescriptor->nScrollDirection = 0;
		}
		break;
	case Struct_Type_voSubtitleImageInfo:
		{
			pvoSubtitleImageInfo  pImage= (pvoSubtitleImageInfo)pIns;
			InitializeStruct(&pImage->stImageData,Struct_Type_voSubtitleImageInfoData);	
			InitializeStruct(&pImage->stImageDesp,Struct_Type_voSubtitleImageInfoDescriptor);
			pImage->pNext = NULL;
		}
		break;
	
	case Struct_Type_voSubtitleTextInfoEntry:
		{
			pvoSubtitleTextInfoEntry pTextInfoEntry = (pvoSubtitleTextInfoEntry)pIns;
			pTextInfoEntry->nSize = 0;
			pTextInfoEntry->pString = NULL;
			InitializeStruct(&pTextInfoEntry->stStringInfo,Struct_Type_voSubtitleStringInfo);
			pTextInfoEntry->pNext = NULL;
		}
		break;
	case Struct_Type_voSubtitleTextRowDescriptor:
		{
			voSubtitleTextRowDescriptor * pTextRowDescriptor = (voSubtitleTextRowDescriptor *)pIns;
			InitializeStruct(&pTextRowDescriptor->stDataBox,Struct_Type_voSubtitleRectInfo);
			pTextRowDescriptor->nHorizontalJustification = 0;
			pTextRowDescriptor->nVerticalJustification = 0;
			pTextRowDescriptor->nPrintDirection = 0;	
		}
		break;
	case Struct_Type_voSubtitleImageInfoData:
		{
			voSubtitleImageInfoData * pImageInfoData = (voSubtitleImageInfoData *)pIns;
			pImageInfoData->nType = VO_IMAGE_Unused;
			pImageInfoData->nWidth = 0;
			pImageInfoData->nHeight = 0;
			pImageInfoData->nSize = 0;
			pImageInfoData->pPicData = NULL;	
		}
		break;
	case Struct_Type_voSubtitleImageInfoDescriptor:
		{
			voSubtitleImageInfoDescriptor *pImgDisplayDescriptor = (voSubtitleImageInfoDescriptor *)pIns;
			InitializeStruct(&pImgDisplayDescriptor->stImageRectInfo,Struct_Type_voSubtitleRectInfo);	
		}
		break;
	case Struct_Type_voSubtitleStringInfo:
		{
			voSubtitleStringInfo * pStringInfo = (voSubtitleStringInfo *)pIns;
			InitializeStruct(&pStringInfo->stFontInfo,Struct_Type_voSubtitleFontInfo);
			InitializeStruct(&pStringInfo->stCharEffect,Struct_Type_voSubtitleFontEffect);
		}
		break;
	case Struct_Type_voSubtitleFontInfo:
		{
			voSubtitleFontInfo * pFontInfo = (voSubtitleFontInfo *)pIns;
			pFontInfo->FontSize = 0;
			pFontInfo->FontStyle = FontStyle_Default;
			InitializeStruct(&pFontInfo->stFontColor,Struct_Type_voSubtitleRGBAColor);
		}
		break;
	case Struct_Type_voSubtitleFontEffect:
		{
			voSubtitleFontEffect * pFontEffect= (voSubtitleFontEffect *)pIns;
			pFontEffect->TextTag = TextTag_Dialog;
			pFontEffect->Italic = 0;
			pFontEffect->Underline = 0;
			pFontEffect->EdgeType = 0;
			pFontEffect->Offset = 0;
			InitializeStruct(&pFontEffect->EdgeColor,Struct_Type_voSubtitleRGBAColor);	
		}
		break;
	
	default:
		break;
	}
}

VO_PTR NewSubTitleEntry(Struct_Type eType)
{
	VO_PTR pIns = NULL;
	switch(eType)
	{
	case Struct_Type_voSubtitleInfo:
		{
			pIns = new voSubtitleInfo;
		}
		break;
	case Struct_Type_voSubtitleInfoEntry:
		{
			pIns = new voSubtitleInfoEntry;
		}
		break;
	case Struct_Type_voSubtitleRectInfo:
		{
			pIns = new voSubtitleRectInfo;
		}
		break;
	case Struct_Type_voSubtitleDisplayInfo:
		{
			pIns = new voSubtitleDisplayInfo;
		}
		break;
	case Struct_Type_voSubtitleRect:
		{
			pIns = new voSubtitleRect;
		}
		break;
	case Struct_Type_voSubtitleRGBAColor:
		{
			pIns = new voSubtitleRGBAColor;
		}
		break;
	case Struct_Type_voSubtitleDisPlayEffect:
		{
			pIns = new voSubtitleRGBAColor;
		}
		break;
	case Struct_Type_voSubtitleTextRowInfo:
		{
			pIns = new voSubtitleTextRowInfo;
		}
		break;
	case Struct_Type_voSubtitleTextDisplayDescriptor:
		{
			pIns = new voSubtitleTextDisplayDescriptor;
		}
		break;
	case Struct_Type_voSubtitleImageInfo:
		{
			pIns = new voSubtitleImageInfo;
		}
		break;
	case Struct_Type_voSubtitleTextInfoEntry:
		{
			pIns = new voSubtitleTextInfoEntry;
		}
		break;
	case Struct_Type_voSubtitleTextRowDescriptor:
		{
			pIns = new voSubtitleTextRowDescriptor;	
		}
		break;
	case Struct_Type_voSubtitleImageInfoData:
		{
			pIns = new voSubtitleImageInfoData;	
		}
		break;
	case Struct_Type_voSubtitleImageInfoDescriptor:
		{
			pIns = new voSubtitleImageInfoDescriptor;	
		}
		break;
	case Struct_Type_voSubtitleStringInfo:
		{
			pIns = new voSubtitleStringInfo;
		}
		break;
	case Struct_Type_voSubtitleFontInfo:
		{
			pIns = new voSubtitleFontInfo;
		}
		break;
	case Struct_Type_voSubtitleFontEffect:
		{
			pIns = new voSubtitleFontEffect;
		}
		break;

	default:
		break;
	}
	if(!pIns)
		return NULL;
	InitializeStruct(pIns,eType);
	return pIns;
}


VO_VOID DelSubTitleEntry(VO_PTR pIns,Struct_Type eType)
{
	if(!pIns)
		return;
	switch(eType)
	{
	case Struct_Type_voSubtitleInfo:
		{
			voSubtitleInfo *pTmp = (voSubtitleInfo *)pIns;
			DelSubTitleEntry(pTmp->pSubtitleEntry,Struct_Type_voSubtitleInfoEntry);
			delete pTmp;
			pTmp = NULL;
		}
		break;
	case Struct_Type_voSubtitleInfoEntry:
		{
			voSubtitleInfoEntry *pTmp = (voSubtitleInfoEntry *)pIns;

			pvoSubtitleTextRowInfo pTextRow = pTmp->stSubtitleDispInfo.pTextRowInfo;
			pvoSubtitleTextRowInfo pTmpTextRow = NULL;
			while(pTextRow)
			{
				pTmpTextRow = pTextRow;
				pTextRow = pTextRow->pNext;
				DelSubTitleEntry(pTmpTextRow,Struct_Type_voSubtitleTextRowInfo);
			}

			pvoSubtitleImageInfo pImage = pTmp->stSubtitleDispInfo.pImageInfo;
			pvoSubtitleImageInfo pTmpImage = NULL;

			while(pImage)
			{
				pTmpImage = pImage;
				pImage = pImage->pNext;
				DelSubTitleEntry(pTmpImage, Struct_Type_voSubtitleImageInfo);
			}
			delete pTmp;
			pTmp = NULL;
		}
		break;
	case Struct_Type_voSubtitleRectInfo:
		{
			voSubtitleRectInfo *pRectInfo = (voSubtitleRectInfo *)pIns;
			delete pRectInfo;
			pRectInfo = NULL;
		}
		break;
	case Struct_Type_voSubtitleDisplayInfo:
		{
			voSubtitleDisplayInfo *pDisplayInfo = (voSubtitleDisplayInfo *)pIns;

			pvoSubtitleTextRowInfo pTextRow = pDisplayInfo->pTextRowInfo;
			pvoSubtitleTextRowInfo pTmpTextRow = NULL;
			while(pTextRow)
			{
				pTmpTextRow = pTextRow;
				pTextRow = pTextRow->pNext;
				DelSubTitleEntry(pTmpTextRow,Struct_Type_voSubtitleTextRowInfo);
			}

			pvoSubtitleImageInfo pImage = pDisplayInfo->pImageInfo;
			pvoSubtitleImageInfo pTmpImage = NULL;
			
			while(pImage)
			{
				pTmpImage = pImage;
				pImage = pImage->pNext;
				DelSubTitleEntry(pTmpImage,Struct_Type_voSubtitleImageInfo);
			}
			delete pDisplayInfo;
			pDisplayInfo = NULL;
		}
		break;
	case Struct_Type_voSubtitleRect:
		{
			voSubtitleRect *pRect = (voSubtitleRect *)pIns;
			delete pRect;
			pRect = NULL;
		}
		break;
	case Struct_Type_voSubtitleRGBAColor:
		{
			voSubtitleRGBAColor *pColor = (voSubtitleRGBAColor *)pIns;
			delete pColor;
			pColor = NULL;
		}
		break;
	case Struct_Type_voSubtitleDisPlayEffect:
		{
			voSubtitleDisPlayEffect *pEffect = (voSubtitleDisPlayEffect *)pIns;
			delete pEffect;
			pEffect = NULL;
		}
		break;
	case Struct_Type_voSubtitleTextRowInfo:
		{
			voSubtitleTextRowInfo *pTmp = (voSubtitleTextRowInfo *)pIns;

			pvoSubtitleTextInfoEntry pInfoEntry = pTmp->pTextInfoEntry;
			pvoSubtitleTextInfoEntry pTmpInfoEntry = NULL;
			while (pInfoEntry)
			{
				pTmpInfoEntry = pInfoEntry;
				pInfoEntry = pInfoEntry->pNext;	
				delete pTmpInfoEntry;
			}
			pTmpInfoEntry = NULL;
			delete pTmp;
			pTmp = NULL;
		}
		break;
	case Struct_Type_voSubtitleTextDisplayDescriptor:
		{
			voSubtitleTextDisplayDescriptor *pTextDisplayDescriptor = (voSubtitleTextDisplayDescriptor *)pIns;
			delete pTextDisplayDescriptor;
			pTextDisplayDescriptor = NULL;
		}
		break;
	case Struct_Type_voSubtitleImageInfo:
		{
			//pPicData is out of memory
			pvoSubtitleImageInfo  pImage= (pvoSubtitleImageInfo)pIns;
			delete pImage;
			pImage = NULL;
		}
		break;

	case Struct_Type_voSubtitleTextInfoEntry:
		{
			//pString is out of band memory
			pvoSubtitleTextInfoEntry pTextInfoEntry = (pvoSubtitleTextInfoEntry)pIns;
			delete pTextInfoEntry;
			pTextInfoEntry = NULL;
		}
		break;
	case Struct_Type_voSubtitleTextRowDescriptor:
		{
			voSubtitleTextRowDescriptor * pTextRowDescriptor = (voSubtitleTextRowDescriptor *)pIns;
			delete pTextRowDescriptor;
			pTextRowDescriptor = NULL;
		}
		break;
	case Struct_Type_voSubtitleImageInfoData:
		{
			voSubtitleImageInfoData * pImageInfoData = (voSubtitleImageInfoData *)pIns;
			delete pImageInfoData;
			pImageInfoData = NULL;
		}
		break;
	case Struct_Type_voSubtitleImageInfoDescriptor:
		{
			voSubtitleImageInfoDescriptor *pImgInfoDescriptor = (voSubtitleImageInfoDescriptor *)pIns;
			delete pImgInfoDescriptor;
			pImgInfoDescriptor = NULL;
		}
		break;
	case Struct_Type_voSubtitleStringInfo:
		{
			voSubtitleStringInfo * pStringInfo = (voSubtitleStringInfo *)pIns;
			delete pStringInfo;
			pStringInfo = NULL;
		}
		break;
	case Struct_Type_voSubtitleFontInfo:
		{
			voSubtitleFontInfo * pFontInfo = (voSubtitleFontInfo *)pIns;
			delete pFontInfo;
			pFontInfo = NULL;
		}
		break;
	case Struct_Type_voSubtitleFontEffect:
		{
			voSubtitleFontEffect * pFontEffect= (voSubtitleFontEffect *)pIns;
			delete pFontEffect;
			pFontEffect = NULL;
		}
		break;

	default:
		break;
	}
}
VO_VOID DumpStringInfo(wchar_t *pString)
{
	if (!fDumpFile)
	{
#ifdef _WIN32
		fDumpFile = fopen("E:\\CCDump.txt","wt+");
#else
		fDumpFile = fopen("/sdcard/CCDump.txt","wt+");
#endif
		if (!fDumpFile)
		{
			return;
		}
	}
	fwrite(pString,1,wcslen(pString)*sizeof(wchar_t),fDumpFile);
	fflush(fDumpFile);
}

#ifdef _VONAMESPACE
}
#endif