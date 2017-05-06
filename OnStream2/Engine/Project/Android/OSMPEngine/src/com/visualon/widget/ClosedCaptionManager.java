
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
 * @file ClosedCaptionManager.java
 * wrap class for class voSubTitleManager.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/


package com.visualon.widget;

import com.visualon.OSMPSubTitle.voSubTitleManager;


/**
 * 
 * ClosedCaptionManager class is deprecated.
 * Please use com.visualon.OSMPSubTitle.voSubTitleManager class
 * 
 * 
 * @author zhang_yi
 *
 */
public class ClosedCaptionManager extends voSubTitleManager{
	public voSubTitleFormatSettingImpl getSettings()
	{
		if(settings == null)
			settings = new CCSettings();
		return settings;
	}
	
	/**
	 * 
	 * CCSettings class is deprecated.
	 * Please use com.visualon.OSMPSubTitle.voSubTitleFormatSetting
	 * 
	 * @author zhang_yi
	 *
	 */
	public class CCSettings extends voSubTitleManager.voSubTitleFormatSettingImpl{
	
	}
}
