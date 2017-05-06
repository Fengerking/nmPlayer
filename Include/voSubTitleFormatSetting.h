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
#pragma once

#include <windows.h>
#include <tchar.h>
#include "voSubtitleType.h"

#define VOOSMP_PID_CLOSED_CAPTION_SETTINGS  0x00002901  /*!< to get subtitle format setting object, return voSubTitleFormatSetting object */

class voSubTitleFormatSetting {
	
	
public:
    /**
     * set font color.
     *
     * @param nFontColor the font color for text
     */
	virtual void setFontColor(COLORREF nFontColor) = 0;
	
    /**
     * set font size scale.
     *
     * @param nFontSize the font size scale for text, the parameter value is between 50 and 200, 50 is the smallest and is the 0.5 times of default font size, 200 is the biggest font and is the 2.0 times of default font size.
     */
	virtual void setFontSizeScale(int nFontSize) = 0;
	
    /**
     * set text background color.
     *
     * @param nBackgroundColor the background color for text
     */
	virtual void setBackgroundColor(COLORREF nBackgroundColor) = 0;
	
    /**
     * set window color.
     *
     * @param nBackgroundColor the window color for text
     */
	virtual void setWindowColor(COLORREF nBackgroundColor) = 0;
    /**
     * set window background color opacity rate.
     *
     * @param nAlpha the window background color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
     */
	virtual void setWindowOpacity(int nAlpha) = 0;
	
    /**
     * set font as italic or not.
     *
     * @param nFontItalic if it is true, the font will be italic
     */
	virtual void setFontItalic(bool nFontItalic) = 0;
	
    /**
     * set font as bold or not.
     *
     * @param nBold if it is true, the font will be bold
     */
	virtual void setFontBold(bool nBold) = 0;
	
    /**
     * set font as underline or not.
     *
     * @param nFontUnderline if it is true, the font will be underline
     */
	virtual void setFontUnderline(bool nFontUnderline) = 0;
	
    /**
     * set font name.
     *
     * @param strFontName the font name for text
     */
	virtual void setFontName(TCHAR* strFontName) = 0;
	
    /**
     * set font style, same as setFontName.
     *
     * @param style the font style for text, style is value of below:
     * <li> {@link #VOOSMP_FontStyle_Default}
     * <li> {@link #VOOSMP_Monospaced_with_serifs}
     * <li> {@link #VOOSMP_FontStyle_Default_Proportionally_spaced_with_serifs}
     * <li> {@link #VOOSMP_FontStyle_Default_Monospaced_without_serifs}
     * <li> {@link #VOOSMP_FontStyle_Default_Proportionally_spaced_without_serifs}
     * <li> {@link #VOOSMP_FontStyle_Default_Casual}
     * <li> {@link #VOOSMP_FontStyle_Default_Cursive}
     * <li> {@link #VOOSMP_FontStyle_Default_Small_capitals}
     */
	virtual void setFontStyle(int style) = 0;
	
    /**
     * set the edge type of font.
     *
     * @param edgeType the edge type of font, 0:NONE,1:RAISED,2:DEPRESSED,3:UNIFORM,4:LEFT_DROP_SHADOW,5:RIGHT_DROP_SHADOW
     */
	virtual void setEdgeType(int edgeType) = 0;
	
    /**
     * set font edge color.
     *
     * @param edgeColor the font edge color for text
     */
	virtual void setEdgeColor(COLORREF edgeColor) = 0;
	
    /**
	 * Reset all parameters to default status. Subtitle will be presented as subtitle stream specified, instead of user specified
     */
	virtual void reset() = 0;
};
