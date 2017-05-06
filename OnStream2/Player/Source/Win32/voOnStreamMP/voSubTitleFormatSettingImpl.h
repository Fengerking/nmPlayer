
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
 * @file voSubTitleFormatSettingImpl.h
 * The interface of settings for subtitle.
 *
 * 
 *
 *
 * @author  Li Mingbo
 * @date    2012-2012 
 ************************************************************************/
#pragma once

#include <windows.h>
#include <tchar.h>
#include "voSubTitleFormatSetting.h"


class voSubTitleFormatSettingImpl : public voSubTitleFormatSetting{
	
public:
	voSubTitleFormatSettingImpl();

	virtual ~voSubTitleFormatSettingImpl();

	/**
     * set font color.
     *
     * @param nFontColor the font color for text
     */
	virtual void setFontColor(COLORREF nFontColor);
	
    /**
     * set font color opacity rate.
     *
     * @param nAlpha the font color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
     */
	virtual void setFontOpacity(int nAlpha);
	
	//void setFontSize(float nSmallFontSize, float nMediumFontSize, float nLargeFontSzie);
	
    /**
     * set font size scale.
     *
     * @param nFontSize the font size scale for text, the parameter value is between 50 and 200, 50 is the smallest and is the 0.5 times of default font size, 200 is the biggest font and is the 2.0 times of default font size.
     */
	virtual void setFontSizeScale(int nFontSize);
	
    /**
     * set text background color.
     *
     * @param nBackgroundColor the background color for text
     */
	virtual void setBackgroundColor(COLORREF nBackgroundColor);
    /**
     * set background color opacity rate.
     *
     * @param nAlpha the background color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
     */
	virtual void setBackgroundOpacity(int nAlpha);
	
    /**
     * set window color.
     *
     * @param nBackgroundColor the window color for text
     */
	virtual void setWindowColor(COLORREF nBackgroundColor);
    /**
     * set window background color opacity rate.
     *
     * @param nAlpha the window background color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
     */
	virtual void setWindowOpacity(int nAlpha);
	
    /**
     * set font as italic or not.
     *
     * @param nFontItalic if it is true, the font will be italic
     */
	virtual void setFontItalic(bool nFontItalic);
	
    /**
     * set font as bold or not.
     *
     * @param nBold if it is true, the font will be bold
     */
	virtual void setFontBold(bool nBold);
	
    /**
     * set font as underline or not.
     *
     * @param nFontUnderline if it is true, the font will be underline
     */
	virtual void setFontUnderline(bool nFontUnderline);
	
    /**
     * set font name.
     *
     * @param strFontName the font name for text
     */
	virtual void setFontName(TCHAR* strFontName);
	
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
	virtual void setFontStyle(int style);
	
    /**
     * set the edge type of font.
     *
     * @param edgeType the edge type of font, 0:NONE,1:RAISED,2:DEPRESSED,3:UNIFORM,4:LEFT_DROP_SHADOW,5:RIGHT_DROP_SHADOW
     */
	virtual void setEdgeType(int edgeType);
	
    /**
     * set font edge color.
     *
     * @param edgeColor the font edge color for text
     */
	virtual void setEdgeColor(COLORREF edgeColor);
    /**
     * set edge color opacity rate.
     *
     * @param nAlpha the edge color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
     */
	virtual void setEdgeOpacity(int nAlpha);
	
    /**
	 * Reset all parameters to default status. Subtitle will be presented as subtitle stream specified, instead of user specified
     */
	virtual void reset();

public:
	//gets
	int GetEdgeType(){return edgeType;};

	COLORREF GetEdgeColor(){return edgeColor;};
	COLORREF GetFontColor(){return fontColor;};
	COLORREF GetWindowBackgroundColor(){return windowBackgroundColor;};
	COLORREF GetBackgroundColor(){return backgroundColor;};
	int GetEdgeColorOpacityRate(){return edgeColorOpacityRate;};
	int GetFontColorOpacityRate(){return fontColorOpacityRate;};
	int GetWindowBackgroundColorOpacityRate(){return windowBackgroundColorOpacityRate;};
	int GetBackgroundColorOpacityRate(){return backgroundColorOpacityRate;};

	float GetFontSizeMedium(){return fontSizeMedium;};
	int GetFontItalic(){return fontItalic;};
	int GetFontUnderline(){return fontUnderline;};
	int GetFontBold(){return fontBold;};
	voSubtitleRGBAColor converColor(voSubtitleRGBAColor oldColor, COLORREF newColor, bool enableColor);
	voSubtitleRGBAColor converColorOpacityRate(voSubtitleRGBAColor oldColor, int newColorOpacityRate, bool enableColorOpacityRate);
	bool GetFontColorEnable(){return fontColorEnable;};
	bool GetEdgeColorEnable(){return edgeColorEnable;};
	bool GetBackgroundColorEnable(){return backgroundColorEnable;};
	bool GetWindowBackgroundColorEnable(){return windowBackgroundColorEnable;};
	bool GetFontColorOpacityRateEnable(){return fontColorOpacityRateEnable;};
	bool GetEdgeColorOpacityRateEnable(){return edgeColorOpacityRateEnable;};
	bool GetBackgroundColorOpacityRateEnable(){return backgroundColorOpacityRateEnable;};
	bool GetWindowBackgroundColorOpacityRateEnable(){return windowBackgroundColorOpacityRateEnable;};

	bool GetFontSizeEnable(){return fontSizeEnable;};
	bool GetFontItalicEnable(){return fontItalicEnable;};
	bool GetFontUnderlineEnable(){return fontUnderlineEnable;};
	bool GetFontBoldEnable(){return fontBoldEnable;};

	void ConvertFontName(TCHAR* fnm, int style);
	TCHAR* GetFontName();
protected:
	int checkAlpha(int nAlpha);
	void setFontSize(float nSmallFontSize, float nMediumFontSize, float nLargeFontSzie);
	void setFontSize(float nFontSize);


private:
	int edgeType;

	COLORREF edgeColor;
	COLORREF fontColor;
	COLORREF windowBackgroundColor;
	COLORREF backgroundColor;
	int edgeColorOpacityRate;
	int fontColorOpacityRate;
	int windowBackgroundColorOpacityRate;
	int backgroundColorOpacityRate;

	float fontSizeMedium;
	int	fontItalic;
	int	fontUnderline;
	int	fontBold;
	bool edgeTypeEnable;

	bool fontColorEnable;
	bool edgeColorEnable;
	bool backgroundColorEnable;
	bool windowBackgroundColorEnable;
	bool fontColorOpacityRateEnable;
	bool edgeColorOpacityRateEnable;
	bool backgroundColorOpacityRateEnable;
	bool windowBackgroundColorOpacityRateEnable;

	bool fontSizeEnable;
	bool	fontItalicEnable;
	bool	fontUnderlineEnable;
	bool	fontBoldEnable;
	TCHAR	fontName[64];

	RECT	m_rectDisplay;


};
