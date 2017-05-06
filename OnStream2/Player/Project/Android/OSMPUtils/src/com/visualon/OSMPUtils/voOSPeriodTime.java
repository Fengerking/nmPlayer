package com.visualon.OSMPUtils;

public class voOSPeriodTime {
	private int mPeriodSequenceNumber;
	private long mTimeStamp;
	
	public void setPeriodSequenceNumber(int periodSequenceNumber)
	{
		mPeriodSequenceNumber = periodSequenceNumber;
	}

	public void setTimeStamp(long timeStamp)
	{
		mTimeStamp = timeStamp;
	}

	public int getPeriodSequenceNumber()
	{
		return mPeriodSequenceNumber;
	}

	public long getTimeStamp()
	{
		return mTimeStamp;
	}
}
