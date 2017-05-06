package com.visualon.OSMPUtils;

import android.os.Parcel;

public class voOSSEIClockTimeStampImpl implements voOSSEIClockTimeStamp {

    public int getClockTimestampFlag() {
		return mClockTimestampFlag;
	}

	public int getCtType() {
		return mCtType;
	}

	public int getNuitFieldBasedFlag() {
		return mNuitFieldBasedFlag;
	}

	public int getCountingType() {
		return mCountingType;
	}

	public int getFullTimestampFlag() {
		return mFullTimestampFlag;
	}

	public int getDiscontinuityFlag() {
		return mDiscontinuityFlag;
	}

	public int getCntDroppedFlag() {
		return mCntDroppedFlag;
	}

	public int getFrames() {
		return mFrames;
	}

	public int getSecondsValue() {
		return mSecondsValue;
	}

	public int getMinutesValue() {
		return mMinutesValue;
	}

	public int getHoursValue() {
		return mHoursValue;
	}

	public int getSecondsFlag() {
		return mSecondsFlag;
	}

	public int getMinutesFlag() {
		return mMinutesFlag;
	}

	public int getHoursFlag() {
		return mHoursFlag;
	}

	public int getTimeOffset() {
		return mTimeOffset;
	}
	


    /**
	 * @param mClockTimestampFlag
	 * @param mCtType
	 * @param mNuitFieldBasedFlag
	 * @param mCountingType
	 * @param mFullTimestampFlag
	 * @param mDiscontinuityFlag
	 * @param mCntDroppedFlag
	 * @param mFrames
	 * @param mSecondsValue
	 * @param mMinutesValue
	 * @param mHoursValue
	 * @param mSecondsFlag
	 * @param mMinutesFlag
	 * @param mHoursFlag
	 * @param mTimeOffset
	 */
	public voOSSEIClockTimeStampImpl(int mClockTimestampFlag, int mCtType,
			int mNuitFieldBasedFlag, int mCountingType, int mFullTimestampFlag,
			int mDiscontinuityFlag, int mCntDroppedFlag, int mFrames,
			int mSecondsValue, int mMinutesValue, int mHoursValue,
			int mSecondsFlag, int mMinutesFlag, int mHoursFlag, int mTimeOffset) {
		super();
		this.mClockTimestampFlag = mClockTimestampFlag;
		this.mCtType = mCtType;
		this.mNuitFieldBasedFlag = mNuitFieldBasedFlag;
		this.mCountingType = mCountingType;
		this.mFullTimestampFlag = mFullTimestampFlag;
		this.mDiscontinuityFlag = mDiscontinuityFlag;
		this.mCntDroppedFlag = mCntDroppedFlag;
		this.mFrames = mFrames;
		this.mSecondsValue = mSecondsValue;
		this.mMinutesValue = mMinutesValue;
		this.mHoursValue = mHoursValue;
		this.mSecondsFlag = mSecondsFlag;
		this.mMinutesFlag = mMinutesFlag;
		this.mHoursFlag = mHoursFlag;
		this.mTimeOffset = mTimeOffset;
	}



	private int mClockTimestampFlag;
	private int mCtType;
    private int mNuitFieldBasedFlag;
    private int mCountingType;
    private int mFullTimestampFlag;
    private int mDiscontinuityFlag;
    private int mCntDroppedFlag;
    private int mFrames;
    private int mSecondsValue;
    private int mMinutesValue;
    private int mHoursValue;
    private int mSecondsFlag;
    private int mMinutesFlag;
    private int mHoursFlag;
    private int mTimeOffset;
    

}
