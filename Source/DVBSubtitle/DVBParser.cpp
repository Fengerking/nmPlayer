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
* @file DVBParser.cpp
*    
*
*
* @author  Ferry Zhang
* Change History
* 2012-12-24  Create File
************************************************************************/

#include "DVBParser.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE
#endif

#define MIN(a,b) ((a)<(b))?(a):(b)
#define MAX(a,b) ((a)>(b))?(a):(b)

DVBParser::DVBParser()
{
	dvbBufferCache = NULL;
	pages = NULL;
	m_bNeedSend = VO_FALSE;
	displayWidth = 0;
	displayHeight = 0;
	
	if(VO_ERR_DVB_PARSER_OK != Open())
	{
		VOLOGI("DVBParser open failed!");
	}
}

DVBParser::~DVBParser()
{
	Close();
}

VO_U32 DVBParser::Open()
{
	dvbBufferCache = new DVBBufferCache();
	if(!dvbBufferCache)
		return VO_ERR_DVB_PARSER_OUT_OF_MEMORY;
	pages = new voList<DVBPage>();
	if(!pages)
		return VO_ERR_DVB_PARSER_OUT_OF_MEMORY;
	m_stSubtitleInfo.pSubtitleEntry = (pvoSubtitleInfoEntry)NewSubTitleEntry(Struct_Type_voSubtitleInfoEntry);
	if(!m_stSubtitleInfo.pSubtitleEntry)
		return VO_ERR_DVB_PARSER_OUT_OF_MEMORY;
	InitializeStruct(m_stSubtitleInfo.pSubtitleEntry,Struct_Type_voSubtitleInfoEntry);
	m_stNullSubtitleInfo.pSubtitleEntry = (pvoSubtitleInfoEntry)NewSubTitleEntry(Struct_Type_voSubtitleInfoEntry);
	if(!m_stNullSubtitleInfo.pSubtitleEntry)
		return VO_ERR_DVB_PARSER_OUT_OF_MEMORY;
	InitializeStruct(m_stNullSubtitleInfo.pSubtitleEntry,Struct_Type_voSubtitleInfoEntry);
		
	return VO_ERR_DVB_PARSER_OK;
}

VO_U32 DVBParser::Close()
{
	if(dvbBufferCache)
	{
		delete	dvbBufferCache;
		dvbBufferCache = NULL;
	}
	if(pages)
	{
		delete pages;
		pages = NULL;
	}		
	if(m_stSubtitleInfo.pSubtitleEntry)
		DelSubTitleEntry(m_stSubtitleInfo.pSubtitleEntry,Struct_Type_voSubtitleInfoEntry);
	if(m_stNullSubtitleInfo.pSubtitleEntry)
		DelSubTitleEntry(m_stNullSubtitleInfo.pSubtitleEntry,Struct_Type_voSubtitleInfoEntry);
	return VO_ERR_DVB_PARSER_OK;
}

VO_U32 DVBParser::Process(VO_DVB_PARSER_INPUT_BUFFER* pBuffer)
{
	if(!pBuffer)
		return VO_ERR_DVB_PARSER_INVALID_ARG;
	VO_U32 nCount = 0;
	VO_U32	nSize = pBuffer->nSize;
	VO_U64  nPts = pBuffer->nTimeStamp;
	VO_PBYTE pData = pBuffer->pData;
	if(!pData)
		return VO_ERR_DVB_PARSER_INVALID_ARG;

	dvbBufferCache->Reset();
	
	if(nSize > 3)
	{

		if(pData[0] == 0x20 && pData[1] == 0x00 && pData[2] == 0x0F)
			dvbBufferCache->Put(pData + 2, nSize -2);
		else
		{
			VOLOGE("DVBParser input data error!");
			return VO_ERR_DVB_PARSER_ERROR;
		}
		
		while (VO_TRUE)
		{
			VO_PBYTE pSegment = dvbBufferCache->Get(nCount);
			if(pSegment && pSegment[0] == 0x0F)
			{
				if(ParseData(pSegment, nCount, nPts) == -1)
					break;
			}
			else 
				break;
		}
	}
	return VO_ERR_DVB_PARSER_OK;
}


VO_U32 DVBParser::GetData(VO_DVB_PARSER_OUTPUT_BUFFER* pBuffer)
{
	if(m_bNeedSend)
	{
		pBuffer->pSubtitleInfo = &m_stSubtitleInfo;
		m_bNeedSend = VO_FALSE;
		VOLOGI("Data need Send, ts is %d",(int)pBuffer->pSubtitleInfo->nTimeStamp);
	}
	else
	{
		pBuffer->pSubtitleInfo = &m_stNullSubtitleInfo;
		VOLOGI("NULL struct has been send,ts is %d",(int)pBuffer->pSubtitleInfo->nTimeStamp);
	}
	return VO_ERR_DVB_PARSER_OK;
}


VO_U32 DVBParser::ParseData(VO_PBYTE pData, VO_U32 nSize, VO_U64 pts)
{
	if(nSize > 5 && pData[0] == 0x0F)
	{
		VO_U32 nSegmentSize = (pData[4] << 8) + pData[5] + 6;
		if(nSegmentSize > nSize)
			return -1;
		VO_U32 nSegmentType = pData[1];
		VO_U32 nPageID = (pData[2] << 8) + pData[3];
		DVBPage* page = NULL;

		
		for(DVBPage *dp = pages->First(); dp; dp = pages->Next(dp))
		{
			if(dp->GetPageID() == nPageID)
			{
				page = dp;
				break;
			}
		}
		if(NULL == page)
		{
			page = new DVBPage(nPageID);
			if(!page)
				return -1;
			pages->Add(page);
		}
		
		if(pts != 0)
			page->SetPts(pts);

		//fix issue 23097: no END_OF_DISPLAY_SET_SEGMENT flag,so update clear screen ts here
		if(m_stNullSubtitleInfo.nTimeStamp != pts)
			m_stNullSubtitleInfo.nTimeStamp = pts;
		
		switch(nSegmentType)
		{
			case PAGE_SEGMENT:
			{
				VOLOGI("PAGE_SEGMENT");
				VO_U32 pageVersion = (pData[7] & 0xF0) >> 4;
				if(pageVersion == page->GetPageVerison())
				{
					VOLOGW("page version is the same as before");
				//	break;
				}
				page->SetPageVersion(pageVersion);
				page->SetTimeout(pData[6]);
				page->SetPageState(pData[7] & 0x0C >> 2);
				page->regions.Clear();

				for(VO_U32 i = 8; i < nSegmentSize; i += 6)
				{
					DVBRegion *region = page->GetRegionByID(pData[i], VO_TRUE);
					if(!region)
						break;
					region->SetHorizontalAddress((pData[i + 2] << 8) + pData[i + 3]);
					region->SetVerticalAddress((pData[i + 4] << 8) + pData[i + 5]);
				}
				break;
			}
			case REGION_SEGMENT:
			{
				VOLOGI("REGION_SEGMENT");
				DVBRegion* region = page->GetRegionByID(pData[6]);
				if(NULL == region)
					break;
				VO_U32	regionVersion = (pData[7] & 0xF0) >> 4;
				if(regionVersion == region->GetRegionVersion())
				{
					VOLOGW("region version is the same as before");
					//break;
				}
				region->SetRegionVersion(regionVersion);
				VO_U32 regionFillFlag = (pData[7] & 0x08) >> 3;
				VO_U32 	regionWidth = (pData[8] << 8) | pData[9];
				VO_U32	regionHeight = (pData[10] << 8) | pData[11];
				region->SetSize(regionWidth, regionHeight);
				region->SetLevel((pData[12] & 0xE0) >> 5);
				region->SetDepth((pData[12] & 0x1C) >> 2);
				region->SetClutID(pData[13]);
				DVBClut* dc = page->GetClutByID(pData[13], VO_FALSE);
				if(dc)
				{
					page->UpdateRegionPalette(dc);
				}
				

				if(regionFillFlag)
				{
					switch (region->GetDepth())
					{
						case 2:
						{
							region->FillRegion((pData[15] & 0x0C) >> 2);
							break;
						}
						case 4:
						{
							region->FillRegion((pData[15] & 0xF0) >> 4);
							break;
						}
						case 8:
						{
							region->FillRegion((pData[14]));
							break;
						}
					}
				}
				for(VO_U32 i = 16; i < nSegmentSize; i += 6)
				{
					DVBObject* object = region->GetObjectByID((pData[i] << 8) | pData[i + 1], VO_TRUE);
					if(!object)
						break;
					VO_U32	objectType = (pData[i + 2] & 0xC0) >> 6;
					object->SetCodingMethod(objectType);
					object->SetProviderFlag((pData[i + 2] & 0x30) >> 4);
					VO_U32	objectHorizontalPosition = ((pData[i + 2] & 0x0F) << 8) | pData[i + 3];
					VO_U32	objectVerticalPosition = ((pData[i + 4] & 0x0F) << 8) | pData[i + 5];
					object->SetPosition(objectHorizontalPosition,objectVerticalPosition);
					if(objectType == 0x01 || objectType == 0x02)
					{
						object->SetFgColor(pData[i + 6]);
						object->SetBgColor(pData[i + 7]);
						i += 2;
					}
				}
				break;
			}
			case CLUT_SEGMENT:
			{
				VOLOGI("CLUT_SEGMENT");
				DVBClut *clut = page->GetClutByID(pData[6], VO_TRUE);
				if(!clut)
					break;
				VO_U32	clutVersion = (pData[7] & 0xF0) >> 4;
				if(clutVersion == clut->GetClutVersion())
				{	
					VOLOGW("clut version is the same as before");
				//	break;
				}
				clut->SetClutVersion(clutVersion);
				page->UpdateRegionPalette(clut);
				for(VO_U32 i = 8; i < nSegmentSize; i += 2)
				{
					VO_U8 clutEntryID = pData[i];
					VO_U32 fullRangeFlag = pData[i + 1] & 0x01;
					VO_U8 y,cb,cr,t;
					if(fullRangeFlag)
					{
						y	= pData[i + 2];
              	 	  	  	cr 	= pData[i + 3];
              			     	cb 	= pData[i + 4];
                			 	t  	= pData[i + 5];
					}
					else
					{
						y	=   pData[i + 2] & 0xFC;
                 				cr	=  ((pData[i + 2] & 0x03) << 6) | ((pData[i + 3] & 0xC0) >> 2);
              				cb 	=  (pData[i + 3] & 0x3C) << 2;
              				t 	=  (pData[i + 3] & 0x03) << 6;
					}
					VO_U32	color = 0;
					if(y)
					{
						color = yuv2rgb(y,cb,cr);
						color |= (255- t) << 24;
						//color = color << 8 | (255-t);
					}
					VO_U32	EntryFlags = pData[i + 1];
					if ((EntryFlags & 0x80) != 0)
		               	    clut->SetColor(2, clutEntryID, color);
		            		if ((EntryFlags & 0x40) != 0)
		              	     clut->SetColor(4, clutEntryID, color);
		           	      if ((EntryFlags & 0x20) != 0)
		               	    clut->SetColor(8, clutEntryID, color);
		                   i += fullRangeFlag ? 4 : 2;
				}
				page->UpdateRegionPalette(clut);
				break;
			}
			case OBJECT_SEGMENT:
			{	
				VOLOGI("OBJECT_SEGMENT");
				DVBObject *object = page->GetObjectByID((pData[6] << 8) | pData[6 + 1]);
				if(NULL == object)
					break;
				VO_U32	objectVersion = (pData[8] & 0xF0) >> 4;
				if(objectVersion == object->GetObjectVersion())
				{
					VOLOGW("object version is the same");
					//break;
				}
				object->SetObjectVerison(objectVersion);
				VO_U32	codingMethod = (pData[8] & 0x0C) >> 2;
				object->SetNonModifyingColorFlag(VO_BOOL(pData[8] & 0x01));
				if(codingMethod == 0)
				{
					VO_U32 i = 6 + 3;
					VO_U32 topFieldLength = (pData[i] << 8) | pData[i + 1];
					VO_U32 bottomFieldLength = (pData[i + 2] << 8) | pData[i + 3];
					object->DecodeSubBlock(pData + i + 4,topFieldLength, VO_TRUE);
					if(bottomFieldLength)
						object->DecodeSubBlock(pData + i + 4 + topFieldLength,bottomFieldLength, VO_FALSE);
					else
						object->DecodeSubBlock(pData + i + 4, topFieldLength, VO_FALSE);
						
				}
				else if(codingMethod == 1)
				{
					VOLOGE("text object is not supported now!");
					//TODO
				}
				break;
			}
			case DISPLAY_SEGMENT:
			{
				VOLOGI("DISPLAY_SEGMENT");
				displayWidth = (pData[7] << 8) | pData[8];
				displayHeight = (pData[9] << 8) | pData[10];
				
				break;
			}
			case END_OF_DISPLAY_SET_SEGMENT:
			{
				VOLOGI("END_OF_DISPLAY_SET_SEGMENT");
				PageTranslate(page);			
				break;
			}
			default:
			{
				VOLOGI("Unknown segment Type!");
				break;
			}
		}
		return nSegmentSize;
	}
	return -1;
}

VO_U32 DVBParser::yuv2rgb(int Y, int Cb, int Cr)
{
  int Ey, Epb, Epr;
  int Eg, Eb, Er;

  Ey = (Y - 16);
  Epb = (Cb - 128);
  Epr = (Cr - 128);
  /* ITU-R 709 */
  Er = MAX(MIN(((298 * Ey             + 460 * Epr) / 256), 255), 0);
  Eg = MAX(MIN(((298 * Ey -  55 * Epb - 137 * Epr) / 256), 255), 0);
  Eb = MAX(MIN(((298 * Ey + 543 * Epb            ) / 256), 255), 0);

  return (Er << 16) | (Eg << 8) | Eb;
//  return (Er << 24) | (Eg << 16) | Eb << 8;
}

VO_VOID DVBParser::PageTranslate(DVBPage *page)
{
	if(!page)
		return;
	m_stSubtitleInfo.nTimeStamp = page->GetPts();
//	m_stNullSubtitleInfo.nTimeStamp = page->GetPts();	//update timestamp to null struct ---> move into ParseData
	m_stSubtitleInfo.pSubtitleEntry ->nDuration = page->GetTimeout() * 1000 ;
	VO_U32 regionNum = page->regions.Count();
	if(regionNum == 0)
		return;
	pvoSubtitleImageInfo TmpSubtitleImageInfo = NULL;
	pvoSubtitleImageInfo SubtitleImageInfo = NULL;
	SubtitleImageInfo = m_stSubtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo;
	
	while(SubtitleImageInfo)
	{
		TmpSubtitleImageInfo = SubtitleImageInfo->pNext;
		DelSubTitleEntry(SubtitleImageInfo, Struct_Type_voSubtitleImageInfo);
		SubtitleImageInfo = TmpSubtitleImageInfo;
	}
	pvoSubtitleImageInfo pTmpListTail = SubtitleImageInfo;
	for(VO_U32 i = 0; i < regionNum; i ++)
	{
		TmpSubtitleImageInfo = (pvoSubtitleImageInfo)NewSubTitleEntry(Struct_Type_voSubtitleImageInfo);
		InitializeStruct(TmpSubtitleImageInfo, Struct_Type_voSubtitleImageInfo);
		
		if(!pTmpListTail)
		{
			pTmpListTail = m_stSubtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo = TmpSubtitleImageInfo;
		}
		else
		{
			while(pTmpListTail)
			{
				if(!pTmpListTail->pNext)
				{
					SubtitleImageInfo = pTmpListTail;
				}
				pTmpListTail = pTmpListTail->pNext;
				
			}
			SubtitleImageInfo->pNext = TmpSubtitleImageInfo;
		}
	}
	for(DVBRegion *dr = page->regions.First(); dr; dr = page->regions.Next(dr))	
	{
		TmpSubtitleImageInfo = m_stSubtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo;
		while(true)
		{
			if(!TmpSubtitleImageInfo->stImageData.pPicData)
				break;
			else
			{
				TmpSubtitleImageInfo = m_stSubtitleInfo.pSubtitleEntry->stSubtitleDispInfo.pImageInfo->pNext;
			}
		}
		if(displayWidth == 0 && displayHeight == 0)
		{
			displayWidth = 720;
			displayHeight = 576;
		}

		TmpSubtitleImageInfo->stImageDesp.stImageRectInfo.stRect.nTop = dr->GetVerticalAddress() * 100 / displayHeight;
		TmpSubtitleImageInfo->stImageDesp.stImageRectInfo.stRect.nLeft = dr->GetHorizontalAddress() * 100 / displayWidth;
		TmpSubtitleImageInfo->stImageDesp.stImageRectInfo.stRect.nRight = (dr->GetHorizontalAddress() + dr->GetWidth()) * 100 / displayWidth;
		TmpSubtitleImageInfo->stImageDesp.stImageRectInfo.stRect.nBottom = (dr->GetVerticalAddress() + dr->GetHeight()) * 100 / displayHeight;
		
/*
		TmpSubtitleImageInfo->stImageDesp.stImageRectInfo.stRect.nTop = dr->GetVerticalAddress();
		TmpSubtitleImageInfo->stImageDesp.stImageRectInfo.stRect.nLeft = dr->GetHorizontalAddress();
		TmpSubtitleImageInfo->stImageDesp.stImageRectInfo.stRect.nRight = dr->GetHorizontalAddress() + dr->GetWidth();
		TmpSubtitleImageInfo->stImageDesp.stImageRectInfo.stRect.nBottom = dr->GetVerticalAddress() + dr->GetHeight();
*/
		dr->indexTrans2Raw();
		TmpSubtitleImageInfo->stImageData.nType = VO_IMAGE_RGBA32;
		TmpSubtitleImageInfo->stImageData.nWidth = dr->GetWidth();
		TmpSubtitleImageInfo->stImageData.nHeight = dr->GetHeight();
		TmpSubtitleImageInfo->stImageData.nSize = dr->GetRawSize();
		TmpSubtitleImageInfo->stImageData.pPicData = dr->GetRawData();
		
#if 0
		//FILE* fp = fopen("C:\\Users\\zhang_xin\\Desktop\\dvbRaw\\1.dump","wb");
		FILE* fp = fopen("/sdcard/dump.dat","wb");
		if(fp)
		{
			fwrite(TmpSubtitleImageInfo->stImageData.pPicData,1,TmpSubtitleImageInfo->stImageData.nSize,fp);
			fclose(fp);
		}
#endif
		dr->objects.Clear();
	}
	m_bNeedSend = VO_TRUE;
}


DVBBufferCache::DVBBufferCache()
{
	pDVBData = NULL;
	nDVBSize = 0;
	Reset();
}

DVBBufferCache::~DVBBufferCache()
{
//	free(pDVBData);
}

VO_VOID		DVBBufferCache::Reset()
{
	nDVBLength = 0;
	nDVBPos = 0;
}

VO_BOOL		DVBBufferCache::Realloc(VO_U32 nSize)
{
	if(nSize > nDVBSize)
	{
		pDVBData = (VO_PBYTE)realloc(pDVBData,nDVBSize);
		if(pDVBData == NULL)
		{
			VOLOGE("Realloc DVB BufferCache Failed!")
			nDVBLength = 0;
			nDVBSize = 0;
			return VO_FALSE;
		}
	}
	return VO_TRUE;
}

VO_PBYTE	DVBBufferCache::Get(VO_U32 &nLength)
{
	if(nDVBLength > nDVBPos + 5)
	{	
		nLength = (pDVBData[nDVBPos + 4] << 8) + pDVBData[nDVBPos + 5] + 6;
		if(nDVBLength >= nDVBPos + nLength)
		{
			VO_PBYTE result = pDVBData + nDVBPos;
			nDVBPos += nLength;
			return result;
		}
	}
	return NULL;
}

VO_VOID		DVBBufferCache::Put(const VO_PBYTE pData, VO_U32 nLength)
{
/*
	if(nLength && Realloc(nDVBLength + nLength))
	{
		memcpy(pDVBData + nDVBLength, pData, nLength);
		nDVBLength += nLength;
	}
	*/
	pDVBData = pData;
	nDVBLength = nLength;
}

/** DVBObject **/
DVBObject::DVBObject(VO_U32	ObjectID, DVBImage* Image)
:objectVersion(-1)
,codingMethod(-1)
,nonModifyingColorFlag(VO_FALSE)
,fgColor(0)
,bgColor(0)
,providerFlag(0)
,px(0)
,py(0)
,bitPos(0)
{
	objectID = ObjectID;
	image = Image;
}


VO_U8	DVBObject::Get2Bits(VO_PBYTE pData, VO_U32 &nIndex)
{
	if (bitPos == 0) 
	{
		nIndex++;
	     	bitPos = 8;
	}
	VO_U8 result = pData[nIndex];
	
	bitPos -= 2;
	return (result >> bitPos) & 0x03;
}

VO_U8 DVBObject::Get4Bits(VO_PBYTE pData, VO_U32 &nIndex)
{
	if(bitPos == 0)
	{
		nIndex++;
		bitPos = 8;
	}
	VO_U8 result = pData[nIndex];
	bitPos -= 4;
	return (result >> bitPos) & 0x0F;
}

VO_BOOL	 DVBObject::Decode2Bpp(VO_PBYTE pData, VO_U32 &nIndex, VO_U32 &x, VO_U32 y)
{
	VO_U32	nLength = 0;
	VO_U8	nColor = 0;
	VO_U8	nCode = 0;
	nCode = Get2Bits(pData, nIndex);
	if(nCode)
	{
		nColor = nCode;
		nLength = 1;
	}
	else
	{
		nCode = Get2Bits(pData,nIndex);
		if(nCode & 2)
		{
			nLength = ((nCode & 1) << 2) + Get2Bits(pData,nIndex) + 3;
			nColor = Get2Bits(pData, nIndex);
		}
		else if(nCode & 1)
		{
			nLength = 1;
		}
		else
		{
			nCode = Get2Bits(pData,nIndex);
			switch(nCode & 3)
			{
				case 0:
					return VO_FALSE;
				case 1:
					nLength = 2;
					break;
				case 2:
					nLength = (Get2Bits(pData,nIndex) << 2) + Get2Bits(pData, nIndex) + 12;
					nColor = Get2Bits(pData,nIndex);
					break;
				case 3:
					nLength =  (Get2Bits(pData, nIndex) << 6) + (Get2Bits(pData, nIndex) << 4) + (Get2Bits(pData, nIndex) << 2) + Get2Bits(pData, nIndex) + 29;
					nColor = Get2Bits(pData, nIndex);
					break;
			}
		}
	}
	if(map_table == DVB_MAPTABLE_2TO4)
	{
		nColor = MAPTABLE2TO4[nColor];
	}
	else if(map_table == DVB_MAPTABLE_2TO8)
	{
		nColor = MAPTABLE2TO8[nColor];
	}
	DrawLine(x,y,nColor,nLength);
	x += nLength;
	return VO_TRUE;
}

VO_BOOL	DVBObject::Decode4Bpp(VO_PBYTE pData, VO_U32 &nIndex, VO_U32 &x, VO_U32 y)
{
	VO_U32	nLength = 0;
	VO_U8	nColor = 0;
	VO_U8	nCode = 0;
	nCode = Get4Bits(pData,nIndex);
	if(nCode)
	{
		nColor = nCode;
		nLength = 1;
	}
	else
	{
		nCode = Get4Bits(pData,nIndex);
		if(nCode & 8)
		{
			if(nCode & 4)
			{
				switch(nCode & 3)
				{
					case 0:
						nLength = 1;
						break;
					case 1:
						nLength = 2;
						break;
					case 2:
						nLength = Get4Bits(pData, nIndex) + 9;
						nColor = Get4Bits(pData, nIndex);
						break;
					case 3:
						nLength = (Get4Bits(pData, nIndex) << 4) + Get4Bits(pData, nIndex) + 25;
                  				nColor = Get4Bits(pData, nIndex);
           				      break;	
				}
			}
			else
			{
				nLength = (nCode & 3) + 4;
				nColor = Get4Bits(pData, nIndex);
			}
		}
		else
		{
			if(nCode == 0)
				return VO_FALSE;
			nLength = nCode + 2;
		}
	}
	if(map_table == DVB_MAPTABLE_4TO8)
	{
		nColor = MAPTABLE4TO8[nColor];
	}
	DrawLine(x, y, nColor, nLength);
	x += nLength;
	return VO_TRUE;
}
VO_BOOL	DVBObject::Decode8Bpp(VO_PBYTE pData, VO_U32 &nIndex, VO_U32 &x, VO_U32 y)
{
	VO_U32 nLength = 0;
	VO_U8 nColor = 0;
	VO_U8 nCode = pData[nIndex++];
	if(nCode)
	{
		nColor = nCode;
		nLength = 1;
	}
	else
	{
		nCode = pData[nIndex++];
		if(((nCode >> 7) & 0x1) == 0)
		{
			if((nCode & 0x7F) != 0)
			{
				nColor = 0x00;
				nLength = nCode & 0x7F;
			}
			else
				return VO_FALSE;
		}
		else
		{
			nColor = pData[nIndex++];
			nLength = nCode & 0x7F;
		}
	}
	DrawLine(x, y, nColor, nLength);
	x += nLength;
	return VO_TRUE;
}

VO_VOID	DVBObject::DecodeSubBlock(VO_PBYTE pData, VO_U32 nSize, VO_BOOL bEven)
{
	VO_U32	x = 0;
	VO_U32	y =  bEven ? 0 : 1;
	VO_U8 nCode = 0;
	VO_BOOL nReturn = VO_FALSE;
	map_table = DVB_MAPTABLE_UNUSED;
	for(VO_U32 index = 0;index < nSize;)
	{
		nCode = pData[index++];
		switch(nCode)
		{
			case 0x10:
			{
				bitPos = 8;
				while(VO_TRUE)
				{
					nReturn = Decode2Bpp(pData, index, x, y);
					if(index >= nSize || !nReturn)
						break;
				}
				if(bitPos == 0)
					index++;
				break;
			}
			case 0x11:
			{
				bitPos = 8;
				while(VO_TRUE)
				{
					if(index >= nSize || !Decode4Bpp(pData, index, x, y))
						break;
				}
				if(bitPos == 0)
					index++;
				break;
			}
			case 0x12:
			{
				while(VO_TRUE)
				{
					if(index >= nSize || !Decode8Bpp(pData, index, x, y))
						break;
				}
				break;
			}
			case 0x20:
				//TODO
			//	map_table = DVB_MAPTABLE_2TO4;
				index += 2;
				break;
			case 0x21:
				//TODO
			//	map_table = DVB_MAPTABLE_2TO8;
				index += 4;
				break;
			case 0x22:
				//TODO
			//	map_table = DVB_MAPTABLE_4TO8;
				index += 16;
				break;
			case 0xF0:
				x = 0;
				y += 2;
				break;
		}
	}
}

VO_VOID DVBObject::DrawLine(VO_U32 x, VO_U32 y, VO_U8 nIndex, VO_U32 nLength)
{
	/*this is conflict with spec(I think),when playing http://qatest.visualon.com:8082/hls/Customers/Ericsson/multi_audio/index1.m3u8*/
	/*remove as to show the subtitle*/
/*
	if (nonModifyingColorFlag && nIndex == 1)
   		  return;
 */	
 	for (VO_U32 pos = x + px; pos < x + px + nLength; pos++)
     		 image->SetIndex(pos, y + py, nIndex);
}


/***DVBImage****/
DVBImage::DVBImage(VO_U32 nWidth, VO_U32 nHeight, VO_U32 nBpp, VO_U32 X, VO_U32 Y)
:DVBPalette(nBpp)
{
	indexImageBuffer = NULL;
	rawImageBuffer = NULL;
	x = X;
	y = Y;
	//SetSize(nWidth,nHeight);
}

DVBImage:: ~DVBImage()
{
	if(indexImageBuffer)
	{
		free(indexImageBuffer);
		indexImageBuffer = NULL;
	}
	if(rawImageBuffer)
	{
		free(rawImageBuffer);
		rawImageBuffer = NULL;		
	}
}

VO_VOID	DVBImage::SetIndex(VO_U32 x, VO_U32 y, VO_U32 nIndex)
{
	if (indexImageBuffer) 
	{
		if (0 <= x && x < width && 0 <= y && y < height)
		{
			 if (indexImageBuffer[width * y + x] != (VO_U8)nIndex)
		 	{
		 		 indexImageBuffer[width * y + x] = (VO_U8)nIndex;
		 	}
		}
		else
		{
			VOLOGE("wrong x and y!(%d,%d) width is %d,height is %d",x,y,width,height);
		}
	}
}
VO_VOID DVBImage::SetSize(VO_U32 nWidth, VO_U32 nHeight)
{
	if (indexImageBuffer && nWidth == width && nHeight == height)
 	 	return;
  	width = nWidth;
  	height = nHeight;
	if(indexImageBuffer)
	{
 		free(indexImageBuffer);
 		indexImageBuffer = NULL;
	}
  	
  	if (width > 0 && height > 0) 
	{
     		indexImageBuffer = (VO_PBYTE)malloc(width * height);
    		if (indexImageBuffer)
      			memset(indexImageBuffer, 0x00, width * height);
  		else
       	 	VOLOGE("alloc image index buffer failed!");
     	}
  	else
     		VOLOGE("invalid width and height");
}

VO_VOID DVBImage::indexTrans2Raw()
{
	VO_U32 color = 0;
	if(rawImageBuffer)
	{
		free(rawImageBuffer);
 		rawImageBuffer = NULL;
	}
	rawBufferSize = width * height *4;
	rawImageBuffer = (VO_PBYTE)malloc(rawBufferSize);
	if (rawImageBuffer)
		memset(rawImageBuffer, 0x00, rawBufferSize);
	else
	{
	 	VOLOGE("alloc image index buffer failed!");
		return;
	}
#if 0
		//FILE* fp = fopen("C:\\Users\\zhang_xin\\Desktop\\dvbRaw\\bit.dump","wb");
		FILE* fp = fopen("/sdcard/bitdump.dat","wb");
		if(fp)
		{
			fwrite(indexImageBuffer,1,width * height,fp);
			fclose(fp);
		}
#endif
	
	for(VO_U32 i = 0; i < width * height; i++)
	{

		color = GetColor(indexImageBuffer[i]);
		/*switch(indexImageBuffer[i])
		{
		case 0: color = 0x00000000;break;
		case 1: color = 0xff565656;break;
		case 2: color = 0xff111111;break;
		case 3: color = 0xffbdbdbd;break;
		}
		*/
		memcpy(rawImageBuffer + i * 4,&color, 4);
	}
}

/***DVBPalette***/
DVBPalette::DVBPalette(VO_U32 nBpp)
{
	memset(color,0x00,MAXCOLORS);
	bpp = nBpp;
	switch(bpp)
	{
		//fill default color look up table
		case 2:
			memcpy(color,CLUT_2BIT,4 * 4);
			numColors = 4;	
			break;
		case 4: 
			memcpy(color,CLUT_4BIT,16 * 4);
			numColors = 16; 
			break;
		case 8: 
			memcpy(color,CLUT_8BIT,256 * 4);
			numColors = 256; 
			break;
		default: 
			memcpy(color,CLUT_8BIT,256 * 4);
			numColors = 256; 
			break;
	}
}

VO_VOID DVBPalette::Replace(DVBPalette &Palette)
{
	for(VO_U32 i = 0; i < Palette.numColors; i++)
		SetColor(i, Palette.color[i]);
	numColors = Palette.numColors;
}

VO_VOID	DVBPalette::SetBpp(VO_U32 nBpp)
{
	bpp = nBpp;
  	numColors = 1 << bpp;
}

/***DVBClut***/
DVBClut::DVBClut(VO_U32 ClutID)
:palette2(2)
,palette4(4)
,palette8(8)
{
	clutID = ClutID;
	version = -1;
}

VO_VOID DVBClut::SetColor(VO_U32 nBpp, VO_U32 nIndex, VO_U32 nColor)
{
	switch(nBpp)
	{
		case 2:palette2.SetColor(nIndex, nColor);break;
		case 4:palette4.SetColor(nIndex, nColor);break;
		case 8:palette8.SetColor(nIndex, nColor);break;
		default: break;
	}
}
DVBPalette*	DVBClut::GetPalette(VO_U32 nBpp)
{
	switch(nBpp)
	{
		case 2: return &palette2;
		case 4: return &palette4;
		case 8: return &palette8;
		default: break;
	}
	return &palette8;
}



/***DVBRegion***/
DVBRegion::DVBRegion(VO_U32 RegionID)
:DVBImage(0,0,2)
{
	regionID = RegionID;
	regionVersion = -1;
	clutID = -1;
	horizontalAddress = 0;
	verticalAddress = 0;
	level = 0;
}

VO_VOID	DVBRegion::FillRegion(VO_U32 nIndex)
{
	for(VO_U32 y = 0;y < GetHeight(); y++)
		for(VO_U32 x = 0; x < GetWidth(); x++)
			SetIndex(x, y, nIndex);
}

DVBObject* DVBRegion::GetObjectByID(VO_U32 ObjectID, VO_BOOL New)
{
	DVBObject *result = NULL;
	for(DVBObject *dvbo = objects.First(); dvbo; dvbo = objects.Next(dvbo))
	{
		if(dvbo->GetObjectID() == ObjectID)
			result = dvbo;
	}
	if(result == NULL && New)
	{
		result = new DVBObject(ObjectID, this);
		if(!result)
			return NULL;
		objects.Add(result);
	}
	return result;
}

/***DVBPage***/
DVBPage::DVBPage(VO_U32 PageID)
{
	pageID = PageID;
	pageVersion = -1;
	pageState = -1;
	pts = 0;
	timeout = 0;
}

DVBPage::~DVBPage()
{
	regions.Clear();
	cluts.Clear();
}

DVBClut*		DVBPage::GetClutByID(VO_U32 ClutID, VO_BOOL New)
{
	DVBClut *result = NULL;
	for(DVBClut *dc = cluts.First(); dc; dc = cluts.Next(dc))
	{
		if(dc->GetClutID() == ClutID)
			result = dc;
	}
	if(result == NULL && New)
	{
		result = new DVBClut(ClutID);
		if(!result)
			return NULL;
		cluts.Add(result);
	}
	return result;
}
DVBObject*	DVBPage::GetObjectByID(VO_U32 ObjectID)
{
	DVBObject *result = NULL;
	for(DVBRegion *dvbo = regions.First(); dvbo && !result; dvbo = regions.Next(dvbo))
		result = dvbo->GetObjectByID(ObjectID);
	return result;
}
DVBRegion*	DVBPage::GetRegionByID(VO_U32 RegionID, VO_BOOL New)
{
	DVBRegion *result = NULL;
	for(DVBRegion *dr = regions.First(); dr; dr = regions.Next(dr))
	{
		if(dr->GetRegionID() == RegionID)
			result = dr;
	}
	if(result == NULL && New)
	{
		result = new DVBRegion(RegionID);
		if(!result)
			return NULL;
		regions.Add(result);
	}
	return result;
}

VO_VOID		DVBPage::SetPageState(VO_U32 nPageState)
{
	pageState = nPageState;
	switch(pageState)
	{
		case 0:	//page update
			break;
		case 1:	//page refresh
			regions.Clear();
			break;
		case 2:	//new page
			regions.Clear();
			cluts.Clear();
			break;
		case 3:
			break;
	}
}
VO_VOID		DVBPage::UpdateRegionPalette(DVBClut* Clut)
{
	for(DVBRegion *dr = regions.First(); dr; dr = regions.Next(dr))
	{
		if(dr->GetClutID() == Clut->GetClutID())
			dr->Replace(*Clut->GetPalette(dr->GetDepth()));
	}
}