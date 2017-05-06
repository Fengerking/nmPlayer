/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/


#ifndef __subtitleType_H__
#define __subtitleType_H__


/*******************************************************************************
Reference:	1, https://developer.apple.com/library/mac/#documentation/QuickTime/QT6_3/Chap1/QT6WhatsNew.html
					2, 3GPP TS 26.245 V9.0.0 (2009-12).doc
*******************************************************************************/
#include "voType.h"
#include "mpxtype.h"
#include "fCC2.h"

// #define LENGTH_ARRAY 20
 #define LENGTH_TEXT 2048
// #define LENGTH_NORMAL 255
struct Tx3gFontRecord   {
	uint16     nFontID;
	uint8       nNameLength;
	unsigned char	strName[255];
};
struct Tx3gFontTableRecord  {
	uint16      nEntryCount;
	Tx3gFontRecord      *pStFontRecord ;
};

struct Tx3gBoxRecord {
	int16 	nTop;
	int16	nLeft;
	int16 	nBottom;
	int16	nRight;
};

struct Tx3gRGBAColor    {
	uint8   nRed;
	uint8   nGreen;
	uint8   nBlue;
	uint8   nTransparency;
};

struct Tx3gStyleRecord  {
	uint16      nStartChar;
	uint16      nEndChar;
	uint16      nFontID;
	uint8       nFontFace;
	uint8       nFontSize;
	Tx3gRGBAColor       stFontColor;
};


//styl
struct Tx3gTextStyleBox{
	uint16	nEntryCount;
	Tx3gStyleRecord	*pStTextStyles;
};
//hlit
struct Tx3gTextHighlightBox{
	uint16	nStartCharoffset;
	uint16	nEndCharoffset; 
};
//hclr
struct Tx3gTextHilightColorBox {
	Tx3gRGBAColor 	stHighlightColor;
};

struct Tx3gKaraokeRecod{
	uint32	nHighlightEndtime;
	uint16	nStartCharoffset;
	uint16	nEndCharoffset; 
};
//krok
struct Tx3gTextKaraokeBox {
	uint32	nHighlightStarttime;
	uint16	nEntryCount;
	Tx3gKaraokeRecod  *pStKaraokeRecod;
};
//dlay
struct Tx3gTextScrollDelayBox{
	uint32	scrollDelay;
};
//href
struct Tx3gTextHyperTextBox {
	uint16	nStartCharoffset;
	uint16	nEndCharoffset;
	uint8	nURLLength;
	unsigned char	*pStrURL;
	uint8	nAltLength;
	unsigned char	*pStrAltString;
};
//tbox
struct Tx3gTextboxBox{
	Tx3gBoxRecord	stTextBox;
};
//blnk
struct Tx3gBlinkBox{
	uint16		nStartCharoffset;
	uint16		nEndCharoffset;
};
//twrp
struct Tx3gTextWrapBox {
	uint8	nWrapFlag;
};



struct Tx3gTextSampleModifierBox{
	uint32 nModifierType;
	void	   *pTxtModifier;
	Tx3gTextSampleModifierBox *pNext;
};

//text Description
struct Tx3gDescription  {
	uint32            nDisplayFlags;
	uint8                            nHorizontalJustification;
	uint8                            nVerticalJustification;
	Tx3gRGBAColor           stBackgroundColor;
	Tx3gBoxRecord			stDefaultTextBox;
	Tx3gStyleRecord			stDefaultStyle;
	Tx3gFontTableRecord	stFontTable;
};
//text Sample buffer
struct Tx3gTextSample {
	uint16					nTextLength;
	unsigned char		*pStrText;
	uint16					nEntryCount;
	Tx3gTextSampleModifierBox *pStTxtSampleModifierBox;
};

#endif // __subtitleType_H__
