/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

package com.visualon.OSMPPlayer;

import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_LANGUAGE_TYPE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;

public interface VOCommonPlayerAssetSelection {
   
/**
 * This index indicates auto-select for video track.
 */
public static final int VO_OSMP_ASSET_AUTO_SELECTED  = -1;   

/**
 * This index indicates not track be selected.
 */
public static final int VO_OSMP_ASSET_NOT_SELECTED   = -2;
  
    /**
     * Get number of available video tracks
     *
     * <pre>
     * A video track is defined as a combination of
     * bit rate and camera angle (merge of internal stream
     * and video track).
     * </pre>
     *
     * @return  Number of available video tracks; -1 if get failed.
     */          
    int getVideoCount();

    /**
     * Get number of available audio tracks
     * <pre>
     * An audio track is defined as a combination of
     * language and codec type. An audio track may have
     * multiple audio bit rates. The final playback bit rate
     * depends on selected video track.
     * </pre>
     *  
     * @return Number of available audio tracks; -1 if get failed.
     */
    int getAudioCount();

    /**
     * Get number of available subtitle tracks
     *
     * @return Number of available subtitle tracks; -1 if get failed.
     */
    int getSubtitleCount();
   

    /**
     * Select video track by its index.
     *
     * @param     index [in] Index of video track. Valid from 0 to {@link #getVideoCount} - 1.
     *            The availability of video tracks depends on current selection of
     *            uncommitted audio and subtitle tracks.
     *
     * @return    {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE selectVideo(int index);

    /**
     * Select audio track by its index.
     *
     * @param     index [in] Index of audio track. Valid from 0 to {@link #getAudioCount} - 1.
     *            The availability of audio tracks depends on current selection of
     *            uncommitted video and subtitle tracks.
     *
     * @return    {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE selectAudio(int index);

    /**
     * Select subtitle track by its index.
     *
     * @param     index [in] Index of subtitle track. Valid from 0 to {@link #getSubtitleCount} - 1.
     *            The availability of subtitle selections depends on current
     *            selection of uncommitted video and audio selections.
     *
     * @return    {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE selectSubtitle(int index);
     
    /**
     * Check if specified video track is available for selection.
     *
     * @param     index [in] Index of video track. Valid from 0 to {@link #getVideoCount} - 1.
     *            The availability of video tracks depends on current selection of
     *            uncommitted audio and subtitle tracks.
     *
     * @return    true if specified video track is available; false if not.
     */
    boolean isVideoAvailable(int index);

    /**
     * Check if specified audio track is available for selection.
     *
     * @param     index [in] Index of audio track. Valid from 0 to {@link #getAudioCount} - 1.
     *            The availability of audio tracks depends on current selection of
     *            uncommitted video and subtitle tracks.
     *
     * @return    true if specified audio track is available; false if not.
     */
    boolean isAudioAvailable(int index);

    /**
     * Check if specified subtitle track is available for selection.
     *
     * @param     index [in] Index of subtitle track. valid from 0 to {@link #getSubtitleCount} - 1.
     *            The availability of subtitle tracks depends on current selection of
     *            uncommitted video and audio tracks.
     *
     * @return    true if specified subtitle track is available; false if not.
     */
    boolean isSubtitleAvailable(int index);
   
    /**
     * Commit all current asset selections. If any asset type is not selected, current playing asset of that type is used.
     * This operation will remove all current selections after commit.
     *
     * @return    {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE commitSelection();
   
    /**
     * Remove all current uncommitted selections
     *
     * @return    {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    VO_OSMP_RETURN_CODE clearSelection();

    /**
     * Retrieve the properties of specified video track.
     *
     * @param    index  [in] Index of video track. Valid from 0 to {@link #getVideoCount} - 1.
     *
     * @return  a {@link VOOSMPAssetProperty} object if successful or null if failed.
     *          Returned object holds a list of key-value pairs where both key and value are
     *          of String type.
     *          Supported property keys are:
     *                   "description"
     *                   "codec"
     *                   "bitrate"
     *                   "width"
     *                   "height"
     */
    VOOSMPAssetProperty getVideoProperty(int index);

    /**
     * Retrieve the properties of specified audio track.
     *
     * @param    index [In] Index of audio track. Valid from 0 to {@link #getAudioCount()} - 1.
     *
     * @return  a {@link VOOSMPAssetProperty} object if successful or null if failed.
     *          Returned object holds a list of key-value pairs where both key and value are
     *          of String type.
     *          Supported property keys are:
     *                   "description"
     *                   "language"
     *                   "codec"    
     *                   "bitrate"
     *                   "channelcount"
     */
    VOOSMPAssetProperty getAudioProperty(int index);

    /**
     * Retrieve the properties of specified subtitle track.
     *
     * @param     index [in] Index of subtitle track. Valid from 0 to {@link #getSubtitleCount()} - 1.
     *
     * @return  a {@link VOOSMPAssetProperty} object if successful or null if failed.
     *          Returned object holds a list of key-value pairs where both key and value are
     *          of String type.
     *          Supported property keys are:
     *                   "description"
     *                   "language"
     *                   "codec"
     */
    VOOSMPAssetProperty getSubtitleProperty(int index);

	/**
	 * Retrieve the actual indices of video/audio/subtitle tracks which are being played.
	 * If a track is being played, its index is returned through VOOSMPAssetIndex.
	 * If no track of a specific type (mostly likely subtitle) is being played, then the corresponding index in VOOSMPAssetIndex is set to VO_OSMP_ASSET_NOT_SELECTED.
	 *
	 * @return   A {@link VOOSMPAssetIndex} object if successful or null if failed. Returned
	 *           the actual indices of video/audio/subtitle tracks which are being played.
	 */
    VOOSMPAssetIndex getPlayingAsset();
	
	/**
	 * Retrieve the current track selection of video/audio/subtitle.
	 * If a track is selected automatically, then the corresponding index in VOOSMPAssetIndex is set to VO_OSMP_ASSET_AUTO_SELECTED.
	 * If a track is being manually selected, its index is returned through VOOSMPAssetIndex.
	 * If no track of a specific type is being selected automatically/manually, then the corresponding index in VOOSMPAssetIndex is set to VO_OSMP_ASSET_NOT_SELECTED.
	 *
	 * @return   A {@link VOOSMPAssetIndex} object if successful or null if failed. Returned
	 *           object holds the current index values of the selected video, audio, and subtitle tracks.
	 */
    VOOSMPAssetIndex getCurrentSelection();
	
    interface VOOSMPAssetIndex
    {
    	/**
    	 * The index of the current video track. 
    	 * 
    	 * @return Valid from 0 to {@link VOCommonPlayerAssetSelection#getVideoCount} - 1.
    	 *
    	 * {@link INDEX_VIDEO_AUTO} indicates auto-select.
    	 */
        int getVideoIndex();
        
        /**
         * The index of the current audio track. 
         * 
         * @return Valid from 0 to {@link VOCommonPlayerAssetSelection#getAudioCount} - 1.
         */
        int getAudioIndex();
        

		/**
		 * The index of the current subtitle track. 
		 * 
		 * @return Valid from 0 to {@link VOCommonPlayerAssetSelection#getSubtitleCount} - 1.
		 */
        int getSubtitleIndex();
    }
   
    interface VOOSMPAssetProperty
    {
    	/**
    	 * Get the number of properties for the asset (track).
    	 *
    	 * @return    number of properties
    	 */
        int getPropertyCount();
        
        /**
         * Get the property key value by index.
         *
         * @param     index [in] The index of property. Valid from 0 to {@link VOOSMPAssetProperty#getPropertyCount} - 1.
         *
         * @return    key value (NSString). Supported property keys are specific to the asset type (video, audio, subtitle) and include:
         *                   @"description"
         *                   @"language"
         *                   @"codec"
         *                   @"bitrate"
         *                   @"width"
         *                   @"height"
         *					 @"channelcount"
         */
        String getKey(int index);
        
        /**
         * Get the property value by index.
         *
         * @param     index [in] The index of property. Valid from 0 to {@link VOOSMPAssetProperty#getPropertyCount} - 1.
         *
         * @return    property value (NSString)
         */
        Object getValue(int index);
    }
    
    /*
     * Set default audio language.
     *
     * @param     type [in] Language type {@link VO_OSMP_LANGUAGE_TYPE}.
     *
     * @return    {@link VO_OSMP_ERR_NONE} if successful
     */
//    VO_OSMP_RETURN_CODE setDefaultAudioLanguage(VO_OSMP_LANGUAGE_TYPE type);


    /*
     * Set default subtitle language.
     *
     * @param     type [in] Language type {@link VO_OSMP_LANGUAGE_TYPE}.
     *
     * @return    {@link VO_OSMP_ERR_NONE} if successful
     */
//    VO_OSMP_RETURN_CODE setDefaultSubtitleLanguage(VO_OSMP_LANGUAGE_TYPE type);
}
 

