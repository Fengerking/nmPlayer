package com.visualon.OSMPUtils;

import android.util.Log;



public interface voOSIOFailedDesc {
	
	static final String TAG = "@@@voOSIOFailedDesc";
	
	public enum VOOSMP_SRC_IO_DOWNLOAD_FAIL_REASON
	{
		/**HTTP IO connect failed*/
		VOOSMP_IO_HTTP_CONNECT_FAILED                 (0x00000000),
		
		/**HTTP IO failed to get response or response can't be parsed or response too large*/
		VOOSMP_IO_HTTP_INVALID_RESPONSE               (0x00000001),
		
		/**HTTP IO 4xx error*/
		VOOSMP_IO_HTTP_CLIENT_ERROR                   (0x00000002),
		
		/**HTTP IO 5xx error*/
		VOOSMP_IO_HTTP_SERVER_ERROR                   (0x00000003),		
		
		/**Max value or unknown error*/
	    VOOSMP_IO_HTTP_FAIL_REASON_MAX_VALUE          (0x7FFFFFFF);   
		
		private int value;
		
		VOOSMP_SRC_IO_DOWNLOAD_FAIL_REASON(int event)
		{
			value = event;
		}
		
		public int getValue()
		{
			return value;
		}
		
		public static VOOSMP_SRC_IO_DOWNLOAD_FAIL_REASON valueOf(int value)
    	{
    		for (int i = 0; i < VOOSMP_SRC_IO_DOWNLOAD_FAIL_REASON.values().length; i ++)
    		{
    			if (VOOSMP_SRC_IO_DOWNLOAD_FAIL_REASON.values()[i].getValue() == value)
    				return VOOSMP_SRC_IO_DOWNLOAD_FAIL_REASON.values()[i];
    		}
    		
    		Log.e(TAG, "VO_OSMP_CB_EVENT_ID isn't match. id = " + Integer.toHexString(value));
    		return VOOSMP_IO_HTTP_FAIL_REASON_MAX_VALUE;
    	}
	}
	
	/** related url*/
	public String						getURL();
	
	/**Failed reason*/
	VOOSMP_SRC_IO_DOWNLOAD_FAIL_REASON  getReason();
	
	/**When the reason is VOOSMP_IO_HTTP_CLIENT_ERROR or VOOSMP_IO_HTTP_SERVER_ERROR,it contain The response string, otherwise it will be NULL*/
	String                              getResponse();
	

}
