#ifndef __MKCommon_MediaInfo_h__
#define __MKCommon_MediaInfo_h__

#include <MKBase/Platform.h>
#include <MKBase/Assert.h>
#include <MKBase/Type.h>
#include <MKBase/FBuf.h>
#include <MKBase/Time.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* INTERFACE: MediaInfo                                                       */
/******************************************************************************/

typedef struct _MK_MediaInfo MK_MediaInfo;

/******************************************************************************/

/*
 * Return the starting time for the clip. If the start time is unknown a start
 * time of zero will be assumed and returned.
 */
MK_INLINE MK_Time MK_MediaInfo_GetBeginTime(const MK_MediaInfo* aInfo);

/*
 * Return the ending time for the clip. If the end time is unknown zero will be
 * returned.
 */
MK_INLINE MK_Time MK_MediaInfo_GetEndTime(const MK_MediaInfo* aInfo);

/******************************************************************************/
/* INTERFACE: TrackInfo                                                       */
/******************************************************************************/

typedef enum
{
    kMediaType_Unknown = 0,

    kMediaType_Audio = 1,
    kMediaType_Video = 2,

} MK_MediaType;

/******************************************************************************/

typedef enum
{
    kMediaFormat_Unknown = 0,

    kMediaFormat_MP4A = 100,
    kMediaFormat_AMR  = 101,

    kMediaFormat_H264 = 200,
    kMediaFormat_MP4V = 201,

} MK_MediaFormat;

/******************************************************************************/

typedef struct _MK_TrackInfo MK_TrackInfo;

/******************************************************************************/

/*
 * Return track media type (see MK_MediaType enum for more info).
 */
MK_INLINE MK_MediaType MK_TrackInfo_GetMediaType(const MK_TrackInfo* aInfo);

/*
 * Return the tracks media format (see MK_MediaFormat enum for more info).
 */
MK_INLINE MK_MediaFormat MK_TrackInfo_GetMediaFormat(const MK_TrackInfo* aInfo);

/*
 * Return the original time resolution of the input data.
 *
 * NOTE: This is not the resulution used by MK_Time. We may transition to using
 * timestamps with the original resolution in the future.
 */
MK_INLINE MK_U32 MK_TrackInfo_GetTimeRes(const MK_TrackInfo* aInfo);

/*
 * Return the starting time for the track. If the start time is unknown a start
 * time of zero will be assumed, and returned.
 */
MK_INLINE MK_Time MK_TrackInfo_GetBeginTime(const MK_TrackInfo* aInfo);

/*
 * Return the ending time for the track. If the end time is unknown zero will be
 * returned.
 */
MK_INLINE MK_Time MK_TrackInfo_GetEndTime(const MK_TrackInfo* aInfo);

/******************************************************************************/
/* INTERFACE: Audio TrackInfo                                                 */
/******************************************************************************/

/*
 * Return the number of bits per audio sample (if applicable). Returns a
 * negative value if the bitcount is unknown.
 */
MK_INLINE MK_S32 MK_TrackInfo_GetAudioSampleBits(const MK_TrackInfo* aInfo);

/*
 * Return the sample rate of the audio. Returns a negative value if the
 * samplerate is unknown.
 */
MK_INLINE MK_S32 MK_TrackInfo_GetAudioSampleRate(const MK_TrackInfo* aInfo);

/*
 * Return the number of audio channels. Returns a negative value if the channel
 * count is unknown.
 */
MK_INLINE MK_S32 MK_TrackInfo_GetAudioChannels(const MK_TrackInfo* aInfo);

/******************************************************************************/

/*
 * Get AudioSpecificConfig for a MP4A track. Returns the length required for a
 * full copy of the ASC to be returned (even if it is larger than aSz).
 */
MK_U32 MK_TrackInfo_GetMP4AConfASC(const MK_TrackInfo* aInfo, MK_U8* aBuf, MK_U32 aSz);

/*
 * Get StreamMuxConfig for a MP4A track. Returns the length required for a
 * full copy of the SMC to be returned (even if it is larger than aSz).
 */
MK_U32 MK_TrackInfo_GetMP4AConfSMC(const MK_TrackInfo* aInfo, MK_U8* aBuf, MK_U32 aSz);

/******************************************************************************/

/*
 * TODO: Add functions to get decoder config?
 *
 * MP4A: anything more?
 * AMR:  none? narrowband vs. wideband? or is that a MediaFormat?
 */

/******************************************************************************/
/* INTERFACE: Video TrackInfo                                                 */
/******************************************************************************/

/*
 * Return the number of depth bits of the video pixels (if applicable). Returns
 * a negative value if the depth is unknown.
 */
MK_INLINE MK_S32 MK_TrackInfo_GetVideoDepthBits(const MK_TrackInfo* aInfo);

/*
 * Returns the average framerate of the video (in f/1000s). A negative value is
 * returned if the framerate is unknown.
 */
MK_INLINE MK_S32 MK_TrackInfo_GetVideoFrameRate(const MK_TrackInfo* aInfo);

/*
 * Returns the width of the video. A negative value is return if the width is
 * unknown.
 */
MK_INLINE MK_S32 MK_TrackInfo_GetVideoWidth(const MK_TrackInfo* aInfo);

/*
 * Returns the height of the video. A negative value is return if the height is
 * unknown.
 */
MK_INLINE MK_S32 MK_TrackInfo_GetVideoHeight(const MK_TrackInfo* aInfo);

/******************************************************************************/

/*
 * Return the number of SequenceParameterSets available for the H264 track.
 */
MK_INLINE MK_U32 MK_TrackInfo_GetH264ConfSPSCount(const MK_TrackInfo* aInfo);

/*
 * Get a SequenceParameterSet for the H264 track by index. Returns the length
 * required for a full copy of the SPS to be returned (even if it is larger than
 * aSz). Returns 0 if there were no PPS at the given index.
 */
MK_U32 MK_TrackInfo_GetH264ConfSPS(const MK_TrackInfo* aInfo, MK_U32 aIdx, MK_U8* aBuf, MK_U32 aSz);

/*
 * Return the number of PictureParameterSets available for the H264 track.
 */
MK_INLINE MK_U32 MK_TrackInfo_GetH264ConfPPSCount(const MK_TrackInfo* aInfo);

/*
 * Get a PictureParameterSet for the H264 track by index. Returns the length
 * required for a full copy of the PPS to be returned (even if it is larger than
 * aSz). Returns 0 if there were no PPS at the given index.
 */
MK_U32 MK_TrackInfo_GetH264ConfPPS(const MK_TrackInfo* aInfo, MK_U32 aIdx, MK_U8* aBuf, MK_U32 aSz);

/******************************************************************************/

/*
 * TODO: Add more functions to get decoder config stuff?
 *
 * MP4V: unsure of format, could differ between MP4V-ES and mpeg4-generic
 * H264: SPS/PPS (H264) in different output formats (avc1/avcC box?, etc...)
 */

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

struct _MK_MediaInfo
{
    MK_Time BeginTime;
    MK_Time EndTime;
};

/******************************************************************************/

MK_INLINE MK_Time MK_MediaInfo_GetBeginTime(const MK_MediaInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo);
    return aInfo->BeginTime;
}

MK_INLINE MK_Time MK_MediaInfo_GetEndTime(const MK_MediaInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo);
    return aInfo->EndTime;
}

/******************************************************************************/

struct _MK_TrackInfo
{
    MK_MediaType Type;
    MK_MediaFormat Format;

    MK_U32 TimeRes;

    MK_Time BeginTime;
    MK_Time EndTime;
};

/******************************************************************************/

MK_INLINE MK_MediaType MK_TrackInfo_GetMediaType(const MK_TrackInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo);
    return aInfo->Type;
}

MK_INLINE MK_MediaFormat MK_TrackInfo_GetMediaFormat(const MK_TrackInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo);
    return aInfo->Format;
}

MK_INLINE MK_U32 MK_TrackInfo_GetTimeRes(const MK_TrackInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo);
    return aInfo->TimeRes;
}

MK_INLINE MK_Time MK_TrackInfo_GetBeginTime(const MK_TrackInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo);
    return aInfo->BeginTime;
}

MK_INLINE MK_Time MK_TrackInfo_GetEndTime(const MK_TrackInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo);
    return aInfo->EndTime;
}

/******************************************************************************/

typedef struct _MK_TrackInfo_Audio
{
    MK_TrackInfo Common;

    MK_S32 SampleBits;
    MK_S32 SampleRate;
    MK_S32 Channels;

} _MK_TrackInfo_Audio;

/******************************************************************************/

MK_INLINE MK_S32 MK_TrackInfo_GetAudioSampleBits(const MK_TrackInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo && kMediaType_Audio == aInfo->Type);
    return MK_Cast(const _MK_TrackInfo_Audio*, aInfo)->SampleBits;
}

MK_INLINE MK_S32 MK_TrackInfo_GetAudioSampleRate(const MK_TrackInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo && kMediaType_Audio == aInfo->Type);
    return MK_Cast(const _MK_TrackInfo_Audio*, aInfo)->SampleRate;
}

MK_INLINE MK_S32 MK_TrackInfo_GetAudioChannels(const MK_TrackInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo && kMediaType_Audio == aInfo->Type);
    return MK_Cast(const _MK_TrackInfo_Audio*, aInfo)->Channels;
}

/******************************************************************************/

typedef struct _MK_TrackInfo_MP4A
{
    _MK_TrackInfo_Audio Audio;

    /* NOTE: Only one needs to be filled in (the other can be generated) */
    MK_FBuf SMC;
    MK_FBuf ASC;

} _MK_TrackInfo_MP4A;

/******************************************************************************/

typedef struct _MK_TrackInfo_Video
{
    MK_TrackInfo Common;

    MK_S32 DepthBits;
    MK_S32 FrameRate;
    MK_S32 Width;
    MK_S32 Height;

} _MK_TrackInfo_Video;

/******************************************************************************/

MK_INLINE MK_S32 MK_TrackInfo_GetVideoDepthBits(const MK_TrackInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo && kMediaType_Video == aInfo->Type);
    return MK_Cast(const _MK_TrackInfo_Video*, aInfo)->DepthBits;
}

MK_INLINE MK_S32 MK_TrackInfo_GetVideoFrameRate(const MK_TrackInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo && kMediaType_Video == aInfo->Type);
    return MK_Cast(const _MK_TrackInfo_Video*, aInfo)->FrameRate;
}

MK_INLINE MK_S32 MK_TrackInfo_GetVideoWidth(const MK_TrackInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo && kMediaType_Video == aInfo->Type);
    return MK_Cast(const _MK_TrackInfo_Video*, aInfo)->Width;
}

MK_INLINE MK_S32 MK_TrackInfo_GetVideoHeight(const MK_TrackInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo && kMediaType_Video == aInfo->Type);
    return MK_Cast(const _MK_TrackInfo_Video*, aInfo)->Height;
}

/******************************************************************************/

typedef struct _MK_TrackInfo_H264
{
    _MK_TrackInfo_Video Video;

    MK_U8  mProfileIndication;
    MK_U8  mProfileCompability;
    MK_U8  mLevelIndication;

    MK_U32 SPSCnt;
    MK_FBuf* SPS;

    MK_U32 PPSCnt;
    MK_FBuf* PPS;

} _MK_TrackInfo_H264;

/******************************************************************************/

MK_INLINE MK_U32 MK_TrackInfo_GetH264ConfSPSCount(const MK_TrackInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo && kMediaType_Video == aInfo->Type && kMediaFormat_H264 == aInfo->Format);
    return MK_Cast(const _MK_TrackInfo_H264*, aInfo)->SPSCnt;
}

MK_INLINE MK_U32 MK_TrackInfo_GetH264ConfPPSCount(const MK_TrackInfo* aInfo)
{
    MK_ASSERT(NULL != aInfo && kMediaType_Video == aInfo->Type && kMediaFormat_H264 == aInfo->Format);
    return MK_Cast(const _MK_TrackInfo_H264*, aInfo)->PPSCnt;
}

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
