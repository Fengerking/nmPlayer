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
 * @file voOSVideoPerformanceImpl.java
 * implementation for voOSVideoPerformance interface.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
package com.visualon.OSMPUtils;

import android.os.Parcel;
/** 
 * @deprecated this API remove for task 27762 .
 */
public class voOSVideoPerformanceImpl implements voOSVideoPerformance {
	
	int mLastTime;                  /*!<Time to look back in */
    int mSourceDropNum;             /*!<Source Drop frame number */
    int mCodecDropNum;              /*!<Codec dropped frame number */
    int mRenderDropNum;             /*!<Render dropped frame number */
    int mDecodedNum;                /*!<Decoded frame number */
    int mRenderNum;                 /*!<Rendered frame number */
    int mSourceTimeNum;             /*!<Number source exceeds the time (I / ms) */
    int mCodecTimeNum;              /*!<Number codec exceeds the time  (I /ms) */
    int mRenderTimeNum;             /*!<Number render exceeds the time (I /ms) */
    int mJitterNum;                 /*!<Number jitter exceeds the time (I /ms) */
    int mCodecErrorsNum;            /*!<Number codec dropped with encounter errors */
    int mCodecErrors[];             /*!<Codec errors */
    int mCPULoad;                   /*!<Current CPU load in percent */
    int mFrequency;                 /*!<Current frequency CPU is scaled to */
    int mMaxFrequency;              /*!<Maximum frequency CPU */
    int mWorstDecodeTime;           /*!<Worst codec decode time (ms) */
    int mWorstRenderTime;           /*!<Worst render time (ms) */
    int mAverageDecodeTime;         /*!<Average codec decode time (ms) */
	int mAverageRenderTime;         /*!<Average render time (ms) */
	int mTotalCPULoad;				/*!<Current total CPU load in percent */
    int mTotalPlaybackDuration;     /*!< The playback duration from the beginning of playback to the time analytics is queried */
    int mTotalSourceDropNum;        /*!< Source dropped frame number from the beginning of playback to the time analytics is queried */
    int mTotalCodecDropNum;         /*!< Codec dropped frame number from the beginning of playback to the time analytics is queried */
    int mTotalRenderDropNum;        /*!< Render dropped frame number from the beginning of playback to the time analytics is queried */
    int mTotalDecodedNum;           /*!< Decoded frame number from the beginning of playback to the time analytics is queried */
    int mTotalRenderNum;            /*!< Rendered frame number from the beginning of playback to the time analytics is queried */

	
	public voOSVideoPerformanceImpl() {
		super();
	}
	
	public void parser(Parcel parcel)
	{
		this.mLastTime = parcel.readInt();
		this.mSourceDropNum = parcel.readInt();
		this.mCodecDropNum =parcel.readInt();
		this.mRenderDropNum = parcel.readInt();
		this.mDecodedNum = parcel.readInt();
		this.mRenderNum =parcel.readInt();
		this.mSourceTimeNum = parcel.readInt();;
		this.mCodecTimeNum = parcel.readInt();
		this.mRenderTimeNum = parcel.readInt();
		this.mJitterNum = parcel.readInt();
		this.mCodecErrorsNum = parcel.readInt();
		this.mCodecErrors = new int[mCodecErrorsNum];
		for (int i = 0; i < mCodecErrorsNum; i++)
		{
			this.mCodecErrors[i] = parcel.readInt();
		}
		this.mCPULoad = parcel.readInt();
		this.mFrequency = parcel.readInt();
		this.mMaxFrequency = parcel.readInt();
		this.mWorstDecodeTime = parcel.readInt();
		this.mWorstRenderTime = parcel.readInt();
		this.mAverageDecodeTime = parcel.readInt();
		this.mAverageRenderTime = parcel.readInt();
		this.mTotalCPULoad = parcel.readInt();
		this.mTotalPlaybackDuration = parcel.readInt();
		this.mTotalSourceDropNum = parcel.readInt();
		this.mTotalCodecDropNum = parcel.readInt();
		this.mTotalRenderDropNum = parcel.readInt();
		this.mTotalDecodedNum = parcel.readInt();
		this.mTotalRenderNum = parcel.readInt();
	}
	
    /**
	 * @param mLastTime
	 * @param mSourceDropNum
	 * @param mCodecDropNum
	 * @param mRenderDropNum
	 * @param mDecodedNum
	 * @param mRenderNum
	 * @param mSourceTimeNum
	 * @param mCodecTimeNum
	 * @param mRenderTimeNum
	 * @param mJitterNum
	 * @param mCodecErrorsNum
	 * @param mCodecErrors
	 * @param mCPULoad
	 * @param mFrequency
	 * @param mMaxFrequency
	 * @param mWorstDecodeTime
	 * @param mWorstRenderTime
	 * @param mAverageDecodeTime
	 * @param mAverageRenderTime
	 * @param mTotalCPULoad
	 * @param mTotalPlaybackDuration
	 * @param mTotalSourceDropNum
	 * @param mTotalCodecDropNum
	 * @param mTotalRenderDropNum
	 * @param mTotalDecodedNum
	 * @param mTotalRenderNum
	 */
	public voOSVideoPerformanceImpl(int mLastTime, int mSourceDropNum,
			int mCodecDropNum, int mRenderDropNum, int mDecodedNum,
			int mRenderNum, int mSourceTimeNum, int mCodecTimeNum,
			int mRenderTimeNum, int mJitterNum, int mCodecErrorsNum,
			int[] mCodecErrors, int mCPULoad, int mFrequency, int mMaxFrequency,
			int mWorstDecodeTime, int mWorstRenderTime, int mAverageDecodeTime,
			int mAverageRenderTime, int mTotalCPULoad,
                        int mTotalPlaybackDuration, int mTotalSourceDropNum,
                        int mTotalCodecDropNum, int mTotalRenderDropNum,
                        int mTotalDecodedNum, int mTotalRenderNum) {
		super();
		this.mLastTime = mLastTime;
		this.mSourceDropNum = mSourceDropNum;
		this.mCodecDropNum = mCodecDropNum;
		this.mRenderDropNum = mRenderDropNum;
		this.mDecodedNum = mDecodedNum;
		this.mRenderNum = mRenderNum;
		this.mSourceTimeNum = mSourceTimeNum;
		this.mCodecTimeNum = mCodecTimeNum;
		this.mRenderTimeNum = mRenderTimeNum;
		this.mJitterNum = mJitterNum;
		this.mCodecErrorsNum = mCodecErrorsNum;
		this.mCodecErrors = mCodecErrors;
		this.mCPULoad = mCPULoad;
		this.mFrequency = mFrequency;
		this.mMaxFrequency = mMaxFrequency;
		this.mWorstDecodeTime = mWorstDecodeTime;
		this.mWorstRenderTime = mWorstRenderTime;
		this.mAverageDecodeTime = mAverageDecodeTime;
		this.mAverageRenderTime = mAverageRenderTime;
		this.mTotalCPULoad = mTotalCPULoad;
	}

	public int LastTime() {
		// TODO Auto-generated method stub
		return mLastTime;
	}

	public int SourceDropNum() {
		// TODO Auto-generated method stub
		return mSourceDropNum;
	}

	public int CodecDropNum() {
		// TODO Auto-generated method stub
		return mCodecDropNum;
	}

	public int RenderDropNum() {
		// TODO Auto-generated method stub
		return mRenderDropNum;
	}

	public int DecodedNum() {
		// TODO Auto-generated method stub
		return mDecodedNum;
	}

	public int RenderNum() {
		// TODO Auto-generated method stub
		return mRenderNum;
	}

	public int SourceTimeNum() {
		// TODO Auto-generated method stub
		return mSourceTimeNum;
	}

	public int CodecTimeNum() {
		// TODO Auto-generated method stub
		return mCodecTimeNum;
	}

	public int RenderTimeNum() {
		// TODO Auto-generated method stub
		return mRenderTimeNum;
	}

	public int JitterNum() {
		// TODO Auto-generated method stub
		return mJitterNum;
	}

	public int CodecErrorsNum() {
		// TODO Auto-generated method stub
		return mCodecErrorsNum;
	}

	public int[] CodecErrors() {
		// TODO Auto-generated method stub
		return mCodecErrors;
	}

	public int CPULoad() {
		// TODO Auto-generated method stub
		return mCPULoad;
	}

	public int Frequency() {
		// TODO Auto-generated method stub
		return mFrequency;
	}

	public int MaxFrequency() {
		// TODO Auto-generated method stub
		return mMaxFrequency;
	}

	public int WorstDecodeTime() {
		// TODO Auto-generated method stub
		return mWorstDecodeTime;
	}

	public int WorstRenderTime() {
		// TODO Auto-generated method stub
		return mWorstRenderTime;
	}

	public int AverageDecodeTime() {
		// TODO Auto-generated method stub
		return mAverageDecodeTime;
	}

	public int AverageRenderTime() {
		// TODO Auto-generated method stub
		return mAverageRenderTime;
	}

	public int TotalCPULoad() {
		// TODO Auto-generated method stub
		return mTotalCPULoad;
	}

	public int TotalPlaybackDuration() {
		// TODO Auto-generated method stub
		return mTotalPlaybackDuration;
	}

	public int TotalSourceDropNum() {
		// TODO Auto-generated method stub
		return mTotalSourceDropNum;
	}

	public int TotalCodecDropNum() {
		// TODO Auto-generated method stub
		return mTotalCodecDropNum;
	}

	public int TotalRenderDropNum() {
		// TODO Auto-generated method stub
		return mTotalRenderDropNum;
	}

	public int TotalDecodedNum() {
		// TODO Auto-generated method stub
		return mTotalDecodedNum;
	}

	public int TotalRenderNum() {
		// TODO Auto-generated method stub
		return mTotalRenderNum;
	}
}
