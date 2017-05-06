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

@protocol VOCommonPlayerStatus <NSObject>

/**
 * Get player engine type
 *
 * @return Player engine type, refer to {@link VO_OSMP_PLAYER_ENGINE}
 */
- (VO_OSMP_PLAYER_ENGINE) getPlayerType;


/**
 * Get player status.
 *
 * @return  player status. Refer to {@link VO_OSMP_STATUS}
 */
- (VO_OSMP_STATUS) getPlayerStatus;


/**
 * Check if the stream can be paused.
 *
 * @return  true if the stream can be paused
 */
- (bool) canBePaused;


/**
 * Check if the opened data source is live streaming.
 *
 * @return true if the data source is opened, is a streaming source and is live streaming
 */
- (bool) isLiveStreaming;


/**
 * Get current playback position.
 *
 * <p><ul>
 * <li> For VOD. returns current playback position.
 * <li> For Live streaming. Depend on {@link VOCommonPlayerConfiguration#enableLiveStreamingDVRPosition:}
 *  <ol>
 *  <li>If enable. returns 0 when playing at the live head. A negative position represents the offset into the DVR window with respect to live head
 *  <li>If disable. returns the offset with respect to the beginning of the playback
 * </ul></p>
 *
 * @return  the position <ms>; -1 if unsuccessful
 */
- (long) getPosition;


/**
 * Get minimum position.
 *
 * <p><ul>
 * <li> For VOD: returns 0
 * <li> For Live: returns (getMaxPosition - DVR window size)
 * </ul></p>
 *
 * @return The position (ms); -1 if unsuccessful
 */
- (long) getMinPosition;


/**
 * Get maximum position.
 *
 * <p><ul>
 * <li> For VOD: returns the duration
 * <li> For Live: depends on {@link VOCommonPlayerConfiguration#enableLiveStreamingDVRPosition}
 *  <ol>
 *  <li>If enabled: always returns 0, which represents the live playhead
 *  <li>If disabled: returns the offset between the live playhead and the beginning of the playback
 * </ul></p>
 *
 * @return The position (ms); -1 if unsuccessful
 */
- (long) getMaxPosition;


/**
 * Get duration of the stream.
 *
 * @return  current stream duration <ms>; 0 if source is a live stream
 */
- (long) getDuration;


/**
 * Get screen display brightness.
 *
 * @return Current display brightness (percent)
 */
- (int) getScreenBrightness;


/**
 * Get unique identifier of the device.
 *
 * @return  unique identifier is successful; nil if unsuccessful
 *
 * @deprecated This method is replaced by {@link VOCommonPlayerConfiguration#getDRMUniqueIdentifier} and will be removed in the future.
 */
- (NSString *) getDRMUniqueIndentifier __attribute__((deprecated));


/**
 * Get unique identifier of the device.
 *
 * @return  unique identifier is successful; nil if unsuccessful
 */
- (NSString *) getDRMUniqueIdentifier;


/**
 * Get version information of a module.
 * <p>
 * The OSMP+ SDK is currently built using the same version for all modules.
 * Instead of passing a module as an argument, simply pass VO_OSMP_MODULE_TYPE_SDK
 * </p>
 *
 * @param   module [in] module type.
 *
 * @return  version information of the given module
 */
- (NSString*) getVersion:(VO_OSMP_MODULE_TYPE)module;


/**
 * Get the SEI info.
 *
 * @param   time [in] the time stamp of SEI that want to get
 * @param   flag [in] the type flag of SEI info {@link VO_OSMP_SEI_INFO_FLAG}
 *
 * @return  the object according to flag if successful
 */
- (id) getSEIInfo:(long long)time flag:(VO_OSMP_SEI_INFO_FLAG)flag;


/**
 * Get the value from specified ID. This method should only be used on recommendation from VisualOn.
 *
 * @param   paramID  [in] Parameter id.
 * @param   pObj     [out] Returned parameter value; depends on parameter id.
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) getParameter:(int)paramID pObj:(void *)pObj;


@end
