package com.visualon.OSMPPlayerImpl;

import com.visualon.OSMPPlayer.VOOSMPBuffer;

class VOOSMPBufferImpl implements VOOSMPBuffer {
    
    protected long   mTimestamp;
    protected int    mBufferSize;
    protected byte[] mBuffer;

    public VOOSMPBufferImpl(long timestamp,int buffersize,byte[] buffer)
    {
        this.mTimestamp = timestamp;
        this.mBufferSize = buffersize;
        this.mBuffer = buffer;
    }
    @Override
    public long getTimestamp() {
        // TODO Auto-generated method stub
        return mTimestamp;
    }

    @Override
    public int getBufferSize() {
        // TODO Auto-generated method stub
        return mBufferSize;
    }

    @Override
    public byte[] getBuffer() {
        // TODO Auto-generated method stub
        return mBuffer;
    }

}
