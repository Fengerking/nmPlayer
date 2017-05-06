#ifndef _TS_PARSER_BASE_DEF_H_
#define _TS_PARSER_BASE_DEF_H_

#include "voYYDef_TS.h"
#include "voTSParser.h"
#include "fMacros.h"





/**
* Stream Media Type
*/
typedef enum
{
    VO_TS_STREAM_MEDIA_TYPE_VIDEO,
    VO_TS_STREAM_MEDIA_TYPE_AUDIO,
    VO_TS_STREAM_MEDIA_TYPE_SUBTITLE,
    VO_TS_STREAM_MEDIA_TYPE_PRIVATE,
}VO_TS_STREAM_TYPE;


/**
* time stamp, mul/div second
*/
typedef struct
{
	unsigned long long mul;
	unsigned int div;
}
VOSTREAMPARSETIMESTAMP;


typedef struct
{
	int length;  /*!< Frame length, in byte. If the length is not known, set it to 0 */
	VOSTREAMPARSETIMESTAMP timestamp;  /*!< Time stamp of the frame */
}
VOSTREAMPARSEFRAMEINFO;

/**
* Program index data
*/ 

typedef struct
{
	int pid_count;  /*!< How many PIDs for program index in this stream */
	int pids[16];   /*!< The PIDs for program index */
}
VOSTREAMPARSEPROGRAMINDEX;


/**
* General audio format info
*/
typedef struct
{
	int sample_rate;  /*!< Sample rate */
	int channels;    /*!< Channel count */
	int sample_bits;  /*!< Bits per sample */
	VO_CHAR     audio_language[16]; /*!< language Desc */
}
VOCODECAUDIOFORMAT;

typedef VOCODECAUDIOFORMAT VOAUDIOFORMAT;


/**
* General video format info
*/
typedef struct
{
	int width;   /*!< Width */
	int height;  /*!< Height */
	VO_VIDEO_FRAMETYPE frame_type; /*!< Frame type, such as I frame, P frame */
} VOCODECVIDEOFORMAT;

typedef VOCODECVIDEOFORMAT VOVIDEOFORMAT;


/**
* General subtitle format info
*/
typedef struct
{
	VO_CHAR             subtitle_language[16]; /*!< language Desc */
	unsigned char       uSubTitleType;         /*!< SubTitle SubType */   
	VO_U32      ulPreserved1;          /*!< Preserved1 Field */
	VO_U32      ulPreserved2;          /*!< Preserved2 Field */
} VOCODECSUBTITLEFORMAT;

typedef VOCODECSUBTITLEFORMAT VOSUBTITLEFORMAT;


/**
* Stream info, including format, config, etc
*/
typedef struct
{
	int id; /*!< element stream id */
	bool is_video;
	VO_U32 codec; /*!< Codec type */
	union
	{
		VOVIDEOFORMAT video;  /*!< valid if VOCODEC_ISVIDEO(codec) */
		VOAUDIOFORMAT audio;  /*!< valid if VOCODEC_ISAUDIO(codec) */
		VOSUBTITLEFORMAT subtitle; /*!< valid if VOCODEC_ISSUBTITLE(codec) */
	};
	void* extra_data; /*!< extra data, format specific */
	int extra_size;  /*!< size of extra data */
	int max_frame_len; /*!< max frame length */
	int packet_size; /*!< suggest packet size, for better performance */
    VO_TS_STREAM_TYPE    eStreamMediaType; /*!< suggest the stream media type */
	void* pClosedCaptionDescData; /*!< closed caption desc data*/
	int   iClosedCaptionDescLen;  /*!< closed caption desc length*/
}
VOSTREAMPARSESTREAMINFO;

class VOSTREAMPARSELISTENER
{
public:
	virtual void OnProgramIndex(VOSTREAMPARSEPROGRAMINDEX* pProgramIndex)= 0;
	virtual void OnNewProgram(VOSTREAMPARSEPROGRAMINFO* pProgramInfo)= 0;
	virtual void OnNewProgramEnd()= 0;
	virtual void OnNewStream(VOSTREAMPARSESTREAMINFO* pStreamInfo)= 0;
	virtual void OnStreamChanged(VOSTREAMPARSESTREAMINFO* pStreamInfo)= 0;
	virtual void OnNewStreamEnd()= 0;
	virtual void OnNewFrame(int nStreamId, VOSTREAMPARSEFRAMEINFO* pFrameInfo)= 0;
	virtual void OnFrameData(int nStreamId, void* pData, int nSize)= 0;
	virtual void OnFrameEnd(int nStreamId)= 0;
	virtual void OnFrameError(int nStreamId, int nErrorCode)= 0;
	virtual VO_BYTE* GetFrameBuf(int nStreamId) = 0;
	virtual VO_U32 GetBufferSize(int nStreamId) = 0;
	virtual VO_U32 SetNewBufSize(int nStreamId, VO_U32  ulNewBufferSize) = 0;
	virtual VO_U32 SetNewTimeStamp(int nStreamId, VO_S64  illTimeStamp) = 0;

//	virtual void OnNewFrameUserData(int nStreamId,VO_BYTE *pdata,VO_U32 cbSize) = 0;

};


typedef int		VOSTREAMPARSERETURNCODE;
typedef void*	HVOSTREAMPARSE;

#endif