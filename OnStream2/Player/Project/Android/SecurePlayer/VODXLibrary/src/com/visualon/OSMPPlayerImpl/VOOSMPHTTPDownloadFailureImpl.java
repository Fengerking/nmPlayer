/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
 ************************************************************************/
package com.visualon.OSMPPlayerImpl;

import com.visualon.OSMPPlayer.VOOSMPHTTPDownloadFailure.VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON;

class VOOSMPHTTPDownloadFailureImpl implements
        com.visualon.OSMPPlayer.VOOSMPHTTPDownloadFailure {
    
    private VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON mReason;
    private String mUrl;
    private String mResponse;
    
    public VOOSMPHTTPDownloadFailureImpl (){
        this.mReason = VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON.VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON_MAX;
        this.mResponse = null;
        this.mUrl = null;
    }
    
    public VOOSMPHTTPDownloadFailureImpl(VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON reason, String response, String url)
    {
        mReason = reason;
        mResponse = response;
        mUrl = url;
    }
    
    @Override
    public String getURL() {
        // TODO Auto-generated method stub
        return mUrl;
    }
    
    @Override
    public VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON getReason() {
        // TODO Auto-generated method stub
        return mReason;
    }

    @Override
    public String getResponse() {
        // TODO Auto-generated method stub
        return mResponse;
    }

}
