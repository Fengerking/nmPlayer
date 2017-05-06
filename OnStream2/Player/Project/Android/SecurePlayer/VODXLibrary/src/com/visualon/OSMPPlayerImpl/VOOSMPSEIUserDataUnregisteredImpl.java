package com.visualon.OSMPPlayerImpl;

import com.visualon.OSMPPlayer.VOOSMPSEIPicTiming;
import com.visualon.OSMPPlayer.VOOSMPSEIUserDataUnregistered;
import com.visualon.OSMPUtils.voOSSEIUserDataImpl;

/**
 *Protocol of User data unregistered SEI message ISO/IEC 14496-10:2005 (E) Annex D 2.6
 */
public class VOOSMPSEIUserDataUnregisteredImpl implements VOOSMPSEIUserDataUnregistered
{

	private static String TAG = "@@@VOOSMPSEIUserDataUnregisteredImpl";
	private voOSSEIUserDataImpl data = null;
    /**
     * getFieldCount
     *
     * @return the number of data fields in the message
     **/
    public int getFieldCount() {
    	if(data==null)
    		return 0;
    	return data.getFieldCount();
	}

    /**
     * getFieldLength
     *
     * @return the length of the i-th data field
     **/
    public int getFieldLength(int i) {
    	if(data==null)
    		return 0;
    	return data.getFieldLength(i);
	}

    /**
     * getDataBuffer
     *
     * @return message data buffer
     **/
    public byte[] getDataBuffer() {
    	if(data==null)
			return null;
    	return data.getDataBuffer();
	}
    
    /**
     * setData
     *
     * to set data
     *
     * @return none
     **/
    public void setData(voOSSEIUserDataImpl obj){
    	data = obj;
    }
}
