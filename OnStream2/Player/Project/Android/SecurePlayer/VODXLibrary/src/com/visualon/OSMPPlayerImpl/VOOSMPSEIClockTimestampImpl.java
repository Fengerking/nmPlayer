package com.visualon.OSMPPlayerImpl;

import com.visualon.OSMPPlayer.VOOSMPSEIClockTimestamp;
import com.visualon.OSMPUtils.voLog;
import com.visualon.OSMPUtils.voOSSEIClockTimeStamp;

class VOOSMPSEIClockTimestampImpl implements VOOSMPSEIClockTimestamp {

	private static String TAG = "@@@VOOSMPSEIClockTimestampImpl";
	
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
	    

	public VOOSMPSEIClockTimestampImpl(voOSSEIClockTimeStamp stamp) {
		super();
		this.mClockTimestampFlag = stamp.getClockTimestampFlag();
		this.mCtType = stamp.getCtType();
		this.mNuitFieldBasedFlag = stamp.getNuitFieldBasedFlag();
		this.mCountingType = stamp.getCountingType();
		this.mFullTimestampFlag = stamp.getFullTimestampFlag();
		this.mDiscontinuityFlag = stamp.getDiscontinuityFlag();
		this.mCntDroppedFlag = stamp.getCntDroppedFlag();
		this.mFrames = stamp.getFrames();
		this.mSecondsValue = stamp.getSecondsValue();
		this.mMinutesValue = stamp.getMinutesValue();
		this.mHoursValue = stamp.getHoursValue();
		this.mSecondsFlag = stamp.getSecondsFlag();
		this.mMinutesFlag = stamp.getMinutesFlag();
		this.mHoursFlag = stamp.getHoursFlag();
		this.mTimeOffset = stamp.getTimeOffset();
		
		voLog.v(TAG, "mClockTimestampFlag is " + mClockTimestampFlag
				+ ", mCtType is " + mCtType +  ", mNuitFieldBasedFlag is " + mNuitFieldBasedFlag
				+ ", mCountingType is " + mCountingType + ", mFullTimestampFlag is " +  mFullTimestampFlag + "\r\n"
				+ "mDiscontinuityFlag is " + mDiscontinuityFlag + ", mCntDroppedFlag is " + mCntDroppedFlag
				+ ", mFrames is " + mFrames + ", mSecondsValue is " + mSecondsValue + ", mMinutesValue is " + mMinutesValue + "\r\n"
				+ "mHoursValue is " + mHoursValue + ", mSecondsFlag is " + mSecondsFlag
				+ ", mMinutesFlag is " + mMinutesFlag + ", mHoursFlag is " + mHoursFlag + ", mTimeOffset is " + mTimeOffset);
	}

	@Override
	public int getClockTimestampFlag() {
		// TODO Auto-generated method stub
		return this.mClockTimestampFlag;
	}

	@Override
	public int getCtType() {
		// TODO Auto-generated method stub
		return this.mCtType;
	}

	@Override
	public int getNuitFieldBasedFlag() {
		// TODO Auto-generated method stub
		return this.mNuitFieldBasedFlag;
	}

	@Override
	public int getCountingType() {
		// TODO Auto-generated method stub
		return this.mCountingType;
	}

	@Override
	public int getFullTimestampFlag() {
		// TODO Auto-generated method stub
		return this.mFullTimestampFlag;
	}

	@Override
	public int getDiscontinuityFlag() {
		// TODO Auto-generated method stub
		return this.mDiscontinuityFlag;
	}

	@Override
	public int getCntDroppedFlag() {
		// TODO Auto-generated method stub
		return this.mCntDroppedFlag;
	}

	@Override
	public int getFrames() {
		// TODO Auto-generated method stub
		return this.mFrames;
	}

	@Override
	public int getSecondsValue() {
		// TODO Auto-generated method stub
		return this.mSecondsValue;
	}

	@Override
	public int getMinutesValue() {
		// TODO Auto-generated method stub
		return this.mMinutesValue;
	}

	@Override
	public int getHoursValue() {
		// TODO Auto-generated method stub
		return this.mHoursValue;
	}

	@Override
	public int getSecondsFlag() {
		// TODO Auto-generated method stub
		return this.mSecondsFlag;
	}

	@Override
	public int getMinutesFlag() {
		// TODO Auto-generated method stub
		return this.mMinutesFlag;
	}

	@Override
	public int getHoursFlag() {
		// TODO Auto-generated method stub
		return this.mHoursFlag;
	}

	@Override
	public int getTimeOffset() {
		// TODO Auto-generated method stub
		return this.mTimeOffset;
	}

}
