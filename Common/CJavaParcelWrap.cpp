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

/************************************************************************
 * @file CJavaParcelWrap.cpp
 * Convert some C struct data to Java Parcel object.
 *
 * This is a wrap class of Java Parcel class
 *
 *
 * @author  Li Mingbo
 * @date    2012-2012 
 ************************************************************************/

#include "CJavaParcelWrap.h"
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

enum
{
	KEY_START_TIME 				= 1,
	KEY_DURATION 				= 2,
	KEY_STRUCT_RECT 			= 3,
	KEY_RECT_BORDER_TYPE 			= 4,
	KEY_RECT_BORDER_COLOR 			= 5,
	KEY_RECT_FILL_COLOR 			= 6,
	KEY_RECT_Z_ORDER	 		= 7,
	KEY_DISPLAY_EFFECTTYPE			= 8,
	KEY_DISPLAY_EFFECTDIRECTION		= 9,
	KEY_DISPLAY_EFFECTSPEED			= 10,
	KEY_TEXT_SIZE				= 11,
	KEY_TEXT_STRING				= 12,
	KEY_FONT_INFO_SIZE			= 13,
	KEY_FONT_INFO_STYLE			= 14,
	KEY_FONT_INFO_COLOR			= 15,
	KEY_FONT_EFFECT_TEXTTAG			= 16,
	KEY_FONT_EFFECT_ITALIC			= 17,
	KEY_FONT_EFFECT_UNDERLINE		= 18,
	KEY_FONT_EFFECT_EDGETYPE		= 19,
	KEY_FONT_EFFECT_OFFSET			= 20,
	KEY_FONT_EFFECT_EDGECOLOR		= 21,
	KEY_TEXT_ROW_DESCRIPTOR_HORI		= 22,
	KEY_TEXT_ROW_DESCRIPTOR_VERT		= 23,
	KEY_TEXT_ROW_DESCRIPTOR_PRINT_DIRECTION	= 24,
	KEY_TEXT_DISPLAY_DESCRIPTOR_WRAP	= 25,
	KEY_TEXT_DISPLAY_DESCRIPTOR_SCROLL_DIRECTION = 26,
	KEY_IMAGE_SIZE				= 27,
	KEY_IMAGE_DATA			 	= 28,
	KEY_IMAGE_WIDTH             = 29,
	KEY_IMAGE_HEIGHT			= 30,
	KEY_IMAGE_TYPE              = 31,

	KEY_SUBTITLE_INFO			= 100,
	KEY_RECT_INFO				= 101,
	KEY_DISPLAY_INFO			= 102,
	KEY_TEXT_ROW_INFO			= 103,
	KEY_TEXT_INFO				= 104,
	KEY_IMAGE_INFO				= 105,
	KEY_STRING_INFO				= 106,
	KEY_FONT_INFO				= 107,
	KEY_FONT_EFFECT				= 108,
	KEY_TEXT_ROW_DESCRIPTOR			= 109,
	KEY_TEXT_DISPLAY_DESCRIPTOR		= 110,
	KEY_IMAGE_DISPLAY_DESCRIPTOR		= 111,
};
uint32_t CreateColor(uint32_t nRed,uint32_t nGreen,uint32_t nBlue,uint32_t nTransparency)
{
	uint32_t clr = (nTransparency)<<24
		|nRed<<16 | nGreen<<8 | nBlue;//CJavaParcelWrap::convertTransparent
	return clr;
}
int CJavaParcelWrap::getParcelFromSubtitleInfo(voSubtitleInfo* sample,CJavaParcelWrap* parcel)
{
		voSubtitleInfo* stinfo = sample;
		uint32_t rgba;

		parcel->writeInt32(KEY_START_TIME);
		parcel->writeInt32(stinfo->nTimeStamp);

		parcel->writeInt32(KEY_SUBTITLE_INFO);
		voSubtitleInfoEntry* pSubtitleEntry = stinfo->pSubtitleEntry;
		while (pSubtitleEntry != NULL)
		{
			parcel->writeInt32(KEY_DURATION);
			parcel->writeInt32(pSubtitleEntry->nDuration);
			
			parcel->writeInt32(KEY_RECT_INFO);
			parcel->writeInt32(KEY_STRUCT_RECT);
			parcel->writeInt32(pSubtitleEntry->stSubtitleRectInfo.stRect.nTop);
			parcel->writeInt32(pSubtitleEntry->stSubtitleRectInfo.stRect.nLeft);
			parcel->writeInt32(pSubtitleEntry->stSubtitleRectInfo.stRect.nBottom);
			parcel->writeInt32(pSubtitleEntry->stSubtitleRectInfo.stRect.nRight);

			parcel->writeInt32(KEY_RECT_BORDER_TYPE);
			parcel->writeInt32(pSubtitleEntry->stSubtitleRectInfo.nRectBorderType);

			parcel->writeInt32(KEY_RECT_BORDER_COLOR);
			rgba = CreateColor((uint32_t) pSubtitleEntry->stSubtitleRectInfo.stRectBorderColor.nRed ,
					(uint32_t) pSubtitleEntry->stSubtitleRectInfo.stRectBorderColor.nGreen,
					(uint32_t) pSubtitleEntry->stSubtitleRectInfo.stRectBorderColor.nBlue,
					(uint32_t) pSubtitleEntry->stSubtitleRectInfo.stRectBorderColor.nTransparency
					);
			parcel->writeInt32(rgba);

			parcel->writeInt32(KEY_RECT_FILL_COLOR);
			rgba = CreateColor((uint32_t) pSubtitleEntry->stSubtitleRectInfo.stRectFillColor.nRed ,
					(uint32_t) pSubtitleEntry->stSubtitleRectInfo.stRectFillColor.nGreen,
					(uint32_t) pSubtitleEntry->stSubtitleRectInfo.stRectFillColor.nBlue,
					(uint32_t) pSubtitleEntry->stSubtitleRectInfo.stRectFillColor.nTransparency
					);
			parcel->writeInt32(rgba);

			parcel->writeInt32(KEY_RECT_Z_ORDER);
			parcel->writeInt32(pSubtitleEntry->stSubtitleRectInfo.nRectZOrder);

			parcel->writeInt32(KEY_DISPLAY_EFFECTTYPE);
			//write as U32
			parcel->writeInt32((0 << 16) | pSubtitleEntry->stSubtitleRectInfo.stRectDisplayEffct.nEffectType);

			parcel->writeInt32(KEY_DISPLAY_EFFECTDIRECTION);
			//write as U32
			parcel->writeInt32((0 << 16) | pSubtitleEntry->stSubtitleRectInfo.stRectDisplayEffct.nEffectDirection);

			parcel->writeInt32(KEY_DISPLAY_EFFECTSPEED);
			//write as U32
			parcel->writeInt32(pSubtitleEntry->stSubtitleRectInfo.stRectDisplayEffct.nEffectSpeed);
			
			parcel->writeInt32(KEY_DISPLAY_INFO);
			parcel->writeInt32(KEY_TEXT_ROW_INFO);
			voSubtitleTextRowInfo*  pRowInfo = pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo;
			while(pRowInfo != NULL)
			{
				voSubtitleTextInfoEntry* pTextInfoEntry = pRowInfo->pTextInfoEntry;
				while(pTextInfoEntry != NULL)
				{
					parcel->writeInt32(KEY_TEXT_SIZE);
					parcel->writeInt32(pTextInfoEntry->nSize);
					parcel->writeInt32(KEY_TEXT_STRING);
					parcel->write(pTextInfoEntry->pString,
							pTextInfoEntry->nSize);
					parcel->writeInt32(KEY_STRING_INFO);

					parcel->writeInt32(KEY_FONT_INFO);
					parcel->writeInt32(KEY_FONT_INFO_SIZE);
					parcel->writeInt32((0 << 16) | pTextInfoEntry->stStringInfo.stFontInfo.FontSize);
					parcel->writeInt32(KEY_FONT_INFO_STYLE);
					parcel->writeInt32((0 << 16) | pTextInfoEntry->stStringInfo.stFontInfo.FontStyle);
					parcel->writeInt32(KEY_FONT_INFO_COLOR);
					rgba = CreateColor((uint32_t) pTextInfoEntry->stStringInfo.stFontInfo.stFontColor.nRed ,
							(uint32_t) pTextInfoEntry->stStringInfo.stFontInfo.stFontColor.nGreen,
							(uint32_t) pTextInfoEntry->stStringInfo.stFontInfo.stFontColor.nBlue,
							(uint32_t) pTextInfoEntry->stStringInfo.stFontInfo.stFontColor.nTransparency
							);
					parcel->writeInt32(rgba);
					
					parcel->writeInt32(KEY_FONT_EFFECT);
					parcel->writeInt32(KEY_FONT_EFFECT_TEXTTAG);
					parcel->writeInt32(pTextInfoEntry->stStringInfo.stCharEffect.TextTag);
					parcel->writeInt32(KEY_FONT_EFFECT_ITALIC);
					parcel->writeInt32((0 << 8) | pTextInfoEntry->stStringInfo.stCharEffect.Italic);
					parcel->writeInt32(KEY_FONT_EFFECT_UNDERLINE);
					parcel->writeInt32((0 << 8) | pTextInfoEntry->stStringInfo.stCharEffect.Underline);
					parcel->writeInt32(KEY_FONT_EFFECT_EDGETYPE);
					parcel->writeInt32((0 << 16) | pTextInfoEntry->stStringInfo.stCharEffect.EdgeType);
					parcel->writeInt32(KEY_FONT_EFFECT_OFFSET);
					parcel->writeInt32(pTextInfoEntry->stStringInfo.stCharEffect.Offset);
					parcel->writeInt32(KEY_FONT_EFFECT_EDGECOLOR);
					rgba = CreateColor((uint32_t) pTextInfoEntry->stStringInfo.stCharEffect.EdgeColor.nRed ,
							(uint32_t) pTextInfoEntry->stStringInfo.stCharEffect.EdgeColor.nGreen,
							(uint32_t) pTextInfoEntry->stStringInfo.stCharEffect.EdgeColor.nBlue,
							(uint32_t) pTextInfoEntry->stStringInfo.stCharEffect.EdgeColor.nTransparency
							);
					parcel->writeInt32(rgba);

					pTextInfoEntry = pTextInfoEntry->pNext;
				}
				parcel->writeInt32(KEY_TEXT_ROW_DESCRIPTOR);
				parcel->writeInt32(KEY_RECT_INFO);
				
				parcel->writeInt32(KEY_STRUCT_RECT);
				parcel->writeInt32(pRowInfo->stTextRowDes.stDataBox.stRect.nTop);
				parcel->writeInt32(pRowInfo->stTextRowDes.stDataBox.stRect.nLeft);
				parcel->writeInt32(pRowInfo->stTextRowDes.stDataBox.stRect.nBottom);
				parcel->writeInt32(pRowInfo->stTextRowDes.stDataBox.stRect.nRight);

				parcel->writeInt32(KEY_RECT_BORDER_TYPE);
				parcel->writeInt32(pRowInfo->stTextRowDes.stDataBox.nRectBorderType);

				parcel->writeInt32(KEY_RECT_BORDER_COLOR);
				rgba = CreateColor((uint32_t) pRowInfo->stTextRowDes.stDataBox.stRectBorderColor.nRed ,
						(uint32_t) pRowInfo->stTextRowDes.stDataBox.stRectBorderColor.nGreen,
						(uint32_t) pRowInfo->stTextRowDes.stDataBox.stRectBorderColor.nBlue,
						(uint32_t) pRowInfo->stTextRowDes.stDataBox.stRectBorderColor.nTransparency
						);
				parcel->writeInt32(rgba);

				parcel->writeInt32(KEY_RECT_FILL_COLOR);
				rgba = CreateColor((uint32_t) pRowInfo->stTextRowDes.stDataBox.stRectFillColor.nRed ,
						(uint32_t) pRowInfo->stTextRowDes.stDataBox.stRectFillColor.nGreen,
						(uint32_t) pRowInfo->stTextRowDes.stDataBox.stRectFillColor.nBlue,
						(uint32_t) pRowInfo->stTextRowDes.stDataBox.stRectFillColor.nTransparency
						);
				parcel->writeInt32(rgba);

				parcel->writeInt32(KEY_RECT_Z_ORDER);
				parcel->writeInt32(pRowInfo->stTextRowDes.stDataBox.nRectZOrder);

				parcel->writeInt32(KEY_DISPLAY_EFFECTTYPE);
				//write as U32
				parcel->writeInt32((0 << 16) | pRowInfo->stTextRowDes.stDataBox.stRectDisplayEffct.nEffectType);

				parcel->writeInt32(KEY_DISPLAY_EFFECTDIRECTION);
				//write as U32
				parcel->writeInt32((0 << 16) | pRowInfo->stTextRowDes.stDataBox.stRectDisplayEffct.nEffectDirection);

				parcel->writeInt32(KEY_DISPLAY_EFFECTSPEED);
				//write as U32
				parcel->writeInt32(pRowInfo->stTextRowDes.stDataBox.stRectDisplayEffct.nEffectSpeed);

				parcel->writeInt32(KEY_TEXT_ROW_DESCRIPTOR_HORI);
				parcel->writeInt32((0 << 16) | pRowInfo->stTextRowDes.nHorizontalJustification);
				parcel->writeInt32(KEY_TEXT_ROW_DESCRIPTOR_VERT);
				parcel->writeInt32((0 << 16) | pRowInfo->stTextRowDes.nVerticalJustification);
				parcel->writeInt32(KEY_TEXT_ROW_DESCRIPTOR_PRINT_DIRECTION);
				parcel->writeInt32(pRowInfo->stTextRowDes.nPrintDirection);
			
				pRowInfo = pRowInfo->pNext;	
			}
			parcel->writeInt32(KEY_TEXT_DISPLAY_DESCRIPTOR);
			parcel->writeInt32(KEY_TEXT_DISPLAY_DESCRIPTOR_WRAP);
			parcel->writeInt32(pSubtitleEntry->stSubtitleDispInfo.stDispDescriptor.stWrap);
			parcel->writeInt32(KEY_TEXT_DISPLAY_DESCRIPTOR_SCROLL_DIRECTION);
			parcel->writeInt32(pSubtitleEntry->stSubtitleDispInfo.stDispDescriptor.nScrollDirection);
			
			voSubtitleImageInfo* pImageInfo = pSubtitleEntry->stSubtitleDispInfo.pImageInfo;
			while(pImageInfo !=NULL)
			{
				parcel->writeInt32(KEY_IMAGE_INFO);
				parcel->writeInt32(KEY_IMAGE_TYPE);
				parcel->writeInt32(pImageInfo->stImageData.nType);
				parcel->writeInt32(KEY_IMAGE_WIDTH);
				parcel->writeInt32(pImageInfo->stImageData.nWidth);
				parcel->writeInt32(KEY_IMAGE_HEIGHT);
				parcel->writeInt32(pImageInfo->stImageData.nHeight);
				parcel->writeInt32(KEY_IMAGE_SIZE);
				parcel->writeInt32(pImageInfo->stImageData.nSize);
				parcel->writeInt32(KEY_IMAGE_DATA);
				parcel->write(pImageInfo->stImageData.pPicData,pImageInfo->stImageData.nSize);

				VOLOGI("KEY_IMAGE_TYPE is %d, KEY_IMAGE_WIDTH is %d, KEY_IMAGE_HEIGHT is %d, KEY_IMAGE_SIZE is %d",
					pImageInfo->stImageData.nType, pImageInfo->stImageData.nWidth, pImageInfo->stImageData.nHeight, pImageInfo->stImageData.nSize);

				parcel->writeInt32(KEY_IMAGE_DISPLAY_DESCRIPTOR);
				parcel->writeInt32(KEY_RECT_INFO);
				
				parcel->writeInt32(KEY_STRUCT_RECT);
				parcel->writeInt32(pImageInfo->stImageDesp.stImageRectInfo.stRect.nTop);
				parcel->writeInt32(pImageInfo->stImageDesp.stImageRectInfo.stRect.nLeft);
				parcel->writeInt32(pImageInfo->stImageDesp.stImageRectInfo.stRect.nBottom);
				parcel->writeInt32(pImageInfo->stImageDesp.stImageRectInfo.stRect.nRight);
	
				parcel->writeInt32(KEY_RECT_BORDER_TYPE);
				parcel->writeInt32(pImageInfo->stImageDesp.stImageRectInfo.nRectBorderType);
	
				parcel->writeInt32(KEY_RECT_BORDER_COLOR);
				rgba = CreateColor((uint32_t) pImageInfo->stImageDesp.stImageRectInfo.stRectBorderColor.nRed ,
						(uint32_t) pImageInfo->stImageDesp.stImageRectInfo.stRectBorderColor.nGreen,
						(uint32_t) pImageInfo->stImageDesp.stImageRectInfo.stRectBorderColor.nBlue,
						(uint32_t) pImageInfo->stImageDesp.stImageRectInfo.stRectBorderColor.nTransparency
						);
				parcel->writeInt32(rgba);
	
				parcel->writeInt32(KEY_RECT_FILL_COLOR);
				rgba = CreateColor((uint32_t) pImageInfo->stImageDesp.stImageRectInfo.stRectFillColor.nRed ,
						(uint32_t) pImageInfo->stImageDesp.stImageRectInfo.stRectFillColor.nGreen,
						(uint32_t) pImageInfo->stImageDesp.stImageRectInfo.stRectFillColor.nBlue,
						(uint32_t) pImageInfo->stImageDesp.stImageRectInfo.stRectFillColor.nTransparency
						);
				parcel->writeInt32(rgba);
	
				parcel->writeInt32(KEY_RECT_Z_ORDER);
				parcel->writeInt32(pImageInfo->stImageDesp.stImageRectInfo.nRectZOrder);
	
				parcel->writeInt32(KEY_DISPLAY_EFFECTTYPE);
				//write as U32
				parcel->writeInt32((0 << 16) | pImageInfo->stImageDesp.stImageRectInfo.stRectDisplayEffct.nEffectType);
	
				parcel->writeInt32(KEY_DISPLAY_EFFECTDIRECTION);
				//write as U32
				parcel->writeInt32((0 << 16) | pImageInfo->stImageDesp.stImageRectInfo.stRectDisplayEffct.nEffectDirection);
	
				parcel->writeInt32(KEY_DISPLAY_EFFECTSPEED);
				//write as U32
				parcel->writeInt32(pImageInfo->stImageDesp.stImageRectInfo.stRectDisplayEffct.nEffectSpeed);
				pImageInfo = pImageInfo->pNext;
			}
			pSubtitleEntry = pSubtitleEntry->pNext;
		}
		return 0;
}

int CJavaParcelWrap::getParcelFromPerformanceData(VOOSMP_PERFORMANCE_DATA* perf,CJavaParcelWrap* parcel)
{

	parcel->writeInt32(perf->nLastTime);
	parcel->writeInt32(perf->nSourceDropNum);
	parcel->writeInt32(perf->nCodecDropNum);
	parcel->writeInt32(perf->nRenderDropNum);
	parcel->writeInt32(perf->nDecodedNum);
	parcel->writeInt32(perf->nRenderNum);
	parcel->writeInt32(perf->nSourceTimeNum);
	parcel->writeInt32(perf->nCodecTimeNum);
	parcel->writeInt32(perf->nRenderTimeNum);
	parcel->writeInt32(perf->nJitterNum);
	parcel->writeInt32(perf->nCodecErrorsNum);
	for (int i = 0; i < perf->nCodecErrorsNum; i++)
	{
		parcel->writeInt32(perf->nCodecErrors[i]);
	}
	
	parcel->writeInt32(perf->nCPULoad);
	parcel->writeInt32(perf->nFrequency);
	parcel->writeInt32(perf->nMaxFrequency);
	parcel->writeInt32(perf->nWorstDecodeTime);
	parcel->writeInt32(perf->nWorstRenderTime);
	parcel->writeInt32(perf->nAverageDecodeTime);
	parcel->writeInt32(perf->nAverageRenderTime);
	parcel->writeInt32(perf->nTotalCPULoad);
	parcel->writeInt32(perf->nTotalPlaybackDuration);
	parcel->writeInt32(perf->nTotalSourceDropNum);
	parcel->writeInt32(perf->nTotalCodecDropNum);
	parcel->writeInt32(perf->nTotalRenderDropNum);
	parcel->writeInt32(perf->nTotalDecodedNum);
	parcel->writeInt32(perf->nTotalRenderNum);

	return 0;
}


#ifdef _VONAMESPACE
}
#endif
