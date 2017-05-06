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
package com.visualon.OSMPPlayer;

public class VOOSMPDRMInit {
    /**
     * DRM Init info
     */
    private int     mDRMDataHandle = 0;
    private byte[]  mDRMData = null;

    /**
     *  @param nDRMDataHandle
     *  @param sDRMData
     */
    public VOOSMPDRMInit(int nDRMDataHandle, byte[] sDRMData) {
        this.mDRMData = sDRMData;
        this.mDRMDataHandle = nDRMDataHandle;
    }

    public int getDRMInitSize() {
        return (mDRMData != null) ? mDRMData.length : 0;
    }
        
    public byte[] getDRMInitData() {
        return mDRMData;
    }

    public int getDRMInitDataHandle() {
        return mDRMDataHandle;
    }
}
                                                                               
