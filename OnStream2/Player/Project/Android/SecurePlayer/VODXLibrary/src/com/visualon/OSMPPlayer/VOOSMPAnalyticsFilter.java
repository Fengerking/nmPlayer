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

import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;

public class VOOSMPAnalyticsFilter {
   
    private int mLastTime;
    private int mSourceTime;
    private int mCodecTime;
    private int mRenderTime;
    private int mJitter;
   
    /**
     *
     */
    public VOOSMPAnalyticsFilter() {
        super();
        mLastTime = 0;
        mSourceTime = 0;
        mCodecTime = 0;
        mRenderTime = 0;
        mJitter = 0;
    }


    /**
     * Init filter with values
     *
     * @param lastTime   [in] How far back to report (seconds).
     * @param sourceTime [in] Number of instances that will be recorded when the source exceeds this time (ms).
     * @param codecTime  [in] Number of instances that will be recorded when the codec exceeds this time (ms).
     * @param renderTime [in] Number of instances that will be recorded when the render exceeds this time (ms).
     * @param jitter     [in] Number of instances that will be recorded when the jitter exceeds this time (ms).
     *
     */
    public VOOSMPAnalyticsFilter(int lastTime, int sourceTime, int codecTime,
            int renderTime, int jitter) {
        super();
        mLastTime = lastTime;
        mSourceTime = sourceTime;
        mCodecTime = codecTime;
        mRenderTime = renderTime;
        mJitter = jitter;
    }


    /**
     * Set how far back to report.
     *
     * @param   lastTime [in] How far back to report (seconds).
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    public VO_OSMP_RETURN_CODE setLastTime(int lastTime)
    {
        mLastTime = lastTime;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }
   
   
    /**
     * Set number of instances that will be recorded when the source exceeds this time (ms).
     *
     * @param   sourceTime [in] Number of instances that will be recorded when the source exceeds this time (ms).
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    public VO_OSMP_RETURN_CODE setSourceTime(int sourceTime)
    {
        mSourceTime = sourceTime;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }
   
    /**
     * Set number of instances that will be recorded when the codec exceeds this time (ms).
     *
     * @param   codecTime [in] Number of instances that will be recorded when the codec exceeds this time (ms).
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    public VO_OSMP_RETURN_CODE setCodecTime(int codecTime)
    {
        mCodecTime = codecTime;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }
   
    /**
     * Set number of instances that will be recorded when the render exceeds this time (ms).
     *
     * @param   renderTime [in] Number of instances that will be recorded when the render exceeds this time (ms).
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    public VO_OSMP_RETURN_CODE setRenderTime(int renderTime)
    {
        mRenderTime = renderTime;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }
   
    /**
     * Set number of instances that will be recorded when the jitter exceeds this time (ms).
     *
     * @param   jitter [in] Number of instances that will be recorded when the jitter exceeds this time (ms).
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    public VO_OSMP_RETURN_CODE setJitterTime(int jitter)
    {
        mJitter = jitter;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }
   
     /** Get how far back to report
     *
     * @return How far back to report (seconds).
     */
    public int getLastTime()
    {
        return mLastTime;
    }
   
   
    /**
     * Get number of instances that will be recorded when the source exceeds this time (ms).
     *
     * @return Number of instances that will be recorded when the source exceeds this time (ms).
     */
    public int getSourceTime()
    {
        return mSourceTime;
    }
   
    /**
     * Get number of instances that will be recorded when the codec exceeds this time (ms).
     *
     * @return Number of instances that will be recorded when the codec exceeds this time (ms).
     */

    public int getCodecTime()
    {
        return mCodecTime;
    }
   
    /**
     * Get number of instances that will be recorded when the render exceeds this time (ms).
     *
     * @return Number of instances that will be recorded when the render exceeds this time (ms).
     */
    public int getRenderTime()
    {
        return mRenderTime;
    }
   
    /**
     * Get number of instances that will be recorded when the jitter exceeds this time (ms).
     *
     * @return Number of instances that will be recorded when the jitter exceeds this time (ms).
     *
     */
    public int getJitterTime()
    {
        return mJitter;
    }
}
