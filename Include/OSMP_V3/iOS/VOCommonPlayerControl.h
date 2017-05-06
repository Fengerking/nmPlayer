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

#import "VOCommonPlayerDelegate.h"

#import "VOOSMPInitParam.h"
#import "VOOSMPOpenParam.h"
#import "VOOSMPType.h"

@protocol VOCommonPlayerControl <NSObject>


#pragma mark Player initialization

/**
 * Initialize a player instance.
 * <pre>This function must be called first to start a session.</pre>
 *
 * @param   playEngineType  [in] Refer to {@link VO_OSMP_PLAYER_ENGINE}.
 * @param   initParam       [in] Currently unused, should be set to nil.
 * @return  player object if successful; nil if unsuccessful
 */
- (id) init:(VO_OSMP_PLAYER_ENGINE)playEngineType initParam:(VOOSMPInitParam *)initParam;


/**
 * Initialize a player instance.
 * <pre>This function must be called first to start a session.</pre>
 *
 * @param   playEngineType  [in] Refer to {@link VO_OSMP_PLAYER_ENGINE}.
 * @param   initParam       [in] Currently unused, should be set to nil.
 * @param   initParamFlag   [in] Currently unused, should be set to {@link VO_OSMP_FLAG_INIT_NOUSE}.
 * @return  player object if successful; nil if unsuccessful
 *
 * @deprecated This method is replaced by {@link VOCommonPlayerControl#init:initParam:} and will be removed in the future.
 */
- (id) init:(VO_OSMP_PLAYER_ENGINE)playEngineType initParam:(id)initParam initParamFlag:(int)initParamFlag __attribute__((deprecated));


/**
 * Set surface view for video playback.
 *
 * @param   view  [in] The UIView(iOS) / CALayer(Mac OS) to render video. Do not add other sub views in this view.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setView:(void*) view;


/**
 * Set the dimensions (rect) of the video display.
 *
 * @param   Rect  [in] The rect of view to display.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful.
 */
- (VO_OSMP_RETURN_CODE) setDisplayArea:(Rect)rect;


#pragma mark Data source initialization

/**
 * Open media source.
 *
 * @param   url             [in] Source file description (e.g. an URL or a file descriptor, etc.)
 * @param   flag            [in] The flag for opening media source. Refer to {@link VO_OSMP_SRC_FLAG}.
 * @param   sourceType      [in] Indicates the source format. Refer to {@link VO_OSMP_SRC_FORMAT}. Default value is {@link VO_OSMP_SRC_AUTO_DETECT}
 * @param   openParam       [in] Open parameters. Refer to {@link VOOSMPOpenParam}. Valid fields depend on the value.
 *
 * @return {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) open:(NSString *)url flag:(VO_OSMP_SRC_FLAG)flag sourceType:(VO_OSMP_SRC_FORMAT)sourceType openParam:(VOOSMPOpenParam *)openParam;


/**
 * Open media source.
 *
 * @param   url             [in] Source file description (e.g. an URL or a file descriptor, etc.)
 * @param   flag            [in] The flag for opening media source. Refer to {@link VO_OSMP_SRC_FLAG}.
 * @param   type            [in] Indicates the source format. Refer to {@link VO_OSMP_SRC_FORMAT}. Default value is {@link VO_OSMP_SRC_AUTO_DETECT}
 * @param   initParam       [in] Initial parameters. Refer to {@link VOOSMPInitParam}. Valid fields depend on  initParamFlag.
 *                               If (initParamFlag & {@link VO_OSMP_FLAG_INIT_ACTUAL_FILE_SIZE}) > 0,
 *                               You should set the fileSize.
 * @param   initParamFlag   [in] InitParam flag(s). Refer to {@link VO_OSMP_INIT_PARAM_FLAG}.
 *                               You can use "|" (or) to input several parameters.
 *
 * @return {@link VO_OSMP_ERR_NONE} if successful
 *
 * @deprecated this function is replaced by {@link VOCommonPlayerControl#open:flag:sourceType:openParam:} and will be removed in the future
 */
- (VO_OSMP_RETURN_CODE) open:(NSString *)url flag:(VO_OSMP_SRC_FLAG)flag sourceType:(VO_OSMP_SRC_FORMAT)sourceType initParam:(VOOSMPInitParam *)initParam initParamFlag:(int)initParamFlag __attribute__((deprecated));


/**
 * Close media source. Framework is still available.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) close;


#pragma mark Runtime control

/**
 * Start playback.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) start;


/**
 * Pause playback.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) pause;


/**
 * Stop playback.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) stop;


/**
 * Seek operation.
 *
 * @param   msec [in] The position <ms> to seek to.
 * @return  position after seek operation; -1 if unsuccessful
 */
- (long) setPosition:(long)msec;


/**
 * Set playback volume.
 *
 * @param   value [in] A value of 0.0f indicates silence; a value of 1.0f indicates no attenuation.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setVolume:(float)value;


/**
 * Mute the audio.
 *
 * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) mute;


/**
 * Unmute the audio.
 *
 * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) unmute;


/**
 * Suspend player.
 * <pre>The application layer should invoke this method on receiving notification about moving to background.</pre>
 *
 * @param keepAudio [in] Keep/pause audio. True for continuing to play audio, false to pause both video and audio.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful; error code if unsuccessful
 */
- (VO_OSMP_RETURN_CODE) suspend:(bool)keepAudio;


/**
 * Resume player.
 * <pre>The application layer should invoke this method on receiving notification about moving to foreground.</pre>
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful; error code if unsuccessful
 */
- (VO_OSMP_RETURN_CODE) resume;


/**
 * Set aspect ratio of video.
 *
 * @param   ar  [in] Aspect ratio. Refer to {@link VO_OSMP_ASPECT_RATIO}.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful; error code if unsuccessful
 */
- (VO_OSMP_RETURN_CODE) setVideoAspectRatio:(VO_OSMP_ASPECT_RATIO)ar;


/**
 * Redraw the video.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) redrawVideo;


/**
 * Set the delegate to handle SDK events. Events are posted by sub-thread to {@link VOCommonPlayerDelegate#onVOEvent:param1:param2:pObj:}.
 *
 * @param   delegate [in] Event handler instance.
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setOnEventDelegate:(id <VOCommonPlayerDelegate>)delegate;


/**
 * Set screen display brightness.
 *
 * @param brightness [in] Display brightness (percent). The valid range is 0 to 100
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setScreenBrightness:(int)brightness;


/**
 * Start periodic SEI data notifications.
 *
 * @param   interval [in] Time interval <ms> between two SEI data notifications.
 *
 * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) startSEINotification:(int)interval;


/**
 * Stop periodic SEI data notifications.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) stopSEINotification;


/**
 * Capture a video image, only supported by {@link VO_OSMP_VOME2_PLAYER}. Feature is currently disabled.
 *
 * @return  video image data
 */
- (id<VOOSMPImageData>) captureVideoImage;


/**
 * Update media source.
 *
 * @param   url             [in] Source file description (e.g. an URL or a file descriptor, etc.)
 *
 * @return {@link VO_OSMP_ERR_NONE} if successful
 *
 * * <pre>This API should be called after calling the {@link VOCommonPlayerControl#open:flag:sourceType:openParam:} method.</pre>
 *
 */
- (VO_OSMP_RETURN_CODE) updateSourceURL:(NSString *)url;


/**
 * Start periodic analytics data notifications. With each notification, analytics data of past "interval" ms is provided.
 *
 * @param   interval [in] Time interval <ms> between two analytics data notifications.
 * @param   filter   [in] Filters, specified as an {@link VOOSMPAnalyticsFilter} object, to be applied to cached analytic data.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) startAnalyticsNotification:(int)interval filter:(VOOSMPAnalyticsFilter *)filter;


/**
 * Stop periodic analytics data notifications.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) stopAnalyticsNotification;


@end

