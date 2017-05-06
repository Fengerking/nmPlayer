/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

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


public class VOOSMPRTSPPort {

    private int mAudioPort;
    private int mVideoPort;

    public VOOSMPRTSPPort(int audioPort, int videoPort) {
        super();
        mAudioPort = audioPort;
        mVideoPort = videoPort;
    }
    /** audio connection port. */
    public int getAudioConnectionPort() {
        // TODO Auto-generated method stub
        return mAudioPort;
    }

    /** video connection port. */
    public int getVideoConnectionPort() {
        // TODO Auto-generated method stub
        return mVideoPort;
    }

}
