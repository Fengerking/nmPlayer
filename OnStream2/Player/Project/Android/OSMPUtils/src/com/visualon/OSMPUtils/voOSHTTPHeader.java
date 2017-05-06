package com.visualon.OSMPUtils;

/**
 * Structure of HTTP head
 */
public class voOSHTTPHeader {
	
	/**Name of the header, must not be null */
	private String HeaderName;      
	
	/**Value of the header,must not be null */
	private String HeaderValue;             
	    
	/**Indicates some property of this structure,not used right now */
	private int Flag;              
	
	/**With some special flag, this field may use */
	private Object FlagData;      
	
	
	/**
	 * Constructor voOSHTTPHeader class
	 * 
	 * @param headerName Name of the header, must not be null
	 * 
	 * @param headerValue Value of the header,must not be null
	 * 
	 * @param flag Indicates some property of this structure, not used right now, value is 0 now
	 * 
	 * @param flagData With some special flag, this field may use, value is null now
	 */
	public voOSHTTPHeader(String headerName, String headerValue, int flag, Object flagData) {
		super();
		HeaderName = headerName;
		HeaderValue = headerValue;
		Flag = flag;
		FlagData = flagData;
	}


	/**
	 * @return the headerName
	 */
	public String getHeaderName() {
		return HeaderName;
	}


	/**
	 * @return the headerValue
	 */
	public String getHeaderValue() {
		return HeaderValue;
	}


	/**
	 * @return the flag
	 */
	public int getFlag() {
		return Flag;
	}


	/**
	 * @return the flagData
	 */
	public Object getFlagData() {
		return FlagData;
	}


}
