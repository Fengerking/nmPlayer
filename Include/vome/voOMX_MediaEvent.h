/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __VO_OMX_MEDIA_EVENT_H__
#define __VO_OMX_MEDIA_EVENT_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enum media_event_type {
	MEDIA_NOP               = 0, // interface test message
	MEDIA_PREPARED          = 1,
	MEDIA_PLAYBACK_COMPLETE = 2,
	MEDIA_BUFFERING_UPDATE  = 3,
	MEDIA_SEEK_COMPLETE     = 4,
	MEDIA_SET_VIDEO_SIZE    = 5,
	MEDIA_ERROR             = 100,
	MEDIA_INFO              = 200,
};

#define VOEXT1_DRM_BASE			0x8610
#define VOEXT1_SOURCE_BASE		0x8620
#define VOEXT1_STREAMING_BASE	0x8630
#define VOEXT1_CODEC_BASE		0x8650
#define VOEXT1_INFO_BASE		0x8670
#define VOEXT1_PRIVATE			0x87FF

// Generic error codes for the media player framework.  Errors are fatal, the
// playback must abort.
//
// Errors are communicated back to the client using the
// MediaPlayerListener::notify method defined below.
// In this situation, 'notify' is invoked with the following:
//   'msg' is set to MEDIA_ERROR.
//   'ext1' should be a value from the enum media_error_type.
//   'ext2' contains an implementation dependant error code to provide
//          more details. Should default to 0 when not used.
//
// The codes are distributed as follow:
//   0xx: Reserved
//   1xx: Android Player errors. Something went wrong inside the MediaPlayer.
//   2xx: Media errors (e.g Codec not supported). There is a problem with the
//        media itself.
//   3xx: Runtime errors. Some extraordinary condition arose making the playback
//        impossible.
//
enum media_error_type {
	// 0xx
	MEDIA_ERROR_UNKNOWN = 1,
	// 1xx
	MEDIA_ERROR_SERVER_DIED = 100,
	// 2xx
	MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 200,
	// 3xx

	MEDIA_ERROR_VO_DRM_EXPIRED = (VOEXT1_DRM_BASE | 0x1), 
	MEDIA_ERROR_VO_DRM_NOAUTH  = (VOEXT1_DRM_BASE | 0x2), 
	MEDIA_ERROR_VO_DRM_NOAUTH_RESLUTION = (VOEXT1_DRM_BASE | 0x3), 
	MEDIA_ERROR_VO_DRM_RENTAL_EXPIRED   = (VOEXT1_DRM_BASE | 0x4), 

	MEDIA_ERROR_VO_CODEC_FATAL_ERROR = (VOEXT1_CODEC_BASE | 0x1), //reserved
};


// Info and warning codes for the media player framework.  These are non fatal,
// the playback is going on but there might be some user visible issues.
//
// Info and warning messages are communicated back to the client using the
// MediaPlayerListener::notify method defined below.  In this situation,
// 'notify' is invoked with the following:
//   'msg' is set to MEDIA_INFO.
//   'ext1' should be a value from the enum media_info_type.
//   'ext2' contains an implementation dependant error code to provide
//          more details. Should default to 0 when not used.
//
// The codes are distributed as follow:
//   0xx: Reserved
//   7xx: Android Player info/warning (e.g player lagging behind.)
//   8xx: Media info/warning (e.g media badly interleaved.)
// 
enum media_info_type {
	// 0xx
	MEDIA_INFO_UNKNOWN = 1,
	// 7xx
	// The video is too complex for the decoder: it can't decode frames fast
	// enough. Possibly only the audio plays fine at this stage.
	MEDIA_INFO_VIDEO_TRACK_LAGGING = 700,
	
	 // MediaPlayer is temporarily pausing playback internally in order to
    // buffer more data.
    MEDIA_INFO_BUFFERING_START = 701,
    // MediaPlayer is resuming playback after filling buffers.
    MEDIA_INFO_BUFFERING_END = 702,
	
	// 8xx
	// Bad interleaving means that a media has been improperly interleaved or not
	// interleaved at all, e.g has all the video samples first then all the audio
	// ones. Video is playing but a lot of disk seek may be happening.
	MEDIA_INFO_BAD_INTERLEAVING = 800,
	// The media is not seekable (e.g live stream).
	MEDIA_INFO_NOT_SEEKABLE = 801,

	MEDIA_INFO_VO_DRM_PURCHASE = (VOEXT1_DRM_BASE | 0x1),	/* reserved */
	MEDIA_INFO_VO_DRM_RENTAL = (VOEXT1_DRM_BASE | 0x2),		/* 0xRRRRUUTT R: reserved, U: used view count, T: total view count */

	MEDIA_INFO_VO_FORMAT_UNSUPPORTED  = (VOEXT1_SOURCE_BASE | 0x1), 
	MEDIA_INFO_VO_SOURCE_FRAMEDROPPED = (VOEXT1_SOURCE_BASE | 0x2), 
	MEDIA_INFO_VO_AUDIO_UNSUPPORTED   = (VOEXT1_SOURCE_BASE | 0x3),
	MEDIA_INFO_VO_VIDEO_UNSUPPORTED   = (VOEXT1_SOURCE_BASE | 0x4),

	MEDIA_INFO_VO_STREAMING_PACKETLOST = (VOEXT1_STREAMING_BASE | 0x1), 
	MEDIA_INFO_VO_STREAMING_SERVERGOODBYE = (VOEXT1_STREAMING_BASE | 0x2), 
	MEDIA_INFO_VO_STREAMING_ERROREVENT = (VOEXT1_STREAMING_BASE | 0x3), /* streaming error event ID */
	MEDIA_INFO_VO_STREAMING_BITRATE = (VOEXT1_STREAMING_BASE | 0x4), /* streaming bitrate (k bits/s) */
	MEDIA_INFO_VO_STREAMING_THROUGHPUT = (VOEXT1_STREAMING_BASE | 0x5), /* streaming throughput (k bytes/s) */
	MEDIA_INFO_VO_STREAMING_RTCPRR = (VOEXT1_STREAMING_BASE | 0x6), /* streaming RTCP RR (VORTCPReceiverReport* definded in voStreaming.h) */

	MEDIA_INFO_VO_CODEC_PASSABLE_ERROR = (VOEXT1_CODEC_BASE | 0x1), /* 0: unspecified; 1: audio decoder; 2: video decoder; 3: audio encoder; 4, video encoder. */
	MEDIA_INFO_VO_CODEC_UNSUPPORTED = (VOEXT1_CODEC_BASE | 0x2), 

	MEDIA_INFO_VO_MEDIA_CONTENT_TYPE = (VOEXT1_INFO_BASE | 0x1), /* 1: audio only; 2: video only; 3: audio and video. */
	MEDIA_INFO_VO_MEDIA_AUDIO_FOURCC = (VOEXT1_INFO_BASE | 0x2), /* four cc */
	MEDIA_INFO_VO_MEDIA_VIDEO_FOURCC = (VOEXT1_INFO_BASE | 0x3), /* four cc */
	MEDIA_INFO_VO_MEDIA_VIDEO_ROTATION = (VOEXT1_INFO_BASE | 0x4), /* notify only need rotation: 90, 180, 270 */
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __VO_OMX_MEDIA_EVENT_H__
