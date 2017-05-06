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
 * @file voOSCPUInfoImpl.java
 * implementation for voOSCPUInfo interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

public class voOSCPUInfoImpl implements voOSCPUInfo{
	
	int			mCoreCount;
	int			mCPUType;
	int			mFrequency;
	long 		mllReserved;
	
	
	public int			CoreCount()
	{
		return mCoreCount;
	}
	
	
	public int			CPUType()
	{
		return mCPUType;
	}

	
	public int			Frequency()
	{
		return mFrequency;
	}
	
	
	public long			ReservedField()
	{
		return mllReserved;
	}


	/**
	 * @param nCoreCount
	 * @param nCPUType
	 * @param nFrequency
	 * @param nllReserved
	 */
	public voOSCPUInfoImpl(int nCoreCount, int nCPUType, int nFrequency,
			long nllReserved) {
		super();
		this.mCoreCount = nCoreCount;
		this.mCPUType = nCPUType;
		this.mFrequency = nFrequency;
		this.mllReserved = nllReserved;
	}

}
