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

#ifndef __voSink_H__
#define __voSink_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voIndex.h>
#include <voString.h>
#include <voMem.h>

#include <voAudio.h>
#include <voVideo.h>
#include <voFile.h>
#include <voType.h>


/**
* Parameter ID
*/
#define	VO_PID_SINK_BASE					0x44000000						/*!< the base param ID for sink modules */
#define	VO_PID_SINK_SIZE					(VO_PID_SINK_BASE | 0x0001)		/*!< <S>Set the limitation size. VO_U32 * type */
#define VO_PID_SINK_EXTDATA					(VO_PID_SINK_BASE | 0x0002)		/*!< Set video/audio extension data */
#define VO_PID_SINK_INDEXONLY				(VO_PID_SINK_BASE | 0x0003)		/*!< Writer index first or writer index and data */
#define VO_PID_SINK_FRAME_INFO				(VO_PID_SINK_BASE | 0x0004)		/*!< Set file frames information, parameter FramesInfo * */
#define VO_PID_SINK_INTRA_DETECT_IFRAME		(VO_PID_SINK_BASE | 0x0005)		/*!< Set whether detect I Frame in mux module */
#define VO_PID_SINK_VIDEO_CHUNK_SIZE		(VO_PID_SINK_BASE | 0x0006)		/*!< Set the max video trunk size VO_U32 * type */
#define VO_PID_SINK_AUDIO_CHUNK_SIZE		(VO_PID_SINK_BASE | 0x0007)		/*!< Set the max audio trunk size VO_U32 * type */
#define VO_PID_SINK_VIDEO_CHUNK_DURATION	(VO_PID_SINK_BASE | 0x0008)		/*!< Set the max video trunk duration VO_U32 * type */
#define VO_PID_SINK_AUDIO_CHUNK_DURATION	(VO_PID_SINK_BASE | 0x0009)		/*!< Set the max audio trunk duration VO_U32 * type */
#define VO_PID_SINK_FLUSH					(VO_PID_SINK_BASE | 0x000A)		/*!< Writer all frames to file */
#define VO_PID_SINK_AV_SYNC_BUFFER_DURATION	(VO_PID_SINK_BASE | 0x000B)		/*!< Max duration for Audio && Video sync buffer */
#define VO_PID_SINK_FORMAT_CHANGE			(VO_PID_SINK_BASE | 0x000C)		/*!< Assemble New foramt A/V */
#define VO_PID_SINK_ASSEMBLE_PAT_PMT		(VO_PID_SINK_BASE | 0x000D)		/*!< Assemble PAT&&PMT for HLS */
#define VO_PID_SINK_INPUT_PTS				(VO_PID_SINK_BASE | 0x000E)		/*!< Input is PTS not timestamp*/
#define VO_PID_SINK_DISABLEAUDIOSTEPSIZE	(VO_PID_SINK_BASE | 0x000F)		/*!< Disable the audio step size*/
/**
* Error code
*/
#define VO_ERR_SINK_OK					VO_ERR_NONE
#define VO_ERR_SINK_BASE				0x86100000
#define VO_ERR_SINK_OPENFAIL			(VO_ERR_SINK_BASE | 0x0001)		/*!< open fail */

/**
 * Sink open parameters
 */
typedef struct
{
	VO_U32				nFlag;				/*!< The param type. */
	VO_PTR				pSinkOP;			/*!< sink operator functions(nFlag = 1, file IO operator pointer). */
	VO_MEM_OPERATOR *	pMemOP;				/*!< memory operator functions. */
	VO_U32				nVideoCoding;		/*!< Video Coding type. */
	VO_U32				nAudioCoding;		/*!< Audio Coding type. */
	VO_U32				nReserve;
	VO_TCHAR *   			strWorkPath;     /*!< Load library work path */
}VO_SINK_OPENPARAM;

/**
 * Sink sample information
 */
typedef struct
{
	VO_PBYTE		Buffer;					/*!< buffer address of sample, null indicate not need get buffer*/
	VO_U32			Size;					/*!< buffer size. the highest bit is key sample or not*/
	VO_S64			Time;					/*!< start time(MS)*/
	VO_U32			Duration;				/*!< duration of sample(MS)*/
	VO_U32			nAV;					/*!< Audio or video buffer. 0 Audio, 1 Video. */
	VO_S64			DTS;					/*!< start time(MS) of decode time,0xFFFFFFFFFFFFFFFF indicate no useable DTS*/
}VO_SINK_SAMPLE;

typedef struct
{
	union
	{
		struct 
		{
			VO_U32 nWidth;   ///< Video Width
			VO_U32 nHeight;  ///< Video Height
			VO_U32 nFrameRate;///<Video Frame Rate,Frame count per second
		}VideoFormat;
		struct 
		{
			VO_U32 nSample_rate;  ///<Sample rate
			VO_U32 nChannels;     ///< Channel count
			VO_U32 nSample_bits;  ///< Bits per sample
		}AudioFormat;
	};

}VO_SINK_MEDIA_INFO;
/**
 * Sink extension data information
 */
typedef struct
{
	VO_PBYTE							Buffer;					/*!< buffer address of extension data*/
	VO_U32								Size;					/*!< buffer size.*/
	VO_U32								nAV;					/*!< Audio or video buffer. 0 Audio, 1 Video. */
}VO_SINK_EXTENSION;

/**
 * Set frame information for count index size
 */
typedef struct 
{
	VO_U32			nVFrames;				/*!< total video frames count */
	VO_U32			nVSyncFrames;			/*!< video sync frames count*/
	VO_U32			nAFrames;				/*!< total audio frames count*/
}VO_SINK_FramesInfo;

/**
 * Sink Writer function set
 */
typedef struct
{
	/**
	 * Open the sink and return sink handle
	 * \param ppHandle [OUT] Return the sink operator handle
	 * \param pSource [IN] The sink source
	 * \param pParam [IN] The sink open param
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Open) (VO_PTR * ppHandle, VO_FILE_SOURCE *pSource, VO_SINK_OPENPARAM * pParam);

	/**
	 * Close the opened sink.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Close) (VO_PTR pHandle);

	/**
	 * add the track buffer
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param pSample [IN] The sample info was filled.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * AddSample) (VO_PTR pHandle, VO_SINK_SAMPLE * pSample);

	/**
	 * Set sink param for special target.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param uID [IN] The param ID.
	 * \param pParam [IN] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * SetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

	/**
	 * Get sink param for special target.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param uID [IN] The param ID.
	 * \param pParam [Out] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * GetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

} VO_SINK_WRITEAPI;

VO_S32 VO_API voGetMP4WriterAPI(VO_SINK_WRITEAPI* pReadHandle, VO_U32 uFlag);
VO_S32 VO_API voGetTSWriterAPI(VO_SINK_WRITEAPI* pReadHandle, VO_U32 uFlag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voSink_H__
