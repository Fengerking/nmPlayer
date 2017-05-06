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

import com.visualon.OSMPDataSource.voOSDataSource;
import com.visualon.OSMPPlayer.VOCommonPlayerAssetSelection.VOOSMPAssetIndex;
import com.visualon.OSMPUtils.voLog;

public class VOOSMPAssetIndexImpl implements VOOSMPAssetIndex {
	
	private final static String TAG = "@@@VOOSMPAssetIndexImpl";
    private int mVideoIndex;
    private int mAudioIndex;
    private int mSubtitleIndex;

    public VOOSMPAssetIndexImpl() {
        mVideoIndex = -1;
        mAudioIndex = -1;
        mSubtitleIndex = -1;
    }
    
    public VOOSMPAssetIndexImpl(int[] index) {
    	if (index == null || index.length < 3)
    	{
    		voLog.e(TAG, "AssetIndex info is invalid.");
    		mVideoIndex = -1;
            mAudioIndex = -1;
            mSubtitleIndex = -1;
    		return;
    	}
        mVideoIndex = index[0];
        mAudioIndex = index[1];
        mSubtitleIndex = index[2];
    }

    public void fillSelectionAssetsIndex(voOSDataSource source) {
        Integer vIndex, aIndex, sIndex;
        vIndex = new Integer(-1);
        aIndex = new Integer(-1);
        sIndex = new Integer(-1);

        source.GetCurrTrackSelection(vIndex, aIndex, sIndex);
        mVideoIndex = vIndex;
        mAudioIndex = aIndex;
        mSubtitleIndex = sIndex;
    }

    public void fillPlayingAssetsIndex(voOSDataSource source) {
        Integer vIndex, aIndex, sIndex;
        vIndex = new Integer(-1);
        aIndex = new Integer(-1);
        sIndex = new Integer(-1);

        source.GetCurrTrackPlaying(vIndex, aIndex, sIndex);
        mVideoIndex = vIndex;
        mAudioIndex = aIndex;
        mSubtitleIndex = sIndex;
    }

    public int getVideoIndex() {
        // TODO Auto-generated method stub
        return mVideoIndex;
    }

    public int getAudioIndex() {
        // TODO Auto-generated method stub
        return mAudioIndex;
    }

    public int getSubtitleIndex() {
        // TODO Auto-generated method stub
        return mSubtitleIndex;
    }

}
