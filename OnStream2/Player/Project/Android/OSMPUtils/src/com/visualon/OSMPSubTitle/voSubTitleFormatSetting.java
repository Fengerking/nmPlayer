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
 * @file voSubTitleFormatSetting.java
 * The interface of settings for subtitle.
 *
 * 
 *
 *
 * @author  Li Mingbo
 * @date    2012-2012 
 ************************************************************************/


package com.visualon.OSMPSubTitle;

import java.util.ArrayList;

import android.graphics.Rect;
import android.graphics.Typeface;

public interface voSubTitleFormatSetting {
	
	public static final int VOOSMP_FontStyle_Default										= 0;
	public static final int VOOSMP_Monospaced_with_serifs									= 1;
	public static final int VOOSMP_FontStyle_Default_Proportionally_spaced_with_serifs		= 2;
	public static final int VOOSMP_FontStyle_Default_Monospaced_without_serifs				= 3;
	public static final int VOOSMP_FontStyle_Default_Proportionally_spaced_without_serifs	= 4;
	public static final int VOOSMP_FontStyle_Default_Casual									= 5;
	public static final int VOOSMP_FontStyle_Default_Cursive								= 6;
	public static final int VOOSMP_FontStyle_Default_Small_capitals							= 7;
	public static final int VOOSMP_FontStyle_Monospaced										= 8;
	public static final int VOOSMP_FontStyle_SansSerif										= 9;
	public static final int VOOSMP_FontStyle_Serif											= 10;
	public static final int VOOSMP_FontStyle_ProportionalSansSerif							= 11;
	public static final int VOOSMP_FontStyle_ProportionalSerif								= 12;
	public static final int VOOSMP_FontStyle_Times_New_Roman								= 13;
	public static final int VOOSMP_FontStyle_Courier										= 14;
	public static final int VOOSMP_FontStyle_Helvetica										= 15;
	public static final int VOOSMP_FontStyle_Arial											= 16;
	public static final int VOOSMP_FontStyle_Dom											= 17;
	public static final int VOOSMP_FontStyle_Coronet										= 18;
	public static final int VOOSMP_FontStyle_Gothic											= 19;
	public static final int VOOSMP_FontStyle_MonospaceSansSerif								= 20;
	
	
    /**
     * set font color.
     *
     * @param nFontColor the font color for text
     */
	public void setFontColor(int nFontColor);
	
    /**
     * set font color opacity rate.
     *
     * @param nAlpha the font color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
     */
	public void setFontOpacity(int nAlpha);
	
	//public void setFontSize(float nSmallFontSize, float nMediumFontSize, float nLargeFontSzie);
	
    /**
     * set font size scale.
     *
     * @param nFontSize the font size scale for text, the parameter value is between 50 and 200, 50 is the smallest and is the 0.5 times of default font size, 200 is the biggest font and is the 2.0 times of default font size.
     */
	public void setFontSizeScale(int nFontSize);
	
    /**
     * set text background color.
     *
     * @param nBackgroundColor the background color for text
     */
	public void setBackgroundColor(int nBackgroundColor);
    /**
     * set background color opacity rate.
     *
     * @param nAlpha the background color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
     */
	public void setBackgroundOpacity(int nAlpha);
	
    /**
     * set window color.
     *
     * @param nBackgroundColor the window color for text
     */
	public void setWindowColor(int nBackgroundColor);
    /**
     * set window background color opacity rate.
     *
     * @param nAlpha the window background color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
     */
	public void setWindowOpacity(int nAlpha);
	
    /**
     * set font as italic or not.
     *
     * @param nFontItalic if it is true, the font will be italic
     */
	public void setFontItalic(boolean nFontItalic);
	
    /**
     * set font as bold or not.
     *
     * @param nBold if it is true, the font will be bold
     */
	public void setFontBold(boolean nBold);
	
    /**
     * set font as underline or not.
     *
     * @param nFontUnderline if it is true, the font will be underline
     */
	public void setFontUnderline(boolean nFontUnderline);

    /**
     * set font type face.
     *
     * @param tf the font type face for text
     */
	public void setTypeface(Typeface tf);

    /**
     * set font name.
     *
     * @param strFontName the font name for text
     */
	public void setFontName(String strFontName);
	
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
	 * <li> {@link #VOOSMP_FontStyle_Monospaced}
	 * <li> {@link #VOOSMP_FontStyle_SansSerif}
	 * <li> {@link #VOOSMP_FontStyle_Serif}
	 * <li> {@link #VOOSMP_FontStyle_ProportionalSansSerif}
	 * <li> {@link #VOOSMP_FontStyle_ProportionalSerif}
	 * <li> {@link #VOOSMP_FontStyle_Times_New_Roman}
	 * <li> {@link #VOOSMP_FontStyle_Courier}
	 * <li> {@link #VOOSMP_FontStyle_Helvetica}
	 * <li> {@link #VOOSMP_FontStyle_Arial}
	 * <li> {@link #VOOSMP_FontStyle_Dom}
	 * <li> {@link #VOOSMP_FontStyle_Coronet}
	 * <li> {@link #VOOSMP_FontStyle_Gothic}
	 * <li> {@link #VOOSMP_FontStyle_MonospaceSansSerif}
     */
	public void setFontStyle(int style);
	
    /**
     * set the edge type of font.
     *
     * @param edgeType the edge type of font, 0:NONE,1:RAISED,2:DEPRESSED,3:UNIFORM,4:LEFT_DROP_SHADOW,5:RIGHT_DROP_SHADOW
     */
	public void setEdgeType(int edgeType);
	
    /**
     * set font edge color.
     *
     * @param edgeColor the font edge color for text
     */
	public void setEdgeColor(int edgeColor);
    /**
     * set edge color opacity rate.
     *
     * @param nAlpha the edge color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
     */
	public void setEdgeOpacity(int nAlpha);
	
    /**
	 * Reset all parameters to default status. Subtitle will be presented as subtitle stream specified, instead of user specified
     */
	public void reset();
    /**
     * clear all rectangle settings of rows.
     */
	//public void clearRowRectList();
	
    /**
     * add rectangle settings of row.
     *
     * @param rt the rectangle information of row, 0=<rt.left<100 0=<rt.top<100 0<rt.right<=100 0<rt.bottom<=100 
     */
	//public void addRowRect(Rect rt);
}
