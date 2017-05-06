package com.visualon.OSMPUtils;

/**
 *Protocol of User data unregistered SEI message ISO/IEC 14496-10:2005 (E) Annex D 2.6
 */
public interface voOSSEIUserData {
	
	/**
     * getFieldCount
     *
     * @return the number of data fields in the message
     **/
    public int getFieldCount();

    /**
     * getFieldLength
     *
     * @return the length of the i-th data field
     **/
    public int getFieldLength(int i);

    /**
     * getDataBuffer
     *
     * @return message data buffer
     **/
    public byte[] getDataBuffer();

}
