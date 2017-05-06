package com.visualon.OSMPPlayerImpl;

import com.visualon.OSMPPlayer.VOOSMPSEIClockTimestamp;
import com.visualon.OSMPPlayer.VOOSMPSEIPicTiming;
import com.visualon.OSMPUtils.voLog;
import com.visualon.OSMPUtils.voOSSEIClockTimeStampImpl;
import com.visualon.OSMPUtils.voOSSEIPicTiming;

class VOOSMPSEIPicTimingImpl implements VOOSMPSEIPicTiming {
	
	private static String TAG = "@@@VOOSMPSEIPicTimingImpl";
	private int mCpbDpbDelaysPresentFlag;
	private int mCpbRemovalDelay;
	private int mDpbOutputDelay;
	private int mPictureStructurePresentFlag;
	private int mPictureStructure;
	private int mNumClockTs;
	private VOOSMPSEIClockTimestampImpl[] mOSClockArr = null;//new voOSClockTimeStamp[3];
	

	/**
	 * @param mCpbDpbDelaysPresentFlag
	 * @param mCpbRemovalDelay
	 * @param mDpbOutputDelay
	 * @param mPictureStructurePresentFlag
	 * @param mPictureStructure
	 * @param mNumClockTs
	 * @param mOSClockArr
	 */
	public VOOSMPSEIPicTimingImpl(voOSSEIPicTiming picTiming) {
		super();
		this.mCpbDpbDelaysPresentFlag = picTiming.getCpbDpbDelaysPresentFlag();
		this.mCpbRemovalDelay = picTiming.getCpbRemovalDelay();
		this.mDpbOutputDelay = picTiming.getDpbOutputDelay();
		this.mPictureStructurePresentFlag = picTiming.getPictureStructurePresentFlag();
		this.mPictureStructure = picTiming.getPictureStructure();
		this.mNumClockTs = picTiming.getNumClockTs();
		
		voLog.v(TAG, "SEI INFO VOOSMPSEIPicTiming CpbDpbDelaysPresentFlag is " + mCpbDpbDelaysPresentFlag + ", CpbRemovalDelay is " + mCpbRemovalDelay
				+ ", DpbOutputDelay is " + mDpbOutputDelay + ", mPictureStructurePresentFlag is " + mPictureStructurePresentFlag
				+ ", PictureStructure is " + mPictureStructure + ", NumClockTs is " + mNumClockTs);
		
		voLog.v(TAG, "First VOOSMPSEIClockTimestamp");
		VOOSMPSEIClockTimestampImpl impl0 = new VOOSMPSEIClockTimestampImpl(picTiming.getClock()[0]);
		voLog.v(TAG, "Second VOOSMPSEIClockTimestamp");
		VOOSMPSEIClockTimestampImpl impl1 = new VOOSMPSEIClockTimestampImpl(picTiming.getClock()[1]);
		voLog.v(TAG, "Third VOOSMPSEIClockTimestamp");
		VOOSMPSEIClockTimestampImpl impl2 = new VOOSMPSEIClockTimestampImpl(picTiming.getClock()[2]);
		voLog.v(TAG, "SEI INFO End");
		this.mOSClockArr = new VOOSMPSEIClockTimestampImpl[]{impl0, impl1, impl2};
	}

	@Override
	public int getCpbDpbDelaysPresentFlag() {
		// TODO Auto-generated method stub
		return this.mCpbDpbDelaysPresentFlag;
	}

	@Override
	public int getCpbRemovalDelay() {
		// TODO Auto-generated method stub
		return this.mCpbRemovalDelay;
	}

	@Override
	public int getDpbOutputDelay() {
		// TODO Auto-generated method stub
		return this.mDpbOutputDelay;
	}

	@Override
	public int getPictureStructurePresentFlag() {
		// TODO Auto-generated method stub
		return this.mPictureStructurePresentFlag;
	}

	@Override
	public int getPictureStructure() {
		// TODO Auto-generated method stub
		return this.mPictureStructure;
	}

	@Override
	public int getNumClockTs() {
		// TODO Auto-generated method stub
		return this.mNumClockTs;
	}

	@Override
	public VOOSMPSEIClockTimestamp[] getClock() {
		// TODO Auto-generated method stub
		return this.mOSClockArr;
	}

}
