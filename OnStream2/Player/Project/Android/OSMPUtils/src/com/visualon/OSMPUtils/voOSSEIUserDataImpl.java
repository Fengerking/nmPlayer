package com.visualon.OSMPUtils;

import android.os.Parcel;

/**
 *Protocol of User data unregistered SEI message ISO/IEC 14496-10:2005 (E) Annex D 2.6
 */
public class voOSSEIUserDataImpl implements voOSSEIUserData  {
	
	private static String TAG = "@@@voOSSEIUserDataImpl";
	/**
	  * Structure of SEI user data
	  */

	private int      nCount; /* User data count*/
	private int []   nSize; /* User data size for each field*/ 
	private byte []  pBuffer; /* User data buffer*/
	 	
	/**
     * getFieldCount
     *
     * @return the number of data fields in the message
     **/
    public int getFieldCount() {
		return nCount;
	}

    /**
     * getFieldLength
     *
     * @return the length of the i-th data field
     **/
    public int getFieldLength(int i) {
    	if(nSize == null)
    		return 0;
    	if(nSize.length<=i)
    		return 0;
		return nSize[i];
	}

    /**
     * getDataBuffer
     *
     * @return message data buffer
     **/
    public byte[] getDataBuffer() {
		return pBuffer;
	}
    
    /**
     * parse
     * 
     * parse raw data to member data
     *
     * @return no
     **/
    public void parse(Parcel parc){
    	parc.setDataPosition(0);
    	nCount = parc.readInt();
    	nSize = new int[nCount>0?nCount:1];
    	nSize[0]=0;
    	int nAll = 0;
    	for(int i = 0;i< nCount;i++){
    		nSize[i]=parc.readInt();
    		nAll+=nSize[i];
    	}
    	if(nAll<=0)
    		return;
    	pBuffer = new byte[nAll];
    	parc.readByteArray(pBuffer);
    }

}
