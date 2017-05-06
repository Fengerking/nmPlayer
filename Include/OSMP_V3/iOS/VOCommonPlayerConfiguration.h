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
#import "VOOSMPVerificationInfo.h"
#import "VOOSMPImageData.h"

@protocol VOCommonPlayerConfiguration <NSObject>

/**
 * Set contents of license file.
 *
 * @param   data  [in] Contents of the license file in a Byte array.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 *
 * <pre>This API must be called before calling the {@link VOCommonPlayerControl#start} method.</pre>
 */
- (VO_OSMP_RETURN_CODE) setLicenseContent:(NSData *)data;


/**
 * Set the location of the license file.
 *
 * @param   path  [in] Location of the license file.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 *
 * <pre>This API must be called before calling the {@link VOCommonPlayerControl#start} method.</pre>
 */
- (VO_OSMP_RETURN_CODE) setLicenseFilePath:(NSString *)path;


/**
 * Set pre-agreed license string
 *
 * @param   str   [in] Pre-agreed license string.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful.
 *
 * <pre>This API must be called before calling the {@link VOCommonPlayerControl#start} method.</pre>
 */
- (VO_OSMP_RETURN_CODE) setPreAgreedLicense:(NSString *)str;


/**
 * Enable/Disable deblock. This is valid for H.264 and Real video. The default is enabled.
 *
 * @param   value  [in] Enable/Disable; true to enable deblock (default), false to disable deblock.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 *
 */
- (VO_OSMP_RETURN_CODE) enableDeblock:(bool) value;


/**
 * Set the location of device capability file. The device capability file contains device-specific display sizes and bit rates to optimize performance. An example cap.xml file is included with the SamplePlayer project.
 *
 * @param   filename  [in] Location of device capability file.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful; error code if unsuccessful
 *
 * <pre>This API should be called before calling the {@link VOCommonPlayerControl#open:flag:sourceType:openParam:} method.</pre>
 *
 */
- (VO_OSMP_RETURN_CODE) setDeviceCapabilityByFile:(NSString *)filename;


/**
 * Set the initial bitrate.
 * The player will select the closest stream at or below that number. If none is available,
 * it will pick the closest one above.
 * If an initial bitrate is not set, the player will follow the streaming protocol specification.
 * If the behavior is undefined in the protocol being used, the player will pick a stream based on cap.xml,
 * the order in the manifest, and other heuristics.
 *
 * @param   bitrate  [in] Bitrate <bps>.
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 *
 * <pre>This API should be called before calling the {@link VOCommonPlayerControl#open:flag:sourceType:openParam:} method.</pre>
 *
 */
- (VO_OSMP_RETURN_CODE) setInitialBitrate:(int) bitrate;


/**
 * Show a portion of video.
 *
 * @param   mode [in] Override automatic screen width and height adjustment according to zoom mode. Refer to {@link VO_OSMP_ZOOM_MODE}.
 *
 * @param   rect [in] The rectangle area specified as the second argument is only used when the zoom mode is {@link VO_OSMP_ZOOM_ZOOMIN}.
 *                    Currently not implemented for iOS.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 *
 */
- (VO_OSMP_RETURN_CODE) setZoomMode:(VO_OSMP_ZOOM_MODE)mode rect:(Rect)rect;


/**
 * Enable defining position based on live streaming DVR window. The default is enabled.
 * <p>
 * When enabled:
 * <p><ul>
 * <li> {@link VOCommonPlayerStatus#getPosition} returns 0 when playing at the live head. A negative position represents
 *      the offset into the DVR window with respect to live head
 * <li> {@link VOCommonPlayerStatus#getMaxPosition} always returns 0 which represent the live head
 * <li> {@link VOCommonPlayerStatus#getMinPosition} returns (getMaxPosition - DVR window size)
 * </ul></p>
 *
 * When disabled:
 * <p><ul>
 * <li> {@link VOCommonPlayerStatus#getPosition} returns the offset with respect to the beginning of the playback
 * <li> {@link VOCommonPlayerStatus#getMaxPosition} returns the offset between the live head and the beginning of the playback
 * <li> {@link VOCommonPlayerStatus#getMinPosition} returns (getMaxPosition - DVR window size)
 * </ul></p>
 * </p>
 *
 * @param   value  [in] Enable/Disable; true to enable(default), false to disable.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) enableLiveStreamingDVRPosition:(bool)enable;


/**
 * Set DRM Library name and API name
 *
 * @param libName    [in] DRM library name.
 * @param libApiName [in] DRM library export api name.
 *
 * @return {@link VO_OSMP_ERR_NONE} if successful .
 *
 * <p>This API should be called before calling the {@link VOCommonPlayerControl#open:flag:sourceType:openParam:} method.</p>
 */
- (VO_OSMP_RETURN_CODE) setDRMLibrary:(NSString *)libName libApiName:(NSString *)libApiName;


/**
 * Set the DRM adapter.
 *
 * @param   adapter  [in] DRM adapter
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setDRMAdapter:(void *)adapter;


/**
 * Set the verification information to start DRM verification.
 *
 * @param   info   [in] Verification information. {@link VOOSMPVerificationInfo}
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setDRMVerificationInfo:(VOOSMPVerificationInfo *)info;


/**
 * Set unique identifier of the device.
 *
 * @param   string [in] Unique identifier string
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 *
 * @deprecated This method is replaced by {@link VOCommonPlayerConfiguration#setDRMUniqueIdentifier:} and will be removed in the future.
 */
- (VO_OSMP_RETURN_CODE) setDRMUniqueIndentifier:(NSString *)string __attribute__((deprecated));


/**
 * Set unique identifier of the device.
 *
 * @param   string [in] Unique identifier string
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setDRMUniqueIdentifier:(NSString *)string;


/**
 * Set the initial buffering time before playback starts.
 *
 * @param   time [in] buffer time <milliseconds>
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setInitialBufferTime:(int)time;


/**
 * Set the max buffering time for playback.
 *
 * @param   time [in] buffer time (milliseconds)
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setMaxBufferTime:(int)time;


/**
 * Set the buffering time when re-buffering is needed during playback
 *
 * @param   time [in] buffer time (milliseconds)
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful.
 */
- (VO_OSMP_RETURN_CODE) setPlaybackBufferingTime:(int)time;


/**
 * Set the buffering time for continue playback when need to buffer again.
 *
 * @param   time [in] buffer time (milliseconds)
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful.
 *
 * @deprecated This method is replaced by {@link VOCommonPlayerConfiguration#setPlaybackBufferingTime:} and will be removed in the future.
 */
- (VO_OSMP_RETURN_CODE) setAnewBufferingTime:(int)time __attribute__((deprecated));


/**
 * Enable/Disable playback of the audio stream. The default is enabled.
 *
 * @param   value  [in] Enable/Disable; true to enable, false to disable.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 *
 */
- (VO_OSMP_RETURN_CODE) enableAudioStream:(bool) value;


/**
 * Enable/Disable playback of the video stream. The default is enabled.
 *
 * @param   value  [in] Enable/Disable; true to enable, false to disable.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 *
 */
- (VO_OSMP_RETURN_CODE) enableVideoStream:(bool) value;


/**
 * Enable/Disable immediate video rendering with low latency.
 * This function is only applicable to video-only streams. The default is disabled.
 *
 * @param   value  [in] Enable/Disable; true to enable, false to disable.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 *
 */
- (VO_OSMP_RETURN_CODE) enableLowLatencyVideo:(bool) value;


/**
 * Enable/Disable audio effect. The default is disabled.
 *
 * @param   value  [in] Enable/Disable; true to enable, false to disable.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 *
 */
- (VO_OSMP_RETURN_CODE) enableAudioEffect:(bool) value;


/**
 * Enable/Disable PCM data output. The default is disabled.
 *
 * @param   value  [in] Enable/Disable; true to enable PCM data output, false to disable (default).
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 *
 */
- (VO_OSMP_RETURN_CODE) enablePCMOutput:(bool) value;


/**
 * Set audio playback speed.
 *
 * @param   speed  [in] speed multiplier with respect to realtime playback. Default is 1.0
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 *
 */
- (VO_OSMP_RETURN_CODE) setAudioPlaybackSpeed:(float)speed;


/**
 * Set the presentation delay time for a live streaming source.
 *
 * @param   time [in] presentation delay time (milliseconds) for a live streaming source with respective to current time
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setPresentationDelay:(int)time;


/**
 * Enable/Disable processing SEI information.
 *
 * @param   flag [in] the flag {@link VO_OSMP_SEI_INFO_FLAG}. Set to VO_OSMP_SEI_INFO_NONE to disable processing SEI information or any other flags to enable
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) enableSEI:(VO_OSMP_SEI_INFO_FLAG)flag;


/**
 * Set the number of retry attempts in progressive download module when network connection fails.
 *
 * @param   times [in] the number of retry attempts. Default is 2. Setting this value to -1 to continue retrying until {@link VOCommonPlayerControl#stop} be called.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setPDConnectionRetryCount:(int)times;


/**
 * Enable/Disable CPU adaptation. The default is enabled.
 *
 * @param   value  [in] Enable/Disable; true to enable, false to disable.
 *
 * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
 *
 */
- (VO_OSMP_RETURN_CODE) enableCPUAdaptation:(bool)value;


/**
 * Set upper/lower bitrate threshold for adaptation
 *
 * @param upper [in] Maximum bitrate that the adaptation must not exceed (bps) in addition to the constraint defined in the device capability file {@link VOCommonPlayerControl#setDeviceCapabilityByFile}.
 * <p><ul>
 * <li> If the value is 0, the maximum bitrate is constrained by the device capability file if it is specified. Otherwise the maximum bitrate is unconstrained.
 * <li> If the value is not 0 and the device capability file is not specified, then it defines the maximum bitrate threshold
 * <li> If the value is not 0 and the device capability file is specified, then the maximum bitrate threshold is defined as min(upper, max bitrate in device capability file)
 * </ul><p>
 * @param lower [in] Mimimum bitrate that the adaptation must not drop below (bps).
 * <p><ul>
 * <li> If the value is 0, then the minimum threshold is unconstrained
 * <li> If the value is not 0, then it defined the minimum bitrate threshold that adaptation must not drop below.
 * </ul><p>
 * @return {@link VO_OSMP_ERR_NONE} if successful.
 *
 * <pre> This API should be called before calling the {@link VOCommonPlayerControl#open:flag:sourceType:openParam:} method. </pre>
 *
 */
- (VO_OSMP_RETURN_CODE) setBitrateThreshold:(int)upper lower:(int)lower;


/**
 * Set the maximum number of retry attempts to download a corresponding segment from a
 * different bit rate due to DRM errors, server issues, or unsupported media format.
 *
 * @param times [in] Maximum number of retry attempts. The default is 0.
 * @return {@link VO_OSMP_ERR_NONE} if successful.
 *
 * <pre>This API should be called before calling the {@link VOCommonPlayerControl#open:flag:sourceType:openParam:} method.</pre>
 *
 */
- (VO_OSMP_RETURN_CODE) setSegmentDownloadRetryCount:(int) times;


/**
 * Enable/Disable loading Dolby libraries. The default is enable.
 *
 * @param   value  [in] Enable/Disable; true to enable, false to disable.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 *
 */
- (VO_OSMP_RETURN_CODE) enableDolbyLibrary:(bool) value;


/**
 * Set the value for a specified ID. This method should only be used on recommendation from VisualOn.
 *
 * @param   paramID  [in] Parameter id.
 * @param   pObj     [in] Parameter value; depends on parameter id.
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setParameter:(int)paramID pObj:(void *)pObj;


@end

