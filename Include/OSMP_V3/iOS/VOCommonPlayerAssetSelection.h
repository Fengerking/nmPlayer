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

#import <Foundation/Foundation.h>

#import "VOOSMPType.h"

@protocol VOOSMPAssetIndex <NSObject>

/**
 * The index of the current video track. Valid from 0 to {@link VOCommonPlayerAssetSelection#getVideoCount} - 1.
 *
 * {@link INDEX_VIDEO_AUTO} indicates auto-select.
 */
@property (readonly, assign, getter=getVideoIndex) int videoIndex;

/**
 * The index of the current audio track. Valid from 0 to {@link VOCommonPlayerAssetSelection#getAudioCount} - 1.
 */
@property (readonly, assign, getter=getAudioIndex) int audioIndex;

/**
 * The index of the current subtitle track. Valid from 0 to {@link VOCommonPlayerAssetSelection#getSubtitleCount} - 1.
 */
@property (readonly, assign, getter=getSubtitleIndex) int subtitleIndex;

@end


@protocol VOOSMPAssetProperty <NSObject>

/**
 * Get the number of properties for the asset (track).
 *
 * @return    number of properties
 */
- (int) getPropertyCount;

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
 
 */
- (NSString*) getKey:(int)index;

/**
 * Get the property value by index.
 *
 * @param     index [in] The index of property. Valid from 0 to {@link VOOSMPAssetProperty#getPropertyCount} - 1.
 *
 * @return    property value (NSString)
 */
- (id) getValue:(int)index;

@end

/**
 * This index indicates a track is automatically selected
 */
const static int VO_OSMP_ASSET_AUTO_SELECTED = -1;

/**
 * This index indicates no track has been selected for a specific type
 */
const static int VO_OSMP_ASSET_NOT_SELECTED  = -2;


@protocol VOCommonPlayerAssetSelection <NSObject>

/**
 * Get the number of available video tracks.
 * <p>
 * A video track is defined as a combination of
 * bit rate and camera angle
 * </p>
 *
 * @return    number of available video tracks; -1 if unsuccessful
 */
- (int) getVideoCount;


/**
 * Get the number of available audio tracks.
 * <p>
 * An audio track is defined as a combination of
 * language and codec type. An audio track may have
 * multiple audio bit rates. The final playback bit rate
 * depends on selected video track.
 * </p>
 *
 * @return    number of available audio tracks; -1 if unsuccessful.
 */
- (int) getAudioCount;


/**
 * Get the number of available subtitle tracks.
 *
 * @return    number of available subtitle tracks; -1 if unsuccessful
 */
- (int) getSubtitleCount;


/**
 * Select video track by its index.
 *
 * @param     index [in] The index of video track. Valid from 0 to {@link VOCommonPlayerAssetSelection#getVideoCount} - 1.
 *            Use {@link INDEX_VIDEO_AUTO} to indicate auto-select.
 *            The availability of video tracks depends on current selection of
 *            uncommitted audio and subtitle tracks.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) selectVideo:(int)index;


/**
 * Select audio track by its index.
 *
 * @param     index [in] Index of audio track. Valid from 0.
 *            The availability of audio tracks depends on current selection of
 *            uncommitted video and subtitle tracks.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) selectAudio:(int)index;


/**
 * Select subtitle track by its index.
 *
 * @param     index [in] Index of subtitle track. Valid from 0.
 *            The availability of subtitle selections depends on current
 *            selection of uncommitted video and audio selections.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) selectSubtitle:(int)index;


/**
 * Check if specified video track is available for selection.
 *
 * @param     index [in] Index of video track. Valid from 0 to {@link VOCommonPlayerAssetSelection#getVideoCount} - 1.
 *            The availability of video track depends on current selection of
 *            uncommitted audio and subtitle tracks.
 *
 * @return    true if specified video stream is available; false if not
 */
- (bool) isVideoAvailable:(int)index;


/**
 * Check if specified audio track is available for selection.
 *
 * @param     index [in] Index of audio track. Valid from 0 to {@link VOCommonPlayerAssetSelection#getAudioCount} - 1.
 *            The availability of audio tracks depends on current selection of
 *            uncommitted video and subtitle tracks.
 *
 * @return    true if specified video stream is available; false if not
 */
- (bool) isAudioAvailable:(int)index;


/**
 * Check if specified subtitle track is available for selection.
 *
 * @param     index [in] Index of subtitle track. Valid from 0 to {@link VOCommonPlayerAssetSelection#getSubtitleCount} - 1.
 *            The availability of subtitle tracks depends on current selection of
 *            uncommitted video and audio tracks.
 *
 * @return    true if specified video stream is available; false if not
 */
- (bool) isSubtitleAvailable:(int)index;


/**
 * Commit all current asset selections. If any asset type is not selected, current playing asset of that type is used.
 * This operation will remove all current selections after commit.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) commitSelection;


/**
 * Remove all current uncommitted selections
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) clearSelection;


/**
 * Get the properties of specified video track.
 *
 * @param    index  [in] Index of video track property. Valid from 0 to {@link VOOSMPAssetProperty#getPropertyCount} - 1.
 *
 * @return   a {@link VOOSMPAssetProperty} object if successful; nil if unsuccessful
 *           Returned object holds a list of key-value pairs where both key and value are
 *           of NSString* type.
 *           Supported property keys are:
 *                   @"description"
 *                   @"codec"
 *                   @"bitrate"
 *                   @"width"
 *                   @"height"
 */
- (id<VOOSMPAssetProperty>) getVideoProperty:(int)index;


/**
 * Get the properties of specified audio track.
 *
 * @param    index  [in] Index of audio track property. Valid from 0 to {@link VOOSMPAssetProperty#getPropertyCount} - 1.
 *
 * @return   a {@link VOOSMPAssetProperty} object if successful; nil if unsuccessful
 *           Returned object holds a list of key-value pairs where both key and value are
 *           of NSString* type.
 *           Supported property keys are:
 *                   @"description"
 *                   @"language"
 *                   @"codec"
 *                   @"bitrate"
 *                   @"channelcount"
 */
- (id<VOOSMPAssetProperty>) getAudioProperty:(int)index;


/**
 * Get the properties of specified subtitle track.
 *
 * @param     index [in] Index of subtitle track property. Valid from 0.
 *
 * @return    a {@link VOOSMPAssetProperty} object if successful; nil if unsuccessful
 *            Returned object holds a list of key-value pairs where both key and value are
 *            of String type.
 *            Supported property keys are:
 *                   @"description"
 *                   @"language"
 *                   @"codec"
 */
- (id<VOOSMPAssetProperty>) getSubtitleProperty:(int)index;


/**
 * Get the actual indices of video/audio/subtitle tracks which are being played.
 * If a track is being played, its index is returned through VOOSMPAssetIndex.
 * If no track of a specific type (mostly likely subtitle) is being played, then the corresponding index in VOOSMPAssetIndex is set to VO_OSMP_ASSET_NOT_SELECTED.
 *
 * @return   a {@link VOOSMPAssetIndex} object if successful; nil if unsuccessful. Returns
 *           the actual indices of video/audio/subtitle tracks which are being played.
 */
- (id<VOOSMPAssetIndex>) getPlayingAsset;


/**
 * Get the current track selection of video/audio/subtitle.
 * If a track is selected automatically, then the corresponding index in VOOSMPAssetIndex is set to VO_OSMP_ASSET_AUTO_SELECTED.
 * If a track is being manually selected, its index is returned through VOOSMPAssetIndex.
 * If no track of a specific type is being selected automatically/manually, then the corresponding index in VOOSMPAssetIndex is set to VO_OSMP_ASSET_NOT_SELECTED.
 *
 * @return   a {@link VOOSMPAssetIndex} object if successful; nil if unsuccessful. Returned
 *           object holds the current index values of the selected video, audio, and subtitle tracks.
 */
- (id<VOOSMPAssetIndex>) getCurrentSelection;


/**
 * Set default audio language.
 *
 * @param     type [in] Language type, follow definition of ISO 639-2.
 * For example,it should be "fre" if want French language
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setDefaultAudioLanguage:(NSString*)type;


/**
 * Set default subtitle language.
 *
 * @param     type [in] Language type, follow definition of ISO 639-2.
 * For example,it should be "fre" if want French language
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setDefaultSubtitleLanguage:(NSString*)type;


@end


