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
import com.visualon.OSMPPlayer.VOOSMPOpenParam;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;

public class VOOSMPInitParam{

    private Object mContext = null;
    private String mLibPath = null;
    private long mFileSize = 0;

    public VOOSMPInitParam() {
        super();
    }

    /**
    * Get application context 
    *
    * @return  application context 
    */
    public Object getContext() {
        // TODO Auto-generated method stub
        return mContext;
    }
    
    /**
     * Get library directory path name
     *
     * @return  library directory path name
     */
    public String getLibraryPath() {
        // TODO Auto-generated method stub
        return mLibPath;
    }

    
    /**
     * Set context
     *
     * @param context application context
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    public VO_OSMP_RETURN_CODE setContext(Object context) {
        // TODO Auto-generated method stub
        mContext = context;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    /**
     * Set library path 
     *
     * @param  libPath Full directory path name to package library directory (e.g. /data/data/packagename/lib)
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    public VO_OSMP_RETURN_CODE setLibraryPath(String libPath) {
        // TODO Auto-generated method stub
        mLibPath = libPath;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    /**
     * Get actual file size
     *
     * @return  file size
     *
     * @deprecated This function is moved to {@link com.visualon.OSMPPlayer.VOOSMPOpenParam#getFileSize()}.
     */
   @Deprecated
    public long getFileSize() {
        // TODO Auto-generated method stub
        return mFileSize;
    }

   /**
    * Set actual file size 
    *
    * @param fileSize File size
    *
    * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
    *
    * @deprecated This function is moved to {@link com.visualon.OSMPPlayer.VOOSMPOpenParam#setFileSize(long)}.
    */
   @Deprecated
    public VO_OSMP_RETURN_CODE setFileSize(long fileSize) {
        // TODO Auto-generated method stub
        mFileSize = fileSize;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

}

