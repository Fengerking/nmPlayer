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
package com.visualon.OSMPPlayerImpl;

import com.visualon.OSMPUtils.voLog;


class voOSMPDRM {
    private String TAG = "@@@voOSMPDRM";
    
    public voOSMPDRM()
    { 
        super();
    }
    public  int creatDRM(Integer pHandle, String packagePath,String libName)
    {
        voLog.i(TAG, " Creat DRM handle by lib name "+libName);
        return nativeCreatDRM( pHandle,packagePath,libName);
    }
    
    public  int destoryDRM(Integer pHandle)
    {
        voLog.i(TAG, " destory DRM handle %d  "+pHandle);
        return nativeDestoryDRM( pHandle);
    }
    

        
    public native int nativeCreatDRM(Integer pHandle, String packagePath, String libName);
    public native int nativeDestoryDRM(Integer pHandle);
    
static {
        
        try {
            System.loadLibrary("vodl");
            System.loadLibrary("voDRMWrapJni_OSMP");
        } catch (UnsatisfiedLinkError e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
}

}
