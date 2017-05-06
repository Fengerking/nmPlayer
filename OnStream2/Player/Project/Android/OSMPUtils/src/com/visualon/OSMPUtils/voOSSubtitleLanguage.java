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
 * @file voOSSubtitleLanguage.java
 * interface for getting Subtitle Language information.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public interface voOSSubtitleLanguage {
	
    /**Indicate unknow subtitle */
    public static final int VOOSMP_LANG_TYPE_UNKNOWN 					= 0;
    
    /**Indicate Simple Chinese subtitle */
    public static final int VOOSMP_LANG_TYPE_SIMPLE_CHINESE				= 1;
    
    /**Indicate Traditional Chinese subtitle */
    public static final int VOOSMP_LANG_TYPE_TRADITIONAL_CHINESE		= 2;
    
    /**Indicate English subtitle */
    public static final int VOOSMP_LANG_TYPE_ENGLISH					= 3;
    
    /**Indicate French subtitle */
    public static final int VOOSMP_LANG_TYPE_FRENCH						= 4;     
    
    /**Indicate Korean subtitle */
    public static final int VOOSMP_LANG_TYPE_KOREAN						= 5;                    
    
    /**Indicate Japanese subtitle */
    public static final int VOOSMP_LANG_TYPE_JAPANESE					= 6;
    
    /**Indicate Germany subtitle */                 
    public static final int VOOSMP_LANG_TYPE_GERMANY					= 7;
    
    /**Indicate Italian subtitle */
    public static final int VOOSMP_LANG_TYPE_ITALIAN					= 8;
    
    /**Indicate Spanish subtitle */
    public static final int VOOSMP_LANG_TYPE_SPANISH					= 9;   

	
	/**
	 * Subtile language name 
	 * 
	 * @return language name
	 */
    public String   		LangName();
    
    /**
     * Subtitle language type
     * 
     * @return value refer to 
     * <ul>
     * <li> {@link #VOOSMP_LANG_TYPE_UNKNOWN}
     * <li> {@link #VOOSMP_LANG_TYPE_SIMPLE_CHINESE}
     * <li> {@link #VOOSMP_LANG_TYPE_TRADITIONAL_CHINESE}
     * <li> {@link #VOOSMP_LANG_TYPE_ENGLISH}
     * <li> {@link #VOOSMP_LANG_TYPE_FRENCH}
     * <li> {@link #VOOSMP_LANG_TYPE_KOREAN}
     * <li> {@link #VOOSMP_LANG_TYPE_JAPANESE}
     * <li> {@link #VOOSMP_LANG_TYPE_GERMANY}
     * <li> {@link #VOOSMP_LANG_TYPE_ITALIAN}
     * <li> {@link #VOOSMP_LANG_TYPE_SPANISH}
     * </ul>
     */
    public int             	LanguageType();      
    
    /**Not used now */
    public int             	Flag();
    
    /**Reserved */
    public int             	Reserved1();    
    
    /**Reserved */
    public int             	Reserved2();         

}
