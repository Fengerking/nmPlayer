package com.visualon.VOOSMPStreamingDownloader;

import com.visualon.OSMPPlayer.VOCommonPlayerAssetSelection;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;

public interface VOOSMPStreamingDownloader extends VOCommonPlayerAssetSelection{
	
	/**
	 * Initialize a Streaming Downloader instance.
	 * <pre>This function must be called first to start a session.</pre>
	 *
	 * @param   listener  [in] Refer to {@link VOOSMPStreamingDownloaderListener}.
	 * @param   initParam [in] Refer to {@link VOOSMPStreamingDownloaderInitParam}.
	 * @return  player object if successful; nil if unsuccessful
	 */
    public VO_OSMP_RETURN_CODE init (VOOSMPStreamingDownloaderListener listener, 
    		VOOSMPStreamingDownloaderInitParam initParam );
    
    /**
     * Destroy a Streaming Downloader instance.
     * 
     * <pre>
     * This function must be called last to close a session.
     * </pre>
     * 
     * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful .
     */
    VO_OSMP_RETURN_CODE destroy();
    
    /**
     * Open media source.
     *
     * @param   url             [in] Source file description (e.g. an URL or a file descriptor, etc.)
     * @param   flag            [in] Flag for opening media source. 
     * @param   localDir        [in] Directory where to save content files.
     *
     * @return {@link VO_OSMP_ERR_NONE} if successful
     */
    public VO_OSMP_RETURN_CODE open (String source, int flag, String localDir);
    
    /**
     * Close media source.
     *
     * @return    {@link VO_OSMP_ERR_NONE} if successful
     */
    public VO_OSMP_RETURN_CODE close ();
    
    /**
     * Start content download.
     *
     * @return    {@link VO_OSMP_ERR_NONE} if successful
     */
    public VO_OSMP_RETURN_CODE start ();
    
    /**
     * Stop content download.
     *
     * @return    {@link VO_OSMP_ERR_NONE} if successful
     */
    public VO_OSMP_RETURN_CODE stop ();
    
    /**
     * Pause content download.
     *
     * @return    {@link VO_OSMP_ERR_NONE} if successful
     */
    public VO_OSMP_RETURN_CODE pause ();
    
    /**
     * Resume content download.
     *
     * @return    {@link VO_OSMP_ERR_NONE} if successful
     */
    public VO_OSMP_RETURN_CODE resume ();

}
