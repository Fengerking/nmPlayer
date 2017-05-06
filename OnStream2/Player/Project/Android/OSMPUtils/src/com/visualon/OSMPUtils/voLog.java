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
 * @file voLog.java
 * output log and control log output.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/

package com.visualon.OSMPUtils;

import android.util.Log;

public class voLog {
	
	private static final boolean m_bPrintLog = true;

	
	static public void v(String tag, String format, Object... args)
	{
		if (m_bPrintLog)
		{
			String prefix = "";
			String string = format;
			try{
				string = prefix + String.format(format, args);
			}
			catch(Exception e){
			}
			Log.v(tag, string);
		}
	}
	
	static public void i(String tag, String format, Object... args)
	{
		if (m_bPrintLog)
		{
			String prefix = "";
			String string = format;
			try{
				string = prefix + String.format(format, args);
			}
			catch(Exception e){
			}
			Log.i(tag, string);
		}
	}
	
	static public void e(String tag, String format, Object... args)
	{
		if (m_bPrintLog)
		{
			String prefix = "";
			String string = format;
			try{
				string = prefix + String.format(format, args);
			}
			catch(Exception e){
			}
			Log.e(tag, string); 
		}
	}
	
	static public void d(String tag, String format, Object... args)
	{
		if (m_bPrintLog)
		{
			String prefix = "";
			String string = format;
			try{
				string = prefix + String.format(format, args);
			}
			catch(Exception e){
			}
			Log.d(tag, string);
		}
	}
	
	static public void w(String tag, String format, Object... args)
	{
		if (m_bPrintLog)
		{
			String prefix = "";
			String string = format;
			try{
				string = prefix + String.format(format, args);
			}
			catch(Exception e){
			}
			Log.w(tag, string);
		}
	}
	

}
