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
 * @file voOSDRMInit.java
 * implementation for voOSDRMInit class.
 *
 * 
 *
 *
 * @author  Nimo wu
 * @date    2013-6-21 
 ************************************************************************/
package com.visualon.OSMPUtils;

public class voOSDRMInit {
        
    private int    mDRMDataHandle;
    private byte[] mDRMData;

    /**
     * @param sDRMData
     * @param nDRMSize
     */
    public voOSDRMInit(int nDRMDataHandle, byte[] sDRMData) {
        this.mDRMDataHandle = nDRMDataHandle;
        this.mDRMData = sDRMData;
    }

    public int getDRMInitDataHandle() {
        return mDRMDataHandle;
    }

    public int getDRMInitSize() {
        return (mDRMData == null) ? 0 : mDRMData.length;
    }

    public byte[] getDRMInitData() {
        return mDRMData;
    }
}
