package com.visualon.OSMPUtils;

/**
 * Threshold of bitrate adaptation.
 */
public class voOSSrcBAThreshold {
	
	private int nUpper = 0;
	private int nLower = 0;
	
	/**
	 * Get Source BA threshold upper value
	 * 
	 * @return Source BA threshold upper value
	 */
	public int getUpper()
	{
		return nUpper;
	}
	
	/**
	 * Get Source BA threshold lower value
	 * 
	 * @return Source BA threshold lower value
	 */
	public int getLower()
	{
		return nLower;
	}
	
	/**
	 * Set Source BA threshold upper value
	 * 
	 * @param upper value
	 * 
	 * @return 0 for successful. 
	 */
	public int setUpper(int value)
	{
		nUpper = value;
		return 0;
	}
	
	/**
	 * Set Source BA threshold lower value
	 * 
	 * @param lower value
	 * 
	 * @return 0 for successful. 
	 */
	public int setLower(int value)
	{
		nLower = value;
		return 0;
	}
}
