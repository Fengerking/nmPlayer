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

import com.visualon.OSMPPlayer.VOOSMPAnalyticsInfo;

class VOOSMPAnalyticsInfoImpl implements VOOSMPAnalyticsInfo {

    private int mLastTime; /* !<Time to look back in */
    private int mSourceDropNum; /* !<Source Drop frame number */
    private int mCodecDropNum; /* !<Codec dropped frame number */
    private int mRenderDropNum; /* !<Render dropped frame number */
    private int mDecodedNum; /* !<Decoded frame number */
    private int mRenderNum; /* !<Rendered frame number */
    private int mSourceTimeNum; /* !<Number source exceeds the time (I / ms) */
    private int mCodecTimeNum; /* !<Number codec exceeds the time (I /ms) */
    private int mRenderTimeNum; /* !<Number render exceeds the time (I /ms) */
    private int mJitterNum; /* !<Number jitter exceeds the time (I /ms) */
    private int mCodecErrorsNum; /* !<Number codec dropped with encounter errors */
    private int mCodecErrors[]; /* !<Codec errors */
    private int mCPULoad; /* !<Current CPU load in percent */
    private int mFrequency; /* !<Current frequency CPU is scaled to */
    private int mMaxFrequency; /* !<Maximum frequency CPU */
    private int mWorstDecodeTime; /* !<Worst codec decode time (ms) */
    private int mWorstRenderTime; /* !<Worst render time (ms) */
    private int mAverageDecodeTime; /* !<Average codec decode time (ms) */
    private int mAverageRenderTime; /* !<Average render time (ms) */
    private int mTotalCPULoad; /* !<Current total CPU load in percent */

    public VOOSMPAnalyticsInfoImpl() {
        super();
        this.mLastTime = 0;
        this.mSourceDropNum = 0;
        this.mCodecDropNum = 0;
        this.mRenderDropNum = 0;
        this.mDecodedNum = 0;
        this.mRenderNum = 0;
        this.mSourceTimeNum = 0;
        this.mCodecTimeNum = 0;
        this.mRenderTimeNum = 0;
        this.mJitterNum = 0;
        this.mCodecErrorsNum = 0;
        this.mCodecErrors = null;
        this.mCPULoad = 0;
        this.mFrequency = 0;
        this.mMaxFrequency = 0;
        this.mWorstDecodeTime = 0;
        this.mWorstRenderTime = 0;
        this.mAverageDecodeTime = 0;
        this.mAverageRenderTime = 0;
        this.mTotalCPULoad = 0;
    }

    @Override
    public int getLastTime() {
        // TODO Auto-generated method stub
        return mLastTime;
    }

    @Override
    public int getSourceDropNum() {
        // TODO Auto-generated method stub
        return mSourceDropNum;
    }

    @Override
    public int getCodecDropNum() {
        // TODO Auto-generated method stub
        return mCodecDropNum;
    }

    @Override
    public int getRenderDropNum() {
        // TODO Auto-generated method stub
        return mRenderDropNum;
    }

    @Override
    public int getDecodedNum() {
        // TODO Auto-generated method stub
        return mDecodedNum;
    }

    @Override
    public int getRenderNum() {
        // TODO Auto-generated method stub
        return mRenderNum;
    }

    @Override
    public int getSourceTimeNum() {
        // TODO Auto-generated method stub
        return mSourceTimeNum;
    }

    @Override
    public int getCodecTimeNum() {
        // TODO Auto-generated method stub
        return mCodecTimeNum;
    }

    @Override
    public int getRenderTimeNum() {
        // TODO Auto-generated method stub
        return mRenderTimeNum;
    }

    @Override
    public int getJitterNum() {
        // TODO Auto-generated method stub
        return mJitterNum;
    }

    @Override
    public int getCodecErrorsNum() {
        // TODO Auto-generated method stub
        return mCodecErrorsNum;
    }

    @Override
    public int[] getCodecErrors() {
        // TODO Auto-generated method stub
        return mCodecErrors;
    }

    @Override
    public int getCPULoad() {
        // TODO Auto-generated method stub
        return mCPULoad;
    }

    @Override
    public int getFrequency() {
        // TODO Auto-generated method stub
        return mFrequency;
    }

    @Override
    public int getMaxFrequency() {
        // TODO Auto-generated method stub
        return mMaxFrequency;
    }

    @Override
    public int getWorstDecodeTime() {
        // TODO Auto-generated method stub
        return mWorstDecodeTime;
    }

    @Override
    public int getWorstRenderTime() {
        // TODO Auto-generated method stub
        return mWorstRenderTime;
    }

    @Override
    public int getAverageDecodeTime() {
        // TODO Auto-generated method stub
        return mAverageDecodeTime;
    }

    @Override
    public int getAverageRenderTime() {
        // TODO Auto-generated method stub
        return mAverageRenderTime;
    }

    @Override
    public int getTotalCPULoad() {
        // TODO Auto-generated method stub
        return mTotalCPULoad;
    }

    /**
     * @param mLastTime
     *            the mLastTime to set
     */
    public void setLastTime(int lastTime) {
        this.mLastTime = lastTime;
    }

    /**
     * @param mSourceDropNum
     *            the mSourceDropNum to set
     */
    public void setSourceDropNum(int sourceDropNum) {
        this.mSourceDropNum = sourceDropNum;
    }

    /**
     * @param mCodecDropNum
     *            the mCodecDropNum to set
     */
    public void setCodecDropNum(int codecDropNum) {
        this.mCodecDropNum = codecDropNum;
    }

    /**
     * @param mRenderDropNum
     *            the mRenderDropNum to set
     */
    public void setRenderDropNum(int renderDropNum) {
        this.mRenderDropNum = renderDropNum;
    }

    /**
     * @param mDecodedNum
     *            the mDecodedNum to set
     */
    public void setDecodedNum(int decodedNum) {
        this.mDecodedNum = decodedNum;
    }

    /**
     * @param mRenderNum
     *            the mRenderNum to set
     */
    public void setRenderNum(int renderNum) {
        this.mRenderNum = renderNum;
    }

    /**
     * @param mSourceTimeNum
     *            the mSourceTimeNum to set
     */
    public void setSourceTimeNum(int sourceTimeNum) {
        this.mSourceTimeNum = sourceTimeNum;
    }

    /**
     * @param mCodecTimeNum
     *            the mCodecTimeNum to set
     */
    public void setCodecTimeNum(int codecTimeNum) {
        this.mCodecTimeNum = codecTimeNum;
    }

    /**
     * @param mRenderTimeNum
     *            the mRenderTimeNum to set
     */
    public void setRenderTimeNum(int renderTimeNum) {
        this.mRenderTimeNum = renderTimeNum;
    }

    /**
     * @param mJitterNum
     *            the mJitterNum to set
     */
    public void setJitterNum(int jitterNum) {
        this.mJitterNum = jitterNum;
    }

    /**
     * @param mCodecErrorsNum
     *            the mCodecErrorsNum to set
     */
    public void setCodecErrorsNum(int codecErrorsNum) {
        this.mCodecErrorsNum = codecErrorsNum;
    }

    /**
     * @param mCodecErrors
     *            the mCodecErrors to set
     */
    public void setCodecErrors(int[] codecErrors) {
        this.mCodecErrors = codecErrors;
    }

    /**
     * @param mCPULoad
     *            the mCPULoad to set
     */
    public void setCPULoad(int CPULoad) {
        this.mCPULoad = CPULoad;
    }

    /**
     * @param mFrequency
     *            the mFrequency to set
     */
    public void setFrequency(int frequency) {
        this.mFrequency = frequency;
    }

    /**
     * @param mMaxFrequency
     *            the mMaxFrequency to set
     */
    public void setMaxFrequency(int maxFrequency) {
        this.mMaxFrequency = maxFrequency;
    }

    /**
     * @param mWorstDecodeTime
     *            the mWorstDecodeTime to set
     */
    public void setWorstDecodeTime(int worstDecodeTime) {
        this.mWorstDecodeTime = worstDecodeTime;
    }

    /**
     * @param mWorstRenderTime
     *            the mWorstRenderTime to set
     */
    public void setWorstRenderTime(int worstRenderTime) {
        this.mWorstRenderTime = worstRenderTime;
    }

    /**
     * @param mAverageDecodeTime
     *            the mAverageDecodeTime to set
     */
    public void setAverageDecodeTime(int averageDecodeTime) {
        this.mAverageDecodeTime = averageDecodeTime;
    }

    /**
     * @param mAverageRenderTime
     *            the mAverageRenderTime to set
     */
    public void setAverageRenderTime(int averageRenderTime) {
        this.mAverageRenderTime = averageRenderTime;
    }

    /**
     * @param mTotalCPULoad
     *            the mTotalCPULoad to set
     */
    public void setTotalCPULoad(int totalCPULoad) {
        this.mTotalCPULoad = totalCPULoad;
    }

}
