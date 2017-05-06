package com.visualon.OSMPPlayerImpl;

import com.visualon.OSMPPlayer.VOOSMPRTSPStatistics;
import com.visualon.OSMPUtils.voOSRTSPStatus;

class VOOSMPRTSPStatisticsImpl implements VOOSMPRTSPStatistics {
    
    voOSRTSPStatus mRtspStatistics;

    /**
     * 
     */
    public VOOSMPRTSPStatisticsImpl() {
        super();
    }
    
    /**
     * 
     */
    public VOOSMPRTSPStatisticsImpl( voOSRTSPStatus rs) {
        super();
        mRtspStatistics = rs;
    }
    @Override
    public int getPacketReceived() {
        // TODO Auto-generated method stub
        return mRtspStatistics.getPacketRecved();
    }

    @Override
    public int getPacketDuplicated() {
        // TODO Auto-generated method stub
        return mRtspStatistics.getPacketDuplicated();
    }

    @Override
    public int getPacketLost() {
        // TODO Auto-generated method stub
        return mRtspStatistics.getPacketLost();
    }

    @Override
    public int getPacketSent() {
        // TODO Auto-generated method stub
        return mRtspStatistics.getPacketSent();
    }

    @Override
    public int getAverageJitter() {
        // TODO Auto-generated method stub
        return mRtspStatistics.getAverageJitter();
    }

    @Override
    public int getAverageLatency() {
        // TODO Auto-generated method stub
        return mRtspStatistics.getAverageLatency();
    }

}
