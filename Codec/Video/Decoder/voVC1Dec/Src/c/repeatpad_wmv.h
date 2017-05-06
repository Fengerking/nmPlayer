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

#define RepeatRef0YArgs 	U8_WMV* ppxlcRef0Y,     \
                        	I32_WMV  iStart,         \
                            I32_WMV  iEnd,           \
                        	I32_WMV     iOldLeftOffet,  \
                        	Bool_WMV    fTop,           \
                            Bool_WMV    fBottom,        \
                            I32_WMV     iWidthY,        \
                            I32_WMV     iWidthYPlusExp, \
                            I32_WMV     iWidthPrevY,  \
                            Bool_WMV bProgressive


#define RepeatRef0UVArgs	U8_WMV* ppxlcRef0U,     \
                        	U8_WMV* ppxlcRef0V,     \
   	                        I32_WMV  iStart,         \
                            I32_WMV  iEnd,           \
	                        I32_WMV     iOldLeftOffet,  \
	                        Bool_WMV    fTop,           \
                            Bool_WMV    fBottom,        \
                            I32_WMV     iWidthUV,       \
                            I32_WMV     iWidthUVPlusExp,\
                            I32_WMV     iWidthPrevUV, \
                             Bool_WMV bProgressive


///////////////////////////////////////////////////////

extern  Void_WMV g_RepeatRef0Y     (RepeatRef0YArgs);
extern  Void_WMV g_RepeatRef0UV     (RepeatRef0UVArgs);

#if defined(VOARMV6) || defined(VOARMV7)
extern Void_WMV	ARMV6_g_memcpy(U8_WMV* ppxliLeftTop, U8_WMV* ppxliSrc, int iWidthPrevY, int iTrueWidth,  int iVertPad);
extern Void_WMV ARMV6_g_RepeatRef0Y_LeftRight ( const U8_WMV* pLeft,	const U8_WMV* pRight, U8_WMV* pDst,I32_WMV iWidthPrevY, I32_WMV iWidthYPlusExp, I32_WMV iRowNum);
extern Void_WMV ARMV6_g_RepeatRef0UV_LeftRight ( const U8_WMV* pLeftU, const U8_WMV* pRightU, const U8_WMV* pLeftV, const U8_WMV* pRightV,U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidthPrevUV,	I32_WMV iWidthUVPlusExp, I32_WMV iRowNum);
#define g_memcpy_Fun					ARMV6_g_memcpy
#define g_RepeatRef0Y_LeftRight_Fun		ARMV6_g_RepeatRef0Y_LeftRight
#define g_RepeatRef0UV_LeftRight_Fun	ARMV6_g_RepeatRef0UV_LeftRight
#elif VOARMV4
extern Void_WMV	g_memcpy_ARMV4(U8_WMV* ppxliLeftTop, U8_WMV* ppxliSrc, int iWidthPrevY, int iTrueWidth,  int iVertPad);
extern Void_WMV g_RepeatRef0Y_LeftRight_ARMV4 ( const U8_WMV* pLeft,	const U8_WMV* pRight, U8_WMV* pDst,I32_WMV iWidthPrevY, I32_WMV iWidthYPlusExp, I32_WMV iRowNum);
extern Void_WMV g_RepeatRef0UV_LeftRight_ARMV4 ( const U8_WMV* pLeftU, const U8_WMV* pRightU, const U8_WMV* pLeftV, const U8_WMV* pRightV,U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidthPrevUV,	I32_WMV iWidthUVPlusExp, I32_WMV iRowNum);
#define g_memcpy_Fun					g_memcpy_ARMV4
#define g_RepeatRef0Y_LeftRight_Fun		g_RepeatRef0Y_LeftRight_ARMV4
#define g_RepeatRef0UV_LeftRight_Fun	g_RepeatRef0UV_LeftRight_ARMV4
#else
extern Void_WMV	g_memcpy_C(U8_WMV* ppxliLeftTop, U8_WMV* ppxliSrc, int iWidthPrevY, int iTrueWidth,  int iVertPad);
extern Void_WMV g_RepeatRef0Y_LeftRight_C ( const U8_WMV* pLeft,	const U8_WMV* pRight, U8_WMV* pDst,I32_WMV iWidthPrevY, I32_WMV iWidthYPlusExp, I32_WMV iRowNum);
extern Void_WMV g_RepeatRef0UV_LeftRight_C ( const U8_WMV* pLeftU, const U8_WMV* pRightU, const U8_WMV* pLeftV, const U8_WMV* pRightV,U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidthPrevUV,	I32_WMV iWidthUVPlusExp, I32_WMV iRowNum);
#define g_memcpy_Fun					g_memcpy_C
#define g_RepeatRef0Y_LeftRight_Fun		g_RepeatRef0Y_LeftRight_C
#define g_RepeatRef0UV_LeftRight_Fun	g_RepeatRef0UV_LeftRight_C
#endif

extern  Void_WMV g_RepeatRef0Y_AP  (RepeatRef0YArgs);
extern  Void_WMV g_RepeatRef0UV_AP     (RepeatRef0UVArgs);

Void_WMV g_InitRepeatRefInfo (tWMVDecInternalMember *pWMVDec,Bool_WMV bAdvancedProfile);

Void_WMV g_RepeatRef0Y_AP  (RepeatRef0YArgs);
Void_WMV g_RepeatRef0UV_AP (RepeatRef0UVArgs);
