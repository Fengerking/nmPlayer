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

package com.visualon.OSMPUtils;

import android.view.View;

/**
 * Class for previewing subtitle information
 */
public class voOSPreviewSubtitleInfo {
	

	/** Text for previewing current subtitle settings */
	private String		mSampleText;
	/** View to which the sample text is being rendered */
	private View		mView;
	
	/**
	 * Get text for previewing current subtitle settings
	 * 
	 * @return text for previewing current subtitle settings
	 */
	public String getSampleText() {
		return mSampleText;
	}
	
	/**
	 * Set text for previewing current subtitle settings
	 * 
	 * @param text for previewing current subtitle settings
	 */
	public void setSampleText(String sampleText) {
		mSampleText = sampleText;
	}
	
	/**
	 * Get view to which the sample text is being rendered
	 * 
	 * @return view to which the sample text is being rendered
	 */
	public View getView() {
		return mView;
	}
	
	/**
	 * Set view to which the sample text is being rendered
	 * 
	 * @param view to which the sample text is being rendered
	 */
	public void setView(View view) {
		mView = view;
	}   
	
}
