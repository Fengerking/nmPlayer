package com.visualon.VOOSMPStreamingDownloader;

import android.content.Context;

import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;

public class VOOSMPStreamingDownloaderInitParam {
	
	private Context mContext = null;
	private String mLibPath = null;

	public VOOSMPStreamingDownloaderInitParam() {
	    super();
	}
	
	/**
	* Get application context 
	*
	* @return  application context 
	*/
	public Context getContext() {
	    // TODO Auto-generated method stub
	    return mContext;
	}

	/**
	 * Get library directory path name
	 *
	 * @return  library directory path name
	 */
	public String getLibraryPath() {
	    // TODO Auto-generated method stub
	    return mLibPath;
	}
	
	/**
	 * Set context
	 *
	 * @param context application context
	 *
	 * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	 */
	public VO_OSMP_RETURN_CODE setContext(Context context) {
	    // TODO Auto-generated method stub
	    mContext = context;
	    return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}
	
	/**
	 * Set library path 
	 *
	 * @param  libPath Full directory path name to package library directory (e.g. /data/data/packagename/lib)
	 *
	 * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
	 */
	public VO_OSMP_RETURN_CODE setLibraryPath(String libPath) {
	    // TODO Auto-generated method stub
	    mLibPath = libPath;
	    return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}

}
