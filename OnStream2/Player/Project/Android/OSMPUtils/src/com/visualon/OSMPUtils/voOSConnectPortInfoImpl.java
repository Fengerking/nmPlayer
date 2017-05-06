package com.visualon.OSMPUtils;

public class voOSConnectPortInfoImpl implements voOSConnectPortInfo {
	
	int mAudioConnectPortIndex;
	int mVideoConnectPortIndex;
	

	public voOSConnectPortInfoImpl(int audioID, int videoID)
	{
		mAudioConnectPortIndex = audioID;
		mVideoConnectPortIndex = videoID;
	}
	@Override
	public int AudioConnectPort() {
		// TODO Auto-generated method stub
		return mAudioConnectPortIndex;
	}

	@Override
	public int VideoConnectPort() {
		// TODO Auto-generated method stub
		return mVideoConnectPortIndex;
	}

}
