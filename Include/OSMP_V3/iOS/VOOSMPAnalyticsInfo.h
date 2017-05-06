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


@protocol VOOSMPAnalyticsInfo <NSObject>

/**
 * How far back to report (seconds).
 *
 * @return how far back to report (seconds)
 */
@property (readonly, assign, getter=getLastTime) int lastTime;

/**
 * Get number of source dropped frames.
 * <p>
 * This function returns the total count of dropped frames by the module which fetches assets from the source.
 * If the playing time is ahead of next or second next key frame, we will drop the frames before this key frame to catch up to the video playing time.
 * </p>
 *
 * @return number of source dropped frames
 */
@property (readonly, assign, getter=getSourceDropNum) int sourceDropNum;

/**
 * Get number of codec dropped frames.
 * <p>
 * This function returns the total count of dropped frames at the input of the video decoder module.
 * Before decoding the current frame, if the playing time is ahead of current frame and this frame is not a reference frame, we will drop it to catch up to the video playing time.
 * </p>
 *
 * @return number of codec dropped frames
 */
@property (readonly, assign, getter=getCodecDropNum) int codecDropNum;

/**
 * Get number of render dropped frames.
 * <p>
 * This function returns the total count of dropped frames at the input of the rendering module.
 * Before rendering the current frame, if the playing time is ahead of current frame and there are more frames ready to be rendered, we will drop it to catch up to the video playing time.
 * <p>
 *
 * @return number of render dropped frames
 */
@property (readonly, assign, getter=getRenderDropNum) int renderDropNum;

/**
 * Get number of decoded frames
 *
 * @return number of decoded frames
 */
@property (readonly, assign, getter=getDecodedNum) int decodedNum;

/**
 * Get number of rendered frames.
 *
 * @return number of rendered frames
 */
@property (readonly, assign, getter=getRenderNum) int renderNum;

/**
 * Get number of instances that the source exceeds the time (I / ms).
 *
 * @return number of instances that the source exceeds the time
 */
@property (readonly, assign, getter=getSourceTimeNum) int sourceTimeNum;

/**
 * Get number of instances that the codec exceeds the time  (I / ms).
 *
 * @return number of instances that the codec exceeds the time
 */
@property (readonly, assign, getter=getCodecTimeNum) int codecTimeNum;

/**
 * Get number of instances that the render exceeds the time (I / ms).
 *
 * @return number of instances that the render exceeds the time
 */
@property (readonly, assign, getter=getRenderTimeNum) int renderTimeNum;

/**
 * Get number of instances that the jitter exceeds the time (I / ms).
 *
 * @return number of instances that the jitter exceeds the time (I / ms)
 *
 */
@property (readonly, assign, getter=getJitterNum) int jitterNum;

/**
 * Get number of codec frames dropped after encountering errors.
 *
 * @return number of codec frames dropped after encountering errors
 */
@property (readonly, assign, getter=getCodecErrorsNum) int codecErrorsNum;

/**
 * Get list of codec errors.
 *
 * @return list of codec error codes
 */
@property (readonly, assign, getter=getCodecErrors) int* codecErrors;

/**
 * Get current CPU load (percent).
 *
 * @return current CPU load (percent)
 */
@property (readonly, assign, getter=getCPULoad) int CPULoad;

/**
 * Get current CPU frequency.
 *
 * @return current CPU frequency
 */
@property (readonly, assign, getter=getFrequency) int frequency;

/**
 * Get maximum CPU frequency.
 *
 * @return aximum CPU frequency
 */
@property (readonly, assign, getter=getMaxFrequency) int maxFrequency;

/**
 * Get worst codec decode time (ms).
 *
 * @return worst codec decode time
 */
@property (readonly, assign, getter=getWorstDecodeTime) int worstDecodeTime;

/**
 * Get worst render time (ms).
 *
 * @return worst render time (ms)
 */
@property (readonly, assign, getter=getWorstRenderTime) int worstRenderTime;

/**
 * Get average codec decode time (ms).
 *
 * @return average codec decode time (ms)
 */
@property (readonly, assign, getter=getAverageDecodeTime) int averageDecodeTime;

/**
 * Get average render time (ms).
 *
 * @return average render time (ms)
 */
@property (readonly, assign, getter=getAverageRenderTime) int averageRenderTime;

/**
 * Get current total CPU load (percent).
 *
 * @return current total CPU load (percent)
 */
@property (readonly, assign, getter=getTotalCPULoad) int totalCPULoad;


/**
 * Get current playback duration since playback began.
 *
 * @return total playback duration (ms)
 */
@property (readonly, assign, getter=getPlaybackDuration) int playbackDuration;

/**
 * Get current count of frames dropped by source module since playback began.
 *
 * @return total number of frames dropped by source module
 */
@property (readonly, assign, getter=getTotalSourceDropNum) int totalSourceDropNum;

/**
 * Get current count of frame dropped by codec since playback began.
 *
 * @return total number of codec frames dropped by codec
 */
@property (readonly, assign, getter=getTotalCodecDropNum) int totalCodecDropNum;

/**
 * Get current count of frames dropped by renderer since playback began.
 *
 * @return total number of frames dropped by renderer
 */
@property (readonly, assign, getter=getTotalRenderDropNum) int totalRenderDropNum;

/**
 * Get current count of decoded frames since playback began.
 *
 * @return total number of decoded frames
 */
@property (readonly, assign, getter=getTotalDocodedNum) int totalDecodedNum;

/**
 * Get current count of rendered frames since playback began.
 *
 * @return total number of rendered frames
 */
@property (readonly, assign, getter=getTotalRenderedNum) int totalRenderedNum;

@end

