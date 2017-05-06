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
 * @file voOSOption.java
 * define global option.
 * Encapsulate a single player option
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

import java.util.List;

/** 
 * Encapsulate a single player option
 */
public class voOSOption {
	/** 
	 * voPlayer options
	 */
	public enum eVoOption {
		eoNone,
		eoVideoColor,
		eoVideoRender,
		eoVideoDecode,
		eoAudioDecode,
		eoVideoEffect,
		eoVideoCloseCaption,
		eoThreadCount,
		eoAudioAnimation,
		eoSocketType,
		eoThirdLibOp,
		eoNotLoadLibrary,
		eoHLSBitrate,
		eoLoop
	}



	/** 
	 * eoVideoRender - use native surface rendering or ToolChain render under Froyo
	 */
	public final static long OPTION_RENDERTYPE_NATIVE = 1;
	/** 
	 * eoVideoRender - use open GL rendering
	 */
	public final static long OPTION_RENDERTYPE_OPENGL = 4;
	/** 
	 * eoVideoDecode - disable deblocking algorithm
	 */
	public final static long OPTION_DECODE_DISABLE_DEBLOCK = 1;
	/** 
	 * eoAudioDecode - use Dolby audio decoder
	 */
	public final static long OPTION_DECODE_DOLBY = 1;

	/** 
	 * eoVideoEffect - use video effect
	 */
	public final static long OPTION_VIDEO_EFFECT_CLOSE_CAPTION = 1;
	/** 
	 * eoVideoCloseCaption - use video effect close caption
	 */
	public final static long OPTION_VIDEO_EFFECT_CLOSE_CAPTION_ON = 1;

	/** 
	 * The type of the option
	 */
	eVoOption	mType = eVoOption.eoNone;
	/** 
	 * The value of the option
	 */
	long		mValue = 0;
	
	/** 
	 * Don't use this constructor directly. Use the make instead
	 * @see voOSOption#make
	 */
	public voOSOption(eVoOption type, long value) {
		mType = type;
		mValue = value;
	}
	
	/** 
	 * Make an option and check if the value is valid
	 */
	public static voOSOption make(eVoOption type, long value) throws Exception {
		
		voOSOption option = new voOSOption(type, value);
		return option;
	}
	
	public eVoOption getType() {
		return mType;
	}
	
	public long getValue() {
		return mValue;
	}
	
	/** 
	 * Unimplemented - read options from a file
	 */
	public List<voOSOption> readConfig(String path) {
		return null;
	}
}
