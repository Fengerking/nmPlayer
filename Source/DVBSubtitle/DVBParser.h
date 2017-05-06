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
* @file DVBParser.h
*    
*
*
* @author  Ferry Zhang
* Change History
* 2012-12-24  Create File
************************************************************************/

#ifndef _DVBPARSER_H_
#define _DVBPARSER_H_

#include "voDVBSubtitleParser.h"
#include "tools.h"
#include "CSubtitlePacker.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE
#endif

#define MAXCOLORS 256
#define PAGE_SEGMENT   		0x10
#define REGION_SEGMENT		0x11
#define CLUT_SEGMENT    		0x12
#define OBJECT_SEGMENT        0x13
#define DISPLAY_SEGMENT	0x14
#define END_OF_DISPLAY_SET_SEGMENT 0x80

const VO_U32 CLUT_2BIT[4] = {0x00000000,0xffffffff,0xff000000,/*0xff7f7f7f*/0xffbdbdbd};

const VO_U32 CLUT_4BIT[16] = {0x00000000,0xffff0000,0xff00ff00,0xffffff00,
			                                0xff0000ff,0xffff00ff,0xff00ffff,0xffffffff,
			                                0xff000000,0xff7f0000,0xff007f00,0xff7f7f00,
			                                0xff00007f,0xff7f007f,0xff007f7f,0xff7f7f7f};

const VO_U32 CLUT_8BIT[256] = {0x00000000,0x3fff0000,0x3f00ff00,0x3fffff00,0x3f0000ff,0x3fff00ff,0x3f00ffff,0x3fffffff,
								0x7f000000,0x7f550000,0x7f005500,0x7f555500,0x7f000055,0x7f550055,0x7f005555,0x7f555555,
								0x7faa0000,0x7fff0000,0x7faa5500,0x7fff5500,0x7faa0055,0x7fff0055,0x7faa5555,0x7fff5555,
								0x7f00aa00,0x7f55aa00,0x7f00ff00,0x7f55ff00,0x7f00aa55,0x7f55aa55,0x7f00ff55,0x7f55ff55,
								0x7faaaa00,0x7fffaa00,0x7faaff00,0x7fffff00,0x7faaaa55,0x7fffaa55,0x7faaff55,0x7fffff55,
								0x7f0000aa,0x7f5500aa,0x7f0055aa,0x7f5555aa,0x7f0000ff,0x7f5500ff,0x7f0055ff,0x7f5555ff,
								0x7faa00aa,0x7fff00aa,0x7faa55aa,0x7fff55aa,0x7faa00ff,0x7fff00ff,0x7faa55ff,0x7fff55ff,
								0x7f00aaaa,0x7f55aaaa,0x7f00ffaa,0x7f55ffaa,0x7f00aaff,0x7f55aaff,0x7f00ffff,0x7f55ffff,
								0x7faaaaaa,0x7fffaaaa,0x7faaffaa,0x7fffffaa,0x7faaaaff,0x7fffaaff,0x7faaffff,0x7fffffff,
								0xff7f7f7f,0xff9f7f7f,0xff7f9f7f,0xff9f9f7f,0xff7f7f9f,0xff9f7f9f,0xff7f9f9f,0xff9f9f9f,
								0xff000000,0xff550000,0xff005500,0xff555500,0xff000055,0xff550055,0xff005555,0xff555555,
								0xffd47f7f,0xfff47f7f,0xffd49f7f,0xfff49f7f,0xffd47f9f,0xfff47f9f,0xffd49f9f,0xfff49f9f,
								0xffaa0000,0xffff0000,0xffaa5500,0xffff5500,0xffaa0055,0xffff0055,0xffaa5555,0xffff5555,
								0xff7fd47f,0xff9fd47f,0xff7ff47f,0xff9ff47f,0xff7fd49f,0xff9fd49f,0xff7ff49f,0xff9ff49f,
								0xff00aa00,0xff55aa00,0xff00ff00,0xff55ff00,0xff00aa55,0xff55aa55,0xff00ff55,0xff55ff55,
								0xffd4d47f,0xfff4d47f,0xffd4f47f,0xfff4f47f,0xffd4d49f,0xfff4d49f,0xffd4f49f,0xfff4f49f,
								0xffaaaa00,0xffffaa00,0xffaaff00,0xffffff00,0xffaaaa55,0xffffaa55,0xffaaff55,0xffffff55,
								0xff7f7fd4,0xff9f7fd4,0xff7f9fd4,0xff9f9fd4,0xff7f7ff4,0xff9f7ff4,0xff7f9ff4,0xff9f9ff4,
								0xff0000aa,0xff5500aa,0xff0055aa,0xff5555aa,0xff0000ff,0xff5500ff,0xff0055ff,0xff5555ff,
								0xffd47fd4,0xfff47fd4,0xffd49fd4,0xfff49fd4,0xffd47ff4,0xfff47ff4,0xffd49ff4,0xfff49ff4,
								0xffaa00aa,0xffff00aa,0xffaa55aa,0xffff55aa,0xffaa00ff,0xffff00ff,0xffaa55ff,0xffff55ff,
								0xff7fd4d4,0xff9fd4d4,0xff7ff4d4,0xff9ff4d4,0xff7fd4f4,0xff9fd4f4,0xff7ff4f4,0xff9ff4f4,
								0xff00aaaa,0xff55aaaa,0xff00ffaa,0xff55ffaa,0xff00aaff,0xff55aaff,0xff00ffff,0xff55ffff,
								0xffd4d4d4,0xfff4d4d4,0xffd4f4d4,0xfff4f4d4,0xffd4d4f4,0xfff4d4f4,0xffd4f4f4,0xfff4f4f4,
								0xffaaaaaa,0xffffaaaa,0xffaaffaa,0xffffffaa,0xffaaaaff,0xffffaaff,0xffaaffff,0xffffffff};

const VO_U8 MAPTABLE2TO4[16] = {0x0,0x7,0x8,0xf};

const VO_U8 MAPTABLE2TO8[4] = {0x0,0x77,0x88,0xff};

const VO_U8 MAPTABLE4TO8[16] = {0x0,0x11,0x22,0x33,
								0x44,0x55,0x66,0x77,
								0x88,0x99,0xaa,0xbb,
								0xcc,0xdd,0xee,0xff
};

typedef enum{
	DVB_MAPTABLE_UNUSED = 0,
	DVB_MAPTABLE_2TO4,
	DVB_MAPTABLE_2TO8,
	DVB_MAPTABLE_4TO8,
}MAPTABLE_TYPE;


class DVBBufferCache
{
public:
	DVBBufferCache();
	virtual		~DVBBufferCache();
	VO_VOID		Reset();
	VO_PBYTE	Get(VO_U32 &nLength);
	VO_VOID		Put(const VO_PBYTE pData, VO_U32 nLength);
private:
	VO_PBYTE 	pDVBData;
	VO_U32		nDVBLength;
	VO_U32		nDVBPos;
	VO_U32		nDVBSize;
	VO_BOOL		Realloc(VO_U32 nSize);
};






class DVBPalette
{
private:
	VO_U32	color[MAXCOLORS];
	VO_U32	bpp;
	VO_U32	numColors;
public:
	DVBPalette(VO_U32 nBpp);
	VO_VOID SetColor(VO_U32 nIndex, VO_U32 nColor) { color[nIndex] = nColor;}
	VO_U32	GetColor(VO_U32 nIndex){return color[nIndex];}
	VO_VOID Replace(DVBPalette &Palette);
	VO_VOID	SetBpp(VO_U32 nBpp);
	VO_U32 	GetBpp() { return bpp; }
};

class DVBImage : public DVBPalette
{
private:
	VO_PBYTE	indexImageBuffer;		//index data of image
	VO_PBYTE     rawImageBuffer;			//raw data of image
	VO_U32		x;
	VO_U32		y;
	VO_U32		width;
	VO_U32		height;
	VO_U32		rawBufferSize;
public:
	DVBImage(VO_U32 nWidth, VO_U32 nHeight, VO_U32 nBpp, VO_U32 X = 0, VO_U32 Y = 0);
	virtual ~DVBImage();
	VO_U32 GetX() {return x;}
	VO_U32	GetY(){return y;}
	VO_U32	GetWidth() {return width;}
	VO_U32	GetHeight() {return height;}
	VO_VOID	SetIndex(VO_U32 x, VO_U32 y, VO_U32 nIndex);
	VO_VOID SetSize(VO_U32 nWidth, VO_U32 nHeight);
	VO_PBYTE GetRawData(){return rawImageBuffer;}
	VO_U32	GetRawSize(){return rawBufferSize;}
	VO_VOID indexTrans2Raw();
};

class DVBClut : public voListObject
{
private:
	VO_U32	clutID;
	VO_U32	version;
	DVBPalette	palette2;
	DVBPalette	palette4;
	DVBPalette	palette8;
public:
	DVBClut(VO_U32 ClutID);
	VO_U32	GetClutID()	{ return clutID;}
	VO_U32	GetClutVersion()	{ return version;}
	VO_VOID	SetClutVersion(VO_U32 nVersion){version = nVersion;}
	VO_VOID SetColor(VO_U32 nBpp, VO_U32 nIndex, VO_U32 nColor);
	DVBPalette*	GetPalette(VO_U32 nBpp);
};

class DVBObject : public voListObject
{
private:
	VO_U32		objectID;
	VO_U32		objectVersion;
	VO_U32		codingMethod;
	VO_BOOL		nonModifyingColorFlag;
	VO_U8		fgColor;
	VO_U8		bgColor;
	VO_U32		providerFlag;
	VO_U32		px;
	VO_U32		py;
	VO_U32		bitPos;
	DVBImage*	image;
	MAPTABLE_TYPE	map_table;

	VO_U8		Get2Bits(VO_PBYTE pData, VO_U32 &nIndex);
	VO_U8		Get4Bits(VO_PBYTE pData, VO_U32 &nIndex);
	VO_BOOL		Decode2Bpp(VO_PBYTE pData, VO_U32 &nIndex, VO_U32 &x, VO_U32 y);
	VO_BOOL		Decode4Bpp(VO_PBYTE pData, VO_U32 &nIndex, VO_U32 &x, VO_U32 y);
	VO_BOOL		Decode8Bpp(VO_PBYTE pData, VO_U32 &nIndex, VO_U32 &x, VO_U32 y);
public:
	DVBObject(VO_U32 ObjectID, DVBImage* Image);
	VO_U32	GetObjectID() { return objectID;}
	VO_U32	GetObjectVersion() { return objectVersion;}
	VO_U32	GetCodingMethod() { return codingMethod;}
	VO_U32	GetNonModifyingColorFlag() { return nonModifyingColorFlag;}
	VO_VOID DecodeSubBlock(VO_PBYTE pData, VO_U32 nSize, VO_BOOL bEven);
	VO_VOID SetObjectVerison(VO_U32 nVersion) { objectVersion = nVersion;}
	VO_VOID SetBgColor(VO_U8 nBgColor) {bgColor = nBgColor;}
	VO_VOID SetFgColor(VO_U8 nFgColor){fgColor = nFgColor;}
	VO_VOID SetNonModifyingColorFlag(VO_BOOL bNonModifyingColorFlag){nonModifyingColorFlag = bNonModifyingColorFlag;}
	VO_VOID SetCodingMethod(VO_U32 nCodingMethod){codingMethod = nCodingMethod;}
	VO_VOID SetPosition(VO_U32 x, VO_U32 y){px = x;py = y;}
	VO_VOID SetProviderFlag(VO_U32 nProviderFlag){providerFlag = nProviderFlag;}
	VO_VOID DrawLine(VO_U32 x, VO_U32 y, VO_U8 nIndex, VO_U32 nLength);
	MAPTABLE_TYPE GetMapTableType(){return map_table;}
};

class DVBRegion : public voListObject, public DVBImage
{
private:
	VO_U32	regionID;
	VO_U32	regionVersion;
	VO_U32  	clutID;
	VO_U32	horizontalAddress;
	VO_U32	verticalAddress;
	VO_U32	level;
public:
	voList<DVBObject> objects;
public:
	DVBRegion(VO_U32 RegionID);
	VO_U32	GetRegionID(){ return regionID;}
	VO_U32	GetRegionVersion(){ return regionVersion;}
	VO_U32	GetClutID(){ return clutID;}
	VO_U32	GetLevel(){return level;}
	VO_U32	GetDepth(){return GetBpp();}
	VO_VOID	FillRegion(VO_U32 nIndex);
	DVBObject* GetObjectByID(VO_U32 ObjectID, VO_BOOL New = VO_FALSE);
	VO_U32	GetHorizontalAddress(){return horizontalAddress;}
	VO_U32	GetVerticalAddress(){return verticalAddress;}
	VO_VOID	SetRegionVersion(VO_U32 nVersion){regionVersion = nVersion;}
	VO_VOID SetClutID(VO_U32 ClutID){clutID = ClutID;}
	VO_VOID SetLevel(VO_U32 nLevel)
	{
		if(nLevel > 0 && nLevel < 4)
			level = 1 << nLevel;
	}
	VO_VOID SetDepth(VO_U32 nDepth)
	{
		if(nDepth > 0 && nDepth < 4)
			SetBpp(1 << nDepth);
	}
	
	VO_VOID SetHorizontalAddress(VO_U32 nHorizontalAddress){horizontalAddress = nHorizontalAddress;}
	VO_VOID SetVerticalAddress(VO_U32 nVerticalAddress){verticalAddress = nVerticalAddress;}	
};

class DVBPage : public voListObject
{
private:
	VO_U32	pageID;
	VO_U32	pageVersion;
	VO_U32	pageState;
	VO_U64	pts;
	VO_U32	timeout;
	voList<DVBClut> cluts;
public:
	DVBPage(VO_U32 PageID);
	virtual 		~DVBPage();
	VO_U32		GetPageID(){return pageID;}
	VO_U32		GetPageVerison(){return pageVersion;}
	VO_U32		GetPageState(){return pageState;}
	DVBClut*	GetClutByID(VO_U32 ClutID, VO_BOOL New = VO_FALSE);
	DVBObject*	GetObjectByID(VO_U32 ObjectID);
	DVBRegion*	GetRegionByID(VO_U32 RegionID, VO_BOOL New = VO_FALSE);
	VO_U64		GetPts(){return pts;}
	VO_U32		GetTimeout(){return timeout;}
	VO_VOID		SetPageVersion(VO_U32 nVersion){pageVersion = nVersion;}
	VO_VOID		SetPts(VO_U64 nPts){pts = nPts;}
	VO_VOID		SetPageState(VO_U32 nPageState);
	VO_VOID		SetTimeout(VO_U32 nTimeout){timeout = nTimeout;}
	VO_VOID		UpdateRegionPalette(DVBClut* Clut);
public:
	voList<DVBRegion> regions;
};

class DVBParser 
{
public:
	DVBParser();
	virtual ~DVBParser();
public:
	VO_U32			Open();
	VO_U32			Close();
	VO_U32			Process(VO_DVB_PARSER_INPUT_BUFFER* pBuffer);
	VO_U32			GetData(VO_DVB_PARSER_OUTPUT_BUFFER* pBuffer);
private:
	VO_U32 			ParseData(VO_PBYTE pData, VO_U32 nSize, VO_U64 pts);
	VO_U32 			yuv2rgb(int Y, int Cb, int Cr);
	VO_VOID			PageTranslate(DVBPage *page);
private:
	DVBBufferCache	*dvbBufferCache;
	voList<DVBPage> 	*pages;
	voSubtitleInfo			m_stSubtitleInfo;
	voSubtitleInfo		m_stNullSubtitleInfo;
	VO_BOOL			m_bNeedSend;
public:
	VO_U32			displayWidth;
	VO_U32			displayHeight;
	

};
#endif