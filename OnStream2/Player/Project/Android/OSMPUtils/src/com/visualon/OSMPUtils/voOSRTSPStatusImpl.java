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
 * @file voOSRTSPStatusImpl.java
 * implementation for voOSRTSPStatus interface.
 *
 * 
 *
 *
 * @author  Wu Jing
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

import com.visualon.OSMPUtils.voOSType.VOOSMP_SOURCE_STREAMTYPE;

/** 
 * @deprecated this API remove for task 27762 .
 */
public class voOSRTSPStatusImpl implements voOSRTSPStatus {
	VOOSMP_SOURCE_STREAMTYPE nTrackType;
	int uPacketRecved;
	int uPacketDuplicated;
	int uPacketLost;
	int uPacketSent;
	int uAverageJitter;
	int uAverageLatency;

	/**
	 * 
	 */
	public voOSRTSPStatusImpl() {
		super();
	}
	
	/**
	 * @param nTrackType
	 * @param uPacketRecved
	 * @param uPacketDuplicated
	 * @param uPacketLost
	 * @param uPacketSent
	 * @param uAverageJitter
	 * @param uAverageLatency
	 */
	public voOSRTSPStatusImpl(VOOSMP_SOURCE_STREAMTYPE nTrackType, int uPacketRecved, int uPacketDuplicated, 
			int uPacketLost,int uPacketSent, int uAverageJitter, int uAverageLatency) {
		super();
		this.nTrackType = nTrackType;
		this.uPacketRecved = uPacketRecved;
		this.uPacketDuplicated = uPacketDuplicated;
		this.uPacketLost = uPacketLost;
		this.uPacketSent = uPacketSent;
		this.uAverageJitter = uAverageJitter;
		this.uAverageLatency = uAverageLatency;
	}
	
	@Override
	public VOOSMP_SOURCE_STREAMTYPE getTrackType() {
		// TODO Auto-generated method stub
		return nTrackType;
	}

	@Override
	public int getPacketRecved() {
		// TODO Auto-generated method stub
		return uPacketRecved;
	}

	@Override
	public int getPacketDuplicated() {
		// TODO Auto-generated method stub
		return uPacketDuplicated;
	}

	@Override
	public int getPacketLost() {
		// TODO Auto-generated method stub
		return uPacketLost;
	}

	@Override
	public int getPacketSent() {
		// TODO Auto-generated method stub
		return uPacketSent;
	}

	@Override
	public int getAverageJitter() {
		// TODO Auto-generated method stub
		return uAverageJitter;
	}

	@Override
	public int getAverageLatency() {
		// TODO Auto-generated method stub
		return uAverageLatency;
	}

}
