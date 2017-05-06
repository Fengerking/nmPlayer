package com.visualon.VOOSMPStreamingDownloaderImpl;

import com.visualon.VOOSMPStreamingDownloader.VOOSMPStreamingDownloaderListener.VOOSMPStreamingDownloaderProgressInfo;

public class VOOSMPStreamingDownloaderProgressInfoImpl implements
		VOOSMPStreamingDownloaderProgressInfo {
	
	int mCurrentDuration = 0;
	int mTotalDuration = 0;
	VOOSMPStreamingDownloaderProgressInfoImpl(int curDuration, int totalDuration)
	{
		mCurrentDuration = curDuration;
		mTotalDuration = totalDuration;
	}

	@Override
	public int getDownloadedStreamDuration() {
		// TODO Auto-generated method stub
		return mCurrentDuration;
	}

	@Override
	public int getTotalStreamDuration() {
		// TODO Auto-generated method stub
		return mTotalDuration;
	}

}
