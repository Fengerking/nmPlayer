//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) 1996 -- 1998  Microsoft Corporation

Module Name:

	repeatpad_wmv.h

Abstract:


Author:


Revision History:

*************************************************************************/

/////////// main external API's /////////////////
#ifndef _WMV9AP_
#define RepeatRef0YArgs 	U8_WMV* ppxlcRef0Y,     \
                        	CoordI  iStart,         \
                            CoordI  iEnd,           \
                        	I32_WMV     iOldLeftOffet,  \
                        	Bool_WMV    fTop,           \
                            Bool_WMV    fBottom,        \
                            I32_WMV     iWidthY,        \
                            I32_WMV     iWidthYPlusExp, \
                            I32_WMV     iWidthPrevY


#define RepeatRef0UVArgs	U8_WMV* ppxlcRef0U,     \
                        	U8_WMV* ppxlcRef0V,     \
   	                        CoordI  iStart,         \
                            CoordI  iEnd,           \
	                        I32_WMV     iOldLeftOffet,  \
	                        Bool_WMV    fTop,           \
                            Bool_WMV    fBottom,        \
                            I32_WMV     iWidthUV,       \
                            I32_WMV     iWidthUVPlusExp,\
                            I32_WMV     iWidthPrevUV
#else
#define RepeatRef0YArgs 	U8_WMV* ppxlcRef0Y,     \
                        	CoordI  iStart,         \
                            CoordI  iEnd,           \
                        	I32_WMV     iOldLeftOffet,  \
                        	Bool_WMV    fTop,           \
                            Bool_WMV    fBottom,        \
                            I32_WMV     iWidthY,        \
                            I32_WMV     iWidthYPlusExp, \
                            I32_WMV     iWidthPrevY,  \
                            Bool_WMV bProgressive


#define RepeatRef0UVArgs	U8_WMV* ppxlcRef0U,     \
                        	U8_WMV* ppxlcRef0V,     \
   	                        CoordI  iStart,         \
                            CoordI  iEnd,           \
	                        I32_WMV     iOldLeftOffet,  \
	                        Bool_WMV    fTop,           \
                            Bool_WMV    fBottom,        \
                            I32_WMV     iWidthUV,       \
                            I32_WMV     iWidthUVPlusExp,\
                            I32_WMV     iWidthPrevUV, \
                             Bool_WMV bProgressive


#endif

extern Void_WMV (*g_pRepeatRef0UV) (RepeatRef0UVArgs);
extern Void_WMV (*g_pRepeatRef0Y) (RepeatRef0YArgs);

///////////////////////////////////////////////////////

extern  Void_WMV g_RepeatRef0Y     (RepeatRef0YArgs);
extern  Void_WMV g_RepeatRef0UV     (RepeatRef0UVArgs);
extern  Void_WMV g_RepeatRef0Y_32   (RepeatRef0YArgs);
extern  Void_WMV g_RepeatRef0UV_16  (RepeatRef0UVArgs);

extern  Void_WMV g_RepeatRef0Y_24   (RepeatRef0YArgs);
extern  Void_WMV g_RepeatRef0UV_12  (RepeatRef0UVArgs);

#if defined(VOARMV6)
extern Void_WMV	ARMV6_g_memcpy(U8_WMV* ppxliLeftTop, U8_WMV* ppxliSrc, int iWidthPrevY, int iTrueWidth,  int iVertPad);
extern Void_WMV ARMV6_g_RepeatRef0Y_LeftRight ( const U8_WMV* pLeft,	const U8_WMV* pRight, U8_WMV* pDst,I32_WMV iWidthPrevY, I32_WMV iWidthYPlusExp, I32_WMV iRowNum);
extern Void_WMV ARMV6_g_RepeatRef0UV_LeftRight ( const U8_WMV* pLeftU, const U8_WMV* pRightU, const U8_WMV* pLeftV, const U8_WMV* pRightV,U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidthPrevUV,	I32_WMV iWidthUVPlusExp, I32_WMV iRowNum);
#define g_memcpy_Fun					ARMV6_g_memcpy
#define g_RepeatRef0Y_LeftRight_Fun		ARMV6_g_RepeatRef0Y_LeftRight
#define g_RepeatRef0UV_LeftRight_Fun	ARMV6_g_RepeatRef0UV_LeftRight
#else
extern Void_WMV	g_memcpy(U8_WMV* ppxliLeftTop, U8_WMV* ppxliSrc, int iWidthPrevY, int iTrueWidth,  int iVertPad);
extern Void_WMV g_RepeatRef0Y_LeftRight ( const U8_WMV* pLeft,	const U8_WMV* pRight, U8_WMV* pDst,I32_WMV iWidthPrevY, I32_WMV iWidthYPlusExp, I32_WMV iRowNum);
extern Void_WMV g_RepeatRef0UV_LeftRight ( const U8_WMV* pLeftU, const U8_WMV* pRightU, const U8_WMV* pLeftV, const U8_WMV* pRightV,U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidthPrevUV,	I32_WMV iWidthUVPlusExp, I32_WMV iRowNum);
#define g_memcpy_Fun					g_memcpy
#define g_RepeatRef0Y_LeftRight_Fun		g_RepeatRef0Y_LeftRight
#define g_RepeatRef0UV_LeftRight_Fun	g_RepeatRef0UV_LeftRight
#endif

#ifdef _WMV_TARGET_X86_
Void_WMV g_RepeatRef0Y_MMX (RepeatRef0YArgs);
Void_WMV g_RepeatRef0UV_MMX (RepeatRef0UVArgs);
#endif

#ifdef _WMV9AP_
extern  Void_WMV g_RepeatRef0Y_AP  (RepeatRef0YArgs);
extern  Void_WMV g_RepeatRef0UV_AP     (RepeatRef0UVArgs);
#ifndef WMV9_SIMPLE_ONLY
Void_WMV g_InitRepeatRefInfo (tWMVDecInternalMember *pWMVDec,Bool_WMV bAdvancedProfile);
#endif
Void_WMV g_RepeatRef0Y_AP  (RepeatRef0YArgs);
Void_WMV g_RepeatRef0UV_AP (RepeatRef0UVArgs);
#    ifdef _WMV_TARGET_X86_
extern  Void_WMV g_RepeatRef0Y_AP_MMX     (RepeatRef0YArgs);
extern  Void_WMV g_RepeatRef0UV_AP_MMX    (RepeatRef0UVArgs);
#    endif
#endif
