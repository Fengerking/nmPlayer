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

/**************************************************************************************************************************/
/* Program																                                                  */
/* Now we have three defines: Stream, Sub Stream , Track				                                                  */
/* For Program, it contains several Streams											                                      */
/* For Stream, Stream is one playable stream, it contains several video or audio tracks.                                  */
/*					Different stream in same program have different bitrate											      */
/* For Track, it is video or audio track								                                                  */
/**************************************************************************************************************************/


#ifndef __VO_SOURCE2_H__

#define __VO_SOURCE2_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voAudio.h"
#include "voVideo.h"
#include "voIndex.h"
#include "voType.h"
#include "voFile.h"
#include "voSource2Ext.h"

/**
*Statice Value
*/
#define MAXURLLEN	2048
#define UNAVALIABLETIME 0xffffffffffffffffll

/**
* Error code
*/
#define VO_RET_SOURCE2_OK								VO_ERR_NONE
#define VO_RET_SOURCE2_BASE								0x86000000

#define VO_RET_SOURCE2_FAIL								VO_RET_SOURCE2_BASE
#define VO_RET_SOURCE2_OPENFAIL							(VO_RET_SOURCE2_BASE | 0x0001)		/*!< open fail */
#define VO_RET_SOURCE2_NEEDRETRY						(VO_RET_SOURCE2_BASE | 0x0002)		/*!< can not finish operation, but maybe you can finish it next time */
#define VO_RET_SOURCE2_END								(VO_RET_SOURCE2_BASE | 0x0003)		/*!< play end */
#define VO_RET_SOURCE2_CONTENTENCRYPT					(VO_RET_SOURCE2_BASE | 0x0004)		/*!< content is encrypt, only can playback after some operations */
#define VO_RET_SOURCE2_CODECUNSUPPORT					(VO_RET_SOURCE2_BASE | 0x0005)		/*!< codec not support */
#define VO_RET_SOURCE2_PLAYMODEUNSUPPORT				(VO_RET_SOURCE2_BASE | 0x0006)		/*!< play mode not support */
#define VO_RET_SOURCE2_ERRORDATA						(VO_RET_SOURCE2_BASE | 0x0007)		/*!< file has error data */
#define VO_RET_SOURCE2_SEEKFAIL							(VO_RET_SOURCE2_BASE | 0x0008)		/*!< seek not support */
#define VO_RET_SOURCE2_FORMATUNSUPPORT					(VO_RET_SOURCE2_BASE | 0x0009)		/*!< file format not support */
#define VO_RET_SOURCE2_TRACKNOTFOUND					(VO_RET_SOURCE2_BASE | 0x000A)		/*!< track can not be found */
#define VO_RET_SOURCE2_EMPTYPOINTOR						(VO_RET_SOURCE2_BASE | 0x000B)		/*!< empty pointor error */
#define VO_RET_SOURCE2_NOIMPLEMENT						(VO_RET_SOURCE2_BASE | 0x000C)		/*!< IMPLEMENT */
#define VO_RET_SOURCE2_NODOWNLOADOP						(VO_RET_SOURCE2_BASE | 0x000D)		/*!< no set download pointer */
#define VO_RET_SOURCE2_NOLIBOP							(VO_RET_SOURCE2_BASE | 0x000E)		/*!< no set lib op  pointer */
#define VO_RET_SOURCE2_OUTPUTNOTFOUND					(VO_RET_SOURCE2_BASE | 0x000F)		/*!< Can not find such output type in this source */
#define VO_RET_SOURCE2_INPUTDATASMALL					(VO_RET_SOURCE2_BASE | 0x0010)		/*!< Indicate the data is not enough to parse */
#define VO_RET_SOURCE2_OUTPUTNOTAVALIBLE				(VO_RET_SOURCE2_BASE | 0x0011)		/*!< This output is not avalible right now */
#define VO_RET_SOURCE2_ONECHUNKFAIL						(VO_RET_SOURCE2_BASE | 0x0012)		/*!< One chunk retry count is more than IO_CONTINUE_TOLERATE_COUNTS in IOError.h */
#define VO_RET_SOURCE2_LINKFAIL							(VO_RET_SOURCE2_BASE | 0x0013)		/*!< The link causes failed, maybe download(checked by ErrorHandler) or other situation. */
#define VO_RET_SOURCE2_CHUNKDROPPED						(VO_RET_SOURCE2_BASE | 0x0014)		/*!< Whatever chunk is been dropped */
#define VO_RET_SOURCE2_CHUNKSKIPPED						(VO_RET_SOURCE2_BASE | 0x0015)		/*!< Whatever chunk is been skipped */
#define VO_RET_SOURCE2_OUTPUTDATASMALL					(VO_RET_SOURCE2_BASE | 0x0016)		/*!< Indicate the buffer is not enough to store data  */
#define VO_RET_SOURCE2_CHUNKPARTERROR					(VO_RET_SOURCE2_BASE | 0x0017)		/*!< Indicate the chunk occurs part error  */
#define VO_RET_SOURCE2_INVALIDPARAM						(VO_RET_SOURCE2_BASE | 0x0018)		/*!< Indicate the parameter is wrong */
#define VO_RET_SOURCE2_SEEKMODEUNSUPPORTED				(VO_RET_SOURCE2_BASE | 0x0019)		/*!< Indicate unsupported VO_ADAPTIVESTREAMPARSER_SEEKMODE_XXX */
#define VO_RET_SOURCE2_ONELINKFAIL						(VO_RET_SOURCE2_BASE | 0x001A)		/*!<The link failed in one bitrate, if failed acceptance is more than 1, ba to another one*/
#define VO_RET_SOURCE2_DRMERROR						(VO_RET_SOURCE2_BASE | 0x001B)		/*!<the chunk get drm error*/
#define VO_RET_SOURCE2_RESOLUTIONFORBIDDEN				(VO_RET_SOURCE2_BASE | 0x001C)		/*!< Resolution forbidden for lg */
#define VO_RET_SOURCE2_UPDATEURL							(VO_RET_SOURCE2_BASE | 0x001D)		/*!< Indicate the url will be updated, only for E for now */

#define VO_RET_SOURCE2_BA_BASE							(VO_RET_SOURCE2_BASE | 0x00010000)	/*!< The return offset of BA*/
#define VO_RET_SOURCE2_ADAPTIVESTREAMING_BASE			(VO_RET_SOURCE2_BASE | 0x00020000)	/*!< The return offset of AdaptiveStreaming */
#define VO_RET_SOURCE2_DS_BASE							(VO_RET_SOURCE2_BASE | 0x00030000)	/*!< The return offset of DS ( controller or source wrapper ) */

#define VO_RET_SOURCE2_ADAPTIVESTREAMING_CHUNK_SKIP		(VO_RET_SOURCE2_ADAPTIVESTREAMING_BASE | 0x0001)	/*!< Indicate controller should or have skiped one chunk */
#define VO_RET_SOURCE2_ADAPTIVESTREAMING_FORCETIMESTAMP	(VO_RET_SOURCE2_ADAPTIVESTREAMING_BASE | 0x0002)	/*!< Indicate controller should use the timestamp in the function param */
#define VO_RET_SOURCE2_ADAPTIVESTREAMING_DATA_IN_MUXED_SEGMENT	(VO_RET_SOURCE2_ADAPTIVESTREAMING_BASE | 0x0003)	/*!< Indicate that the media data contained in the muxed segment */
/**
*Param
*/
#define VO_PID_SOURCE2_BASE								0x47000000
#define VO_PID_SOURCE2_LIBOP							(VO_PID_SOURCE2_BASE | 0x0001)	/*!< <s> set lib operator,  VO_SOURCE2_LIB_FUNC* */
#define VO_PID_SOURCE2_IO								(VO_PID_SOURCE2_BASE | 0x0002)	/*!< <s> set the io, it can be local file or http file, VODOWNLOAD_FUNC* */
#define VO_PID_SOURCE2_DRMCALLBACK						(VO_PID_SOURCE2_BASE | 0x0003)	/*!< <S> set the drm callback into the source, VO_SOURCEDRM_CALLBACK2* defined in voDRM2.h */
#define VO_PID_SOURCE2_EVENTCALLBACK					(VO_PID_SOURCE2_BASE | 0x0004)	/*!< <S> set the event callback into the source, VO_SOURCE2_EVENTCALLBACK* */
#define VO_PID_SOURCE2_SAMPLECALLBACK					(VO_PID_SOURCE2_BASE | 0x0005)	/*!< <S> set the sample callback into the source, VO_SOURCE2_SAMPLECALLBACK* */
#define VO_PID_SOURCE2_BITRATEADAPTATIONCALLBACK		(VO_PID_SOURCE2_BASE | 0x0006)	/*!< <S> set the BA callback into the source, * */
#define VO_PID_SOURCE2_CC_AUTO_SWITCH_DURATION			(VO_PID_SOURCE2_BASE | 0x0007)	/*!< <S> set the BA auto switch from 708 to 608 interval */
#define VO_PID_SOURCE2_ENABLECC							(VO_PID_SOURCE2_BASE | 0x0008)	/*!< <S> set if the CC is enabled, VO_BOOL* */
#define VO_PID_SOURCE2_ENABLELOG						(VO_PID_SOURCE2_BASE | 0x0009)	/*!< <s> set the volog function pointor into the source, VOLOG_PRINT */
#define VO_PID_SOURCE2_CPUINFO							(VO_PID_SOURCE2_BASE | 0x000a)	/*!< <s> set the cpuinfo into the source VO_SOURCE2_CPU_INFO* */
#define VO_PID_SOURCE2_BACAP							(VO_PID_SOURCE2_BASE | 0x000b)	/*!< <s> set the BA cap into the source VO_SOURCE2_CAP_DATA* */
#define VO_PID_SOURCE2_HTTPVERIFICATIONCALLBACK			(VO_PID_SOURCE2_BASE | 0x000c)	/*!< <s> set the http verification callback VO_SOURCE2_IO_HTTP_VERIFYCALLBACK* */
#define VO_PID_SOURCE2_DOHTTPVERIFICATION				(VO_PID_SOURCE2_BASE | 0x000d)	/*!< <s> set the param to start HTTP verification VO_SOURCE2_VERIFICATIONINFO* */
#define VO_PID_SOURCE2_RTSPINITPARAM					(VO_PID_SOURCE2_BASE | 0x000f)	/*!< <s> set the init param of RTSP client VO_SOURCE2_RTSP_INIT_PARAM* */
#define VO_PID_SOURCE2_CONFIGSTRING						(VO_PID_SOURCE2_BASE | 0x0010)	/*!< <s> set the config string */
#define VO_PID_SOURCE2_BA_STARTCAP						(VO_PID_SOURCE2_BASE | 0x0011)	/*!< <s> set the BA start bitrate VO_SOURCE2_CAP_DATA* */
#define VO_PID_SOURCE2_DOWNLOAD_FAIL_MAX_TOLERANT_COUNT	(VO_PID_SOURCE2_BASE | 0x0012)	/*!< <s> set the max download fail tolerant count* */
#define VO_PID_SOURCE2_CPU_BA_WORKMODE					(VO_PID_SOURCE2_BASE | 0x0013)	/*!< <s> set the CPU BA work mode */
#define VO_PID_SOURCE2_HTTPAUTHENTICATIONSTRING			(VO_PID_SOURCE2_BASE | 0x0014)	/*!< <s> set the HTTP Authentication String char* , like username:password */
#define VO_PID_SOURCE2_DRMPOINTER					    (VO_PID_SOURCE2_BASE | 0x0015)	/*!< <S> set the drm api pointer into the fileparser, VO_StreamingDRM_API* */
#define VO_PID_SOURCE2_IOPOINTER					    (VO_PID_SOURCE2_BASE | 0x0016)	/*!< <S> set the Source2 IO api pointer into the fileparser, VO_SOURCE2_IO_API* */
#define VO_PID_SOURCE2_DRMIO							(VO_PID_SOURCE2_BASE | 0x0017)	/*!< <S> set the drm information into the source*/
#define VO_PID_SOURCE2_PROGRAM_TYPE						(VO_PID_SOURCE2_BASE | 0x0018)	/*!< <S> set the program type, VO_SOURCE2_PROGRAM_TYPE* */
#define VO_PID_SOURCE2_BUFFER_STARTBUFFERINGTIME		(VO_PID_SOURCE2_BASE | 0x0019)	/*!< <S> set the start buffering time, VO_U32* */
#define VO_PID_SOURCE2_BUFFER_BUFFERINGTIME				(VO_PID_SOURCE2_BASE | 0x001a)	/*!< <S> set the buffering time, VO_U32* */
#define VO_PID_SOURCE2_SEEKRANGEINFO					(VO_PID_SOURCE2_BASE | 0x001b)	/*!< <G> get the SeekInfo, VO_SOURCE2_SEEK_INFO* */
#define VO_PID_SOURCE2_ACTUALFILESIZE					(VO_PID_SOURCE2_BASE | 0x001c)	/*!< <S> set the actual file size VO_U64* */
#define VO_PID_SOURCE2_WORKPATH							(VO_PID_SOURCE2_BASE | 0x001d)	/*!< <S> set the dll load path VO_TCHAR* */
#define VO_PID_SOURCE2_BA_WORKMODE						(VO_PID_SOURCE2_BASE | 0x001e)	/*!< <s/g> set/get the BA work mode, VO_SOURCE2_BAMODE* */
#define VO_PID_SOURCE2_RTSP_STATS						(VO_PID_SOURCE2_BASE | 0x001f)	/*!< <g> get the rtsp module status value , VO_SOURCE2_RTSP_STATS* */
#define VO_PID_SOURCE2_LOWLATENCYMODE					(VO_PID_SOURCE2_BASE | 0x0020)	/*!< <s> set source module into low latency mode */
#define VO_PID_SOURCE2_AUDIOLANGUAGE					(VO_PID_SOURCE2_BASE | 0x0021)	/*!< <s> not avalible right now */
#define VO_PID_SOURCE2_SUBTITLELANGUAGE					(VO_PID_SOURCE2_BASE | 0x0022)	/*!< <s> not avalible right now */
#define VO_PID_SOURCE2_BUFFER_MAXBUFFERSIZE				(VO_PID_SOURCE2_BASE | 0x0023)	/*!< <s> set source module max buffer size VO_U32* unit is ms */
#define VO_PID_SOURCE2_BA_CPUBADISABLE					(VO_PID_SOURCE2_BASE | 0x0024)	/*!< <s> disable CPU BA VO_BOOL* */
#define VO_PID_SOURCE2_HTTPHEADER						(VO_PID_SOURCE2_BASE | 0x0025)	/*!< <s> set the HTTP Header VO_SOURCE2_HTTPHEADER* */
#define VO_PID_SOURCE2_IO_CBFUNC						(VO_PID_SOURCE2_BASE | 0x0026)	/*!< <s> set the IO callback VO_SOURCE2_IO_HTTPCALLBACK* */
#define VO_PID_SOURCE2_HTTPPROXYINFO					(VO_PID_SOURCE2_BASE | 0x0027)	/*!< <s> set the HTTP Proxy VO_SOURCE2_HTTPPROXYINFO* */
#define VO_PID_SOURCE2_GETTHUMBNAIL						(VO_PID_SOURCE2_BASE | 0x0028)	/*!< <g> get the thumbnail VO_SOURCE2_THUMBNAILINFO* */
#define VO_PID_SOURCE2_LIVELATENCY						(VO_PID_SOURCE2_BASE | 0x0029)	/*!< <s> set the live latency value VO_U64* the unit is ms */
#define VO_PID_SOURCE2_LGE_VIDEO_CONTENT				(VO_PID_SOURCE2_BASE | 0x0030)	/*!< <G> get whether this is LG content, !!!FOR LG ONLY!!!! . VO_U32* */
#define VO_PID_SOURCE2_PERIOD2TIME						(VO_PID_SOURCE2_BASE | 0x0031)	/*!< <s> convert from period sequence number to timestamp , VO_SOURCE2_PERIODTIMEINFO* */
#define VO_PID_SOURCE2_SOCKETREGISTER					(VO_PID_SOURCE2_BASE | 0x0032)	/*!< <s> set the socket register, VO_SOURCE2_SOCKETTAGREG* */
#define VO_PID_SOURCE2_PROXYCHECK						(VO_PID_SOURCE2_BASE | 0x0033)	/*!< <s> set the proxy check to check if we should use proxy , VO_SOURCE2_PROXYCHECK* */
#define VO_PID_SOURCE2_APPLICATION_SUSPEND				(VO_PID_SOURCE2_BASE | 0x0034)	/*!< <s> set application suspend status to network layer* */
#define VO_PID_SOURCE2_APPLICATION_RESUME				(VO_PID_SOURCE2_BASE | 0x0035)	/*!< <s> set application resum status to network layer* */
#define VO_PID_SOURCE2_PLAYBACK_SPEED					(VO_PID_SOURCE2_BASE | 0x0036)      /*!<S>set playback speed*/
#define VO_PID_SOURCE2_TOLERANT_ERROR_NUMBER			(VO_PID_SOURCE2_BASE | 0x0037)	/*<S>set max tolerance error number */
#define VO_PID_SOURCE2_BITRATE_THRESHOLD				(VO_PID_SOURCE2_BASE | 0x0038)	/*<S>Set upper/lower bitrate threshold for adaptation, VO_SOURCE2_BA_THRESHOLD*/				
#define VO_PID_SOURCE2_ENABLE_RTSP_HTTP_TUNNELING       (VO_PID_SOURCE2_BASE | 0x0039)  /*!< Enable/Disable RTSP over HTTP tunneling. The default is disable(0). int* */
#define VO_PID_SOURCE2_RTSP_OVER_HTTP_CONNECTION_PORT	(VO_PID_SOURCE2_BASE | 0x0040)	/*!< Set port number for RTSP over HTTP tunneling. int* */
#define VO_PID_SOURCE2_RESOLUTION_FORBIDDEN				(VO_PID_SOURCE2_BASE | 0x0041) 	/*<S>set max resolution that can be supported*/
#define VO_PID_SOURCE2_MASTER_DOWNLOAD_RETRY_TIMES	(VO_PID_SOURCE2_BASE | 0x0042) 	/*<S>set max download master retry times*/
#define VO_PID_SOURCE2_PlAYLIST_DOWNLOAD_RETRY_TIMES	(VO_PID_SOURCE2_BASE | 0x0043) 	/*<S>set max download playlist retry times*/
#define VO_PID_SOURCE2_TRUNK_DOWNLOAD_RETRY_TIMES		(VO_PID_SOURCE2_BASE | 0x0044) 	/*<S>set max download trunk retry times*/
#define VO_PID_SOURCE2_UPDATE_SOURCE_URL				(VO_PID_SOURCE2_BASE | 0x0045)	/*<S>set updated source url */
#define VO_PID_SOURCE2_HTTP_RETRY_TIMEOUT				(VO_PID_SOURCE2_BASE | 0x0046)	/*<S>set HTTP connection retry timeout, The default is 120 seconds. Setting the value to -1 disables the timeout so that the player will keep retrying, until the connection is established again. int* */
#define VO_PID_SOURCE2_DEFAULT_AUDIO_LANGUAGE			(VO_PID_SOURCE2_BASE | 0x0047)	/*!< Set default audio language, char*, refer to ISO 639-2 code */
#define VO_PID_SOURCE2_DEFAULT_SUBTITLE_LANGUAGE		(VO_PID_SOURCE2_BASE | 0x0048)	/*!< Set default subtitle language, char*, refer to ISO 639-2 code */
#define VO_PID_SOURCE2_SOURCE_IO_API						(VO_PID_SOURCE2_BASE | 0x0049)	/*!< Set SourceIO API, VO_SOURCE2_IO_API*/
#define VO_PID_SOURCE2_SOCKET_CONNECTION_TYPE			(VO_PID_SOURCE2_BASE | 0x0050)	/*!< Set RTSP channel type, refer to VO_SOURCE2_RTSP_INIT_PARAM*/
#define VO_PID_SOURCE2_MIN_BUFFTIME						(VO_PID_SOURCE2_BASE | 0x0051)	/*!< Set Min buffer time, refer to VO_SOURCE2_RTSP_INIT_PARAM*/
#define VO_PID_SOURCE2_RTSP_CONNECTION_PORT				(VO_PID_SOURCE2_BASE | 0x0052)	/*!< Set RTSP Connect Port, refer to VO_SOURCE2_RTSP_INIT_PARAM */
#define VO_PID_SOURCE2_EXTERNAL_HAS_BUFFER				(VO_PID_SOURCE2_BASE | 0x0053)	/*!<S>set there is external buffer out of controller, for example, AV player has its own buffer in player level, VO_BOOL*	*/
#define VO_PID_SOURCE2_ANALYTICS_FUNCTION_SET			(VO_PID_SOURCE2_BASE | 0x0054)   /*!< Set analytics module function set, void* Ref  VOAC_REPORTFUNC*/
#define VO_PID_SOURCE2_CONTENT_DURATION					(VO_PID_SOURCE2_BASE | 0x0055)   /*!< Set content duration for Push Mode PD*/
#define VO_PID_SOURCE2_PREFER_AUDIO_LANGUAGE			(VO_PID_SOURCE2_BASE | 0x0056)   /*!<Set prefer audio language list, char*,refer to ISO 639-2 code*/
#define VO_PID_SOURCE2_PREFER_SUBTITLE_LANGUAGE			(VO_PID_SOURCE2_BASE | 0x0057)   /*!<Set prefer subtitle language list, char*, refer to ISO 639-2 code*/
#define VO_PID_SOURCE2_RTSP_MAX_SOCKET_ERROR_COUNT		(VO_PID_SOURCE2_BASE | 0x0058)   /*!<Set rtsp maximum socket errors while receiving data from server*/
#define VO_PID_SOURCE2_SSLAPI							(VO_PID_SOURCE2_BASE | 0x0100)	/*!< <S> set the open ssl api vosslapi * */

#define VO_PID_SOURCE2_BA_BASE						(VO_PID_SOURCE2_BASE | 0x1000)	/*!< <s> BA private param id offset */
#define VO_PID_SOURCE2_ADAPTIVESTREAMING_BASE		(VO_PID_SOURCE2_BASE | 0x2000)	/*!< <s> Adaptive Streaming private paramid offset */

/**
*event callback ID
*/
#define VO_EVENTID_SOURCE2_BASE						0xcb000000
#define VO_EVENTID_SOURCE2_MEDIATYPECHANGE			(VO_EVENTID_SOURCE2_BASE | 0x0001)				/*!< notify the mediatype has been changed , nParam1 should be the value defined in VO_SOURCE2_MEDIATYPE*/
#define VO_EVENTID_SOURCE2_SEEKCOMPLETE				(VO_EVENTID_SOURCE2_BASE | 0x0002)				/*!< notify seek complete, nParam1 should be the seek return timestamp VO_U64* , nParam2 should be the pointor of VO_RET_SOURCE2_XXXX value */
#define VO_EVENTID_SOURCE2_OPENCOMPLETE				(VO_EVENTID_SOURCE2_BASE | 0x0003)				/*!< notify open complete, nParam1 should be the pointor of VO_RET_SOURCE2_XXXX value */
#define VO_EVENTID_SOURCE2_CUSTOMTAG				(VO_EVENTID_SOURCE2_BASE | 0x0004)				/*!< notify we met one custom tag in the playlist, param 1 will be VO_SOURCE2_CUSTOMERTAGID , param 2 will be depend on VO_SOURCE2_CUSTOMERTAGID */
#define VO_EVENTID_SOURCE2_PROGRAMCHANGED			(VO_EVENTID_SOURCE2_BASE | 0x0005)				/*!< notify the programinfo has been changed in source */
#define VO_EVENTID_SOURCE2_PROGRAMRESET				(VO_EVENTID_SOURCE2_BASE | 0x0006)				/*!< notify the programinfo has been reseted in source */
#define VO_EVENTID_SOURCE2_SEEK2LASTCHUNK			(VO_EVENTID_SOURCE2_BASE | 0x0007)				/*!< notify when seek to the last chunk of play list (NTS link without END tag need change to live mode) */
#define VO_EVENTID_SOURCE2_UPDATEURLCOMPLETE		(VO_EVENTID_SOURCE2_BASE | 0x0008)				/*!< Notify update source URL complete, param 1 will be 0(OK) or event error ID(VO_EVENTID_SOURCE2_ERR_DOWNLOADFAIL,VO_EVENTID_SOURCE2_ERR_DRMFAIL,VO_EVENTID_SOURCE2_ERR_PLAYLISTPARSEERR), int* */
#define VO_EVENTID_SOURCE2_OUTPUT_CONTROL_SETTINGS	(VO_EVENTID_SOURCE2_BASE | 0x0009)				/*!<  notify the output control settings*/
#define VO_EVENTID_SOURCE2_PREFER_AUDIO_LANGUAGE	(VO_EVENTID_SOURCE2_BASE | 0x0010)				/*!< Notify the prefer audio language*/
#define VO_EVENTID_SOURCE2_PREFER_SUBTITLE_LANGUAGE	(VO_EVENTID_SOURCE2_BASE | 0x0011)				/*!< Notify the prefer subtitle language*/

#define VO_EVENTID_SOURCE2_ERR_BASE					0xcbe00000
#define VO_EVENTID_SOURCE2_ERR_CONNECTFAIL			(VO_EVENTID_SOURCE2_ERR_BASE | 0x0001)			/*!< notify connect fail */
#define VO_EVENTID_SOURCE2_ERR_DOWNLOADFAIL			(VO_EVENTID_SOURCE2_ERR_BASE | 0x0002)			/*!< notify download fail */
#define VO_EVENTID_SOURCE2_ERR_DRMFAIL				(VO_EVENTID_SOURCE2_ERR_BASE | 0x0003)			/*!< notify drm engine err */
#define VO_EVENTID_SOURCE2_ERR_PLAYLISTPARSEERR		(VO_EVENTID_SOURCE2_ERR_BASE | 0x0004)			/*!< notify playlist parse error */

#define VO_EVENTID_SOURCE2_RTSP_BASE				0x00010000										/*!< RTSP event callback offset */
#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE	0x00020000										/*!< AdaptiveStreaming event callback offset */

#define VO_EVENTID_SOURCE2_ERR_RTSP_BASE			(VO_EVENTID_SOURCE2_ERR_BASE | VO_EVENTID_SOURCE2_RTSP_BASE)
#define VO_EVENTID_SOURCE2_ERR_RTSP_CONNECT_FAIL	(VO_EVENTID_SOURCE2_ERR_RTSP_BASE | 0x0001)		/*!< notify connect failed in RTSP, error code in param1*/
#define VO_EVENTID_SOURCE2_ERR_RTSP_DESCRIBE_FAIL	(VO_EVENTID_SOURCE2_ERR_RTSP_BASE | 0x0002)		/*!< notify describle failed in RTSP, error code in param1*/
#define VO_EVENTID_SOURCE2_ERR_RTSP_SETUP_FAIL		(VO_EVENTID_SOURCE2_ERR_RTSP_BASE | 0x0003)		/*!< notify setup failed in RTSP, error code in param1*/
#define VO_EVENTID_SOURCE2_ERR_RTSP_PLAY_FAIL		(VO_EVENTID_SOURCE2_ERR_RTSP_BASE | 0x0004)		/*!< notify play failed in RTSP, error code in param1*/
#define VO_EVENTID_SOURCE2_ERR_RTSP_PAUSE_FAIL		(VO_EVENTID_SOURCE2_ERR_RTSP_BASE | 0x0005)		/*!< notify pause failed in RTSP, error code in param1*/
#define VO_EVENTID_SOURCE2_ERR_RTSP_OPTION_FAIL		(VO_EVENTID_SOURCE2_ERR_RTSP_BASE | 0x0006)		/*!< notify option failed in RTSP, error code in param1*/
#define	VO_EVENTID_SOURCE2_ERR_RTP_SOCKET_ERROR		(VO_EVENTID_SOURCE2_ERR_RTSP_BASE | 0x0007)		/*!< notify socket error in RTP channel of RTSP */

//Event FOR HLS v1
#define VO_EVENTID_SOURCE2_HLS_BITRATE_NOTIFY  	    (VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x0001)    /*!< AdaptiveStreaming event callback HLS Bitrate notify */
#define VO_EVENTID_SOURCE2_START_DOWNLOAD_FAIL_WAITING_RECOVER  	    (VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x0002)    /*!< AdaptiveStreaming event callback start download failed waiting recover notify */
#define VO_EVENTID_SOURCE2_DOWNLOAD_FAIL_RECOVER_SUCCESS  	            (VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x0003)    /*!< AdaptiveStreaming event callback download failed recover success notify */
//

//New Event For Adaptive Streaming
#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO						(VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x0011)	/*!< notify info , param 1 will be the value defined in  VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT , param 2 will depend one param 1*/
#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR						(VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x0012)	/*!< notify error, param 1 will be the value defined in VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT , param 2 will depend one param 1*/
#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_WARNING					(VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x0013)	/*!< notify warning, param 1 will be the value defined in VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT , param 2 will depend one param 1*/
#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CUSTOMTAG					(VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x0014)	/*!< notify we met one custom tag in the playlist, param 1 will be VO_SOURCE2_CUSTOMERTAGID , param 2 will be depend on VO_SOURCE2_CUSTOMERTAGID */
//

enum VO_SOURCE2_MEDIATYPE
{
	VO_SOURCE2_MT_PUREAUDIO,
	VO_SOURCE2_MT_PUREVIDEO,
	VO_SOURCE2_MT_AUDIOVIDEO,
	VO_SOURCE2_MT_SUBTITLE,
	VO_SOURCE2_MT_MAX				= VO_MAX_ENUM_VALUE
};

enum VO_SOURCE2_TRACK_TYPE
{
	VO_SOURCE2_TT_AUDIO				= 0X00000001,	/*!< video track*/
	VO_SOURCE2_TT_VIDEO				= 0X00000002,	/*!< audio track*/
	VO_SOURCE2_TT_IMAGE				= 0X00000003,	/*!< image track*/
	VO_SOURCE2_TT_STREAM			= 0X00000004,	/*!< stream track*/
	VO_SOURCE2_TT_SCRIPT			= 0X00000005,	/*!< script track*/
	VO_SOURCE2_TT_HINT				= 0X00000006,	/*!< hint track*/
	VO_SOURCE2_TT_RTSP_VIDEO        = 0X00000007,   /*!< rtsp streaming video track*/
	VO_SOURCE2_TT_RTSP_AUDIO        = 0X00000008,   /*!< rtsp streaming audio track*/
	VO_SOURCE2_TT_SUBTITLE			= 0X00000009,   /*!< sub title track & closed caption*/
	VO_SOURCE2_TT_RICHMEDIA			= 0X0000000B,   /*!< rich media track*/
	VO_SOURCE2_TT_TRACKINFO			= 0X0000000C,	/*!< track info VO_SOURCE2_TRACK_INFO* */
	VO_SOURCE2_TT_CUSTOMTIMEDTAG    = 0X0000000D,   /*!< custom timed tag VO_SOURCE2_CUSTOMERTAG_TIMEDTAG* */
	VO_SOURCE2_TT_AUDIOGROUP		= 0X0000000E,	/*!< if donot know how many audio in, set it*/
	VO_SOURCE2_TT_VIDEOGROUP		= 0X0000000F,	/*!< if donot know how many video in, set it*/
	VO_SOURCE2_TT_SUBTITLEGROUP		= 0X00000010,	/*!< if donot know how many subtitle in ,set it*/
	VO_SOURCE2_TT_MUXGROUP			= 0X00000011,	/*!< if you donot know anything ,set it*/
	VO_SOURCE2_TT_SEGMENTINDEX      = 0X00000012,   /*!< segment index information*/
	VO_SOURCE2_TT_INITDATA 			= 0X00000013, 	/*!< VO_PID_ADAPTIVESTREAMING_INITDATA_INFO from MP4fileparser*/
	VO_SOURCE2_TT_MAX				= VO_MAX_ENUM_VALUE
};

enum VO_SOURCE2_DRM_TYPE
{
	VO_SOURCE2_DRM_PLAYREADY,
	VO_SOURCE2_DRM_AES128,
	VO_SOURCE2_DRM_MAX				= VO_MAX_ENUM_VALUE
};

enum VO_SOURCE2_PROGRAM_TYPE
{
	VO_SOURCE2_STREAM_TYPE_LIVE,
	VO_SOURCE2_STREAM_TYPE_VOD,
	VO_SOURCE2_STREAM_TYPE_UNKNOWN = 255,

	VO_SOURCE2_STREAM_TYPE_MAX				= VO_MAX_ENUM_VALUE
};

enum VO_SOURCE2_VIDEO_ANGLE
{
	VO_SOURCE2_VIDEO_ANGLE_RESERVEFRONT = 50,
	VO_SOURCE2_VIDEO_ANGLE_UPSIDEDOWN,

	VO_SOURCE2_VIDEO_ANGLE_MAX				= VO_MAX_ENUM_VALUE
};

enum VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT
{
	VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_BITRATE_CHANGE = 1,		//param 2 will be the new bitrate, VO_U32* */
	VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_MEDIATYPE_CHANGE = 2,	//param 2 will be the new media type defined in VO_SOURCE2_MEDIATYPE
	VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_PROGRAMTYPE = 3,			//param 2 will be the program type defined in VO_SOURCE2_PROGRAM_TYPE
	VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_CHUNK_BEGINDOWNLOAD = 4,	//param 2 will be VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO* , if the return value of this callback is VO_RET_SOURCE2_ADAPTIVESTREAMING_CHUNK_SKIP, controller should drop this chunk */
	VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_CHUNK_DROPPED = 5 ,		//param 2 will be VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO* */
	VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_CHUNK_DOWNLOADOK = 6,	//param 2 will be VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO* */
	VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_PLAYLIST_DOWNLOADOK = 7,	//param 2 will be VO_SOURCE2_ADAPTIVESTREAMING_PLAYLISTDATA* */
	VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_PROGRAM_CHANGE = 8,		//indicated that Program has been changed in source. when You receive this event,you should getProgramInfo again. param 2 shall be ignored, 
	VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_FILEFORMATSUPPORTED = 9,
	VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_LIVESEEKABLE = 10,		//indicate LIVE streaming can be sought currently
	VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_DISCONTINUESAMPLE = 11,	//indicate this is the first sample from the discontinue chunk, param 2 will be VO_SOURCE2_CHUNK_SAMPLE* */ 
	VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_DRM_OK = 12,
	VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_MAX = 0X7FFFFFFF,
};

enum VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT
{
	VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_PLAYLIST_PARSEFAIL = 1,
	VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_PLAYLIST_UNSUPPORTED = 2,
	VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_UNSUPPORTED = 3,
	VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_DOWNLOADFAIL = 4,
	VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_DRMLICENSEERROR = 5,
	VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_VOLIBLICENSEERROR = 6,
	VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_MAX = 0X7FFFFFFF,
};

enum VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT
{
	VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DOWNLOADERROR = 1,				//param 2 will be VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO* */
	VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_FILEFORMATUNSUPPORTED = 2,
	VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DRMERROR = 3,					//param 2 will be the drm error code
	VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_PLAYLIST_DOWNLOADERROR = 4,			//param 2 will be VO_SOURCE2_ADAPTIVESTREAMING_PLAYLISTDATA* */

	VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_MAX = 0X7FFFFFFF,
};

enum VO_SOURCE2_CUSTOMERTAGID
{
	VO_SOURCE2_CUSTOMERTAGID_TIMEDTAG = 1,					// param2 will be VO_SOURCE2_CUSTOMERTAG_TIMEDTAG* */
};

enum VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE
{
	VO_SOURCE2_ADAPTIVESTREAMING_AUDIO,
	VO_SOURCE2_ADAPTIVESTREAMING_VIDEO,
	VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO,
	VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA,
	VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE,	
	VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX,   	/*!< identify the chunk type is segment index, for DASH 'sidx' */
	VO_SOURCE2_ADAPTIVESTREAMING_INITDATA,        /*!< identify the mpd does not contain "InitRange" & "IndexRange" , for DASH 'moov' & 'sidx' */
	VO_SOURCE2_ADAPTIVESTREAMING_UNKNOWN = 255,

	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE_MAX = 0X7FFFFFFF,
};

enum VO_SOURCE2_CUSTOMERTAG_TIMEDTAG_FLAG
{
	VO_SOURCE2_CUSTOMERTAG_TIMEDTAG_FLAG_STRINGTAG = 0,
	VO_SOURCE2_CUSTOMERTAG_TIMEDTAG_FLAG_ID3TAG = 1,
};

enum VO_SOURCE2_BAMODE
{
	VO_SOURCE2_ADAPTIVESTREAMING_BAMODE_AUTO = 0,
	VO_SOURCE2_ADAPTIVESTREAMING_BAMODE_MANUAL = 1,
};

enum VO_SOURCE2_THUMBNAILMODE
{
	VO_SOURCE2_THUMBNAILMODE_INFOONLY = 0,		/*!< only need the thumbnail info, do not need the thumbnail sample data */
	VO_SOURCE2_THUMBNAILMODE_DATA = 1,			/*!< need the thumbanil sample data */
};

enum VO_SOURCE2_ADAPTIVESTREAMING_CHUNKFLAG
{
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKFLAG_FORMATCHANGE = 0x00000001,
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKFLAG_SMOOTH_ADAPTION = 0x00000002,  /*!<This is obsolete, if not necssary please do not use this flag */
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKFLAG_PROGRAMCHANGE = 0x00000004,	 /*!<Instead of VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_TIMESTAMPRECALCULATE, it indicates that Program maybe changed, it should reset fileparser*/
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKFLAG_CHUNKDROPBEFORE = 0x00000008,		/*!add this flay if the trunk was drop before.*/
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKFLAG_CHUNKSWITCHED = 0x00000010,	/*!< Indicated that the track or bitrate switched started from this chunk */
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKFLAG_SMOOTH_ADAPTIONEX = 0x00000020,	/*!< Indicated that start from this we will do smooth BA */
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKFLAG_DISCONTINUE = 0x00000080,			/*!< Indicated that before this chunk there is a discontinue tag */
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKFLAG_NEEDREF	= 0x00000100,			/*!< Indicated that this chunk may need the ref timestamp to recalculate the timestamp */

	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKFLAG_MAX = 0X7FFFFFFF,
};

enum VO_SOURCE2_SOCKET_USAGE
{
	VO_SOURCE2_SOCKET_USAGE_HTTP = 0x00000001,
	VO_SOURCE2_SOCKET_USAGE_RTSP = 0x00000002,
};

#define VO_SOURCE2_FLAG_INIT_EVENTCALLBACK		0X00000001	/*!<Indicate the init param is VO_SOURCE2_EVENTCALLBACK * */
#define VO_SOURCE2_FLAG_INIT_SAMPLECALLBACK		0X00000002	/*!<Indicate the init param is VO_SOURCE2_SAMPLECALLBACK * */
#define VO_SOURCE2_FLAG_INIT_IO					0x00000004	/*!<Indicate pInitParam contains IO pointor */
#define VO_SOURCE2_FLAG_INIT_ACTUALFILESIZE		0x00000008	/*!<Indicate pInitParam contains actual file size */

#define VO_SOURCE2_FLAG_OPEN_URL				0X00000001	/*!<Indicate the pSource param is a url*/
#define VO_SOURCE2_FLAG_OPEN_SENDBUFFER			0X00000002	/*!<Indicate the you will use sendbuffer api*/
#define VO_SOURCE2_FLAG_OPEN_FILEIO				0X00000004	/*!<Indicate the pSource param is file io VO_FILE_OPERATOR* */
#define VO_SOURCE2_FLAG_OPEN_HANDLE				0X00000008	/*!<Indicate the pSource param is a Handle*/
#define VO_SOURCE2_FLAG_OPEN_ASYNC				0X00000010	/*!<Indicate the open will be async mode */
#define VO_SOURCE2_FLAG_OPEN_PUSHMODE			0X00000100	/*!<ask the source should be push mode*/
#define VO_SOURCE2_FLAG_OPEN_PULLMODE			0X00000200	/*!<ask the source should be push mode*/
#define VO_SOURCE2_FLAG_OPEN_THUMBNAIL			0X00000400	/*!<Indicate this instance is for getting the thumbnail */


#define VO_SOURCE2_FLAG_SAMPLE_KEYFRAME			0X00000001	/*!<Indicate the buffer is key frame */
#define VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT		0X00000002	/*!<Indicate the buffer start new format , when the sample contain this flag, the pBuffer will be point to the structure of VO_SOURCE2_TRACK_INFO  */
#define VO_SOURCE2_FLAG_SAMPLE_FORCE_FLUSH		0X00000004	/*!<Indicate the buffer flush the previous buffers */
#define VO_SOURCE2_FLAG_SAMPLE_EOS				0X00000008	/*!<Indicate the buffer reach EOS */
#define VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM		0X00000010	/*!<Indicate the buffer is a sample for new program , when the sample contain this flag, the pBuffer will be point to the structure of VO_SOURCE2_TRACK_INFO */
#define VO_SOURCE2_FLAG_SAMPLE_FRAMEDROPPED		0x00000020	/*!<only for Video I frame, when some previous frames are dropped */
#define VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE	0x00000040	/*!<notify that start from this sample, the video may not available for a while */
#define VO_SOURCE2_FLAG_SAMPLE_SAMPLEAFTERSEEK	0x00000080	/*!<Indicate this sample is the the first sample after seek, this flag is used in the case that app seek time is not reachable and the real seek pos can not return immediatly
																When you are using this flag, the timestamp shows the reachable seek pos after seek*/
#define VO_SOURCE2_FLAG_SAMPLE_FRAMETYPEUNKNOWN	0x00000100	/*!<Indicate this frame type is unknown and it needs to check*/
#define VO_SOURCE2_FLAG_SAMPLE_FRAMESHOULDDROP	0x00000200	/*!<Indicate this frame should drop*/
#define VO_SOURCE2_FLAG_SAMPLE_BASTART			0x00000400	/*!<Indicate that this is the first frame of old stream for BA */
#define VO_SOURCE2_FLAG_SAMPLE_BAEND			0x00000800	/*!<Indicate that this ia the first frame of new stream for BA */
#define VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE	    0x00001000  /*!<Indicate the buffer is an image */
#define VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET	0x00002000	/*!<Indicate the sample timestamp rollback */
#define VO_SOURCE2_FLAG_SAMPLE_CHUNKDROPPED		0x00004000	/*!<Indicate the chunk has been dropped */
#define VO_SOURCE2_FLAG_SAMPLE_DRMHEADER		0x00008000  /*!<Indicate the chunk is drm header*/
#define VO_SOURCE2_FLAG_SAMPLE_FRAMEDECODEONLY	0x00010000	/*!<Indicate the frame shall only be decoded, but not be rendered.*/

/**
*the lib operator
*/
typedef struct
{
	void *	pUserData;																		/*!< User data  */
	void *	(* LoadLib) (void * pUserData, char * pLibName, int nFlag);						/*!< Address of load lib API */
	void *	(* GetAddress) (void * pUserData, void * hLib, char * pFuncName, int nFlag);	/*!< Address of get addr API  */
	int		(* FreeLib) (void * pUserData, void * hLib, int nFlag);							/*!< Address of free lib API  */
} VO_SOURCE2_LIB_FUNC;

typedef struct
{
	VO_U64			uTime;					/*!< start time(MS)*/
	VO_U64			uOriginTime;				/*!< Origin time(MS)*/
	VO_U32			uDuration;				/*!< duration of sample(MS)*/

	VO_U32			uFlag;					/*!< the flag that indicate the status of the sample,like key frame,codec change from this sample etc. it should use the flag define with VO_SOURCE2_FLAG_SAMPLE prefix*/
	VO_PTR			pFlagData;				/*!< If the the uFlag contains VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT, this field is pointed to VO_SOURCE2_TRACK_INFO* and pBuffer must be NULL */
	
	VO_U32			uSize;					/*!< buffer size.*/
	VO_PBYTE		pBuffer;				/*!< buffer address of sample, null indicate not need get buffer*/


	VO_VOID			*pReserve1;
	VO_VOID			*pReserve2;
}VO_SOURCE2_SAMPLE;

typedef struct
{
	VO_AUDIO_FORMAT sFormat;
	VO_CHAR			chLanguage[256];
} VO_SOURCE2_AUDIO_INFO;

typedef struct
{
	VO_VIDEO_FORMAT sFormat;
	VO_U32			uAngle;				//It should use the value defined in VO_SOURCE2_VIDEO_ANGLE
	VO_CHAR			strVideoDesc[252];
} VO_SOURCE2_VIDEO_INFO;

typedef struct  
{
	VO_CHAR			chLanguage[256];
	VO_U32			uCodingType;
	VO_CHAR			Align[8];
}VO_SOURCE2_SUBTITLE_INFO;

#define VO_SOURCE2_SELECT_SELECTABLE	0X00000000
#define VO_SOURCE2_SELECT_RECOMMEND		0X00000001
#define VO_SOURCE2_SELECT_SELECTED		0X00000002
#define VO_SOURCE2_SELECT_DISABLE		0X00000004
#define VO_SOURCE2_SELECT_DEFAULT		0X00000008
#define VO_SOURCE2_SELECT_FORCE			0X00000010

typedef struct
{
	VO_U32						uTrackID;				/*!< the track id created by our parser, it is unique in this source session */

	VO_U32						uSelInfo;				/*!< Indicated if the track is selected and recommend or default */

	VO_BYTE						strFourCC[8];
	VO_U32						uTrackType;				/*!< video/audio/subtitle */
	VO_U32						uCodec;					/*!< codec type, VO_AUDIO_CODINGTYPE | VO_VIDEO_CODINGTYPE */

	VO_U64						uDuration;				/*!< duration of this track */
	VO_U32						uChunkCounts;			/*!< chunks in the track */

	VO_U32						uBitrate;				/*!< Bitrate of this track */

	union
	{
		VO_SOURCE2_AUDIO_INFO     sAudioInfo;			/*!< audio info */
		VO_SOURCE2_VIDEO_INFO     sVideoInfo;			/*!< video info */
		VO_SOURCE2_SUBTITLE_INFO  sSubtitleInfo;		/*!< subtitle info */
	};

	VO_U32						uHeadSize;				/*!< sequence data size*/
	VO_PBYTE					pHeadData;				/*!< Header data pointor */

	VO_VOID* pVideoClosedCaptionDescData; /*!< extra data, CC Desc Data */
    VO_U16   nVideoClosedCaptionDescDataLen;  /*!< size of CC Desc Data */
	VO_U32							nMuxTrackID;					/*!< the parent stream track ID */
	VO_U32							nElementID;						/*!< the element id in muxed stream */
}VO_SOURCE2_TRACK_INFO;

typedef struct  
{
	VO_U32						uStreamID;				/*!< the stream id created by our parser, it is unique in this source session */

	VO_U32						uSelInfo;				/*!< Indicated if the Stream is selected and recommend or default */

	VO_U32						uBitrate;				/*!< the bitrate of the stream */

	VO_U32						uTrackCount;			/*!< track count if this sub stream */
	VO_SOURCE2_TRACK_INFO		**ppTrackInfo;			/*!< Track info */
}VO_SOURCE2_STREAM_INFO;

typedef struct  
{
	VO_U32						uProgramID;				/*!< the stream id created by our parser, it is unique in this source session */

	VO_U32						uSelInfo;				/*!< Indicated if the Program is selected and recommend or default */

	VO_SOURCE2_PROGRAM_TYPE		sProgramType;			/*!< Indicate if the Program is live or vod */
	VO_CHAR						strProgramName[256];	/*!< Name of the Program */

	VO_U32						uStreamCount;			/*!< Stream Count */
	VO_SOURCE2_STREAM_INFO		**ppStreamInfo;			/*!< Stream info */	
}VO_SOURCE2_PROGRAM_INFO;

typedef struct  
{
	VO_SOURCE2_DRM_TYPE			nType;					/*!< DRM Type */
	VO_PTR						*pDRM_Info;				/*!< Private DRM Info structure, this DRMInfo should mantain by parser itself */
}VO_SOURCE2_DRM_INFO;

typedef struct
{
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE	Type;					/*!< The type of this chunk */	

	VO_CHAR									szRootUrl[MAXURLLEN];	/*!< The URL of manifest. It must be filled by parser. */
	VO_CHAR									szUrl[MAXURLLEN];		/*!< URL of this chunk , maybe relative URL */

	VO_U64									ullStartTime;			/*!< The start offset time of this chunk , the unit of ( ullStartTime / ullTimeScale * 1000 ) should be ms */
	VO_U32									uPeriodSequenceNumber;	/*!< The sequence number of this chunk */

	VO_U64									ullDuration;			/*!< Duration of this chunk , the unit of ( ullDuration / ullTimeScale * 1000 ) should be ms */
	VO_U64									ullTimeScale;			/*!< TimeScale of this chunk */

	VO_U32									uReserved1;				/*!< It is an Http ErrorCode for Warning & Error Event*/
	VO_U32									uReserved2;				/*!< It is the Chunk flag, Please refer to VO_SOURCE2_ADAPTIVESTREAMING_CHUNKFLAG */

}VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO;

typedef struct  
{
	VO_CHAR		szRootUrl[MAXURLLEN];							/*!< The URL of parent playlist */
	VO_CHAR		szUrl[MAXURLLEN];								/*!< The URL of the playlist , maybe relative URL */
	VO_CHAR		szNewUrl[MAXURLLEN];							/*!< The URL after download( maybe redirect forever ), you should always use this url after get this struct */

	VO_PBYTE	pData;											/*!< The data in the playlist */
	VO_U32		uDataSize;										/*!< Playlist size */

	VO_U32		uReserved1;										/*!< It is an Http ErrorCode for Warning & Error Event*/
	VO_U32		uReserved2;


}VO_SOURCE2_ADAPTIVESTREAMING_PLAYLISTDATA;

typedef struct 
{
	VO_U64			ullStartTime;								/*!< if it equals UNAVALIABLETIME, it means it is not avaliable */
	VO_U64			ullEndTime;									/*!< if it equals UNAVALIABLETIME, it means it is not avaliable */
	VO_U64			ullPlayingTime;
	VO_U64			ullLiveTime;								/*!< if it equals UNAVALIABLETIME, it means it is not avaliable */

	VO_VOID			*pReserve1;
	VO_VOID			*pReserve2;
}VO_SOURCE2_SEEKRANGE_INFO;

typedef struct  
{
	VO_U32			nChunkFlag;
	VO_U64			ullChunkStartTime;
	VO_U32			uPeriodSequenceNumber;
	VO_U64			ullPeriodFirstChunkStartTime;
	VO_U64 *		pullRetTimeStamp;

	VO_VOID			*pReserve1;
	VO_VOID			*pReserve2;
}VO_SOURCE2_CHUNK_SAMPLE;

typedef struct
{
//callback instance
	VO_PTR pUserData;
/**
 * The source will notify client via this function for some events.
 * \param pUserData [in] The user data which was set by Open().
 * \param nID [in] The status type.
 * \param nParam1 [in] status specific parameter 1.
 * \param nParam2 [in] status specific parameter 2.
 */
	VO_S32 (VO_API * SendEvent) (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

}VO_SOURCE2_EVENTCALLBACK;

typedef struct
{
//callback instance
	VO_PTR pUserData;

/**
 * Callback function. The source will send the data out..
 * \param pUserData [in] The user data which was set by Open().
 * \param nOutputType [in] The output data type, see VO_SOURCE2_OUTPUT_TYPE
 * \param pData [in] The data pointer, it MUST be converted to correct struct pointer according nType, see VO_SOURCE2_OUTPUT_TYPE
 *                   the param type is depended on the nOutputType, for VO_SOURCE2_TT_VIDEO & VO_SOURCE2_TT_AUDIO please use VO_SOURCE2_SAMPLE
 *                                                                  for VO_SOURCE2_TT_SUBTITLE please use voSubtitleInfo
 */
	VO_S32 (VO_API * SendData) (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);

}VO_SOURCE2_SAMPLECALLBACK;

typedef struct  
{
	VO_PTR pUserData;
	VO_U32 (VO_API * RegSocket)( VO_PTR pUserData , VO_S32 nSocket , VO_SOURCE2_SOCKET_USAGE nUsage );
	VO_U32 (VO_API * UnRegSocket)( VO_PTR pUserData , VO_S32 nSocket );
}VO_SOURCE2_SOCKETREG;

typedef struct  
{
	VO_PTR pUserData;
	VO_BOOL (VO_API * CheckCanUseProxy)( VO_PTR pUserData , VO_CHAR * pUrl );
}VO_SOURCE2_PROXYCHECK;

typedef struct 
{
	VO_SOURCE2_IO_API * pIO;
	VO_U64				ullActualFileSize;
}VO_SOURCE2_COMMONINITPARAM;


typedef struct  
{
	VO_U32 uFlag;
	VO_PTR pInitParam;
	VO_TCHAR *   strWorkPath;     /*!< Load library work path */
}VO_SOURCE2_INITPARAM;

/**
 * CAP data
 */
typedef struct
{
	VO_U32		nCodecType;			/*!<The codec you want to query performance data */
	VO_U32		nBitRate;			/*!<The bitrate */
	VO_U32		nVideoWidth;		/*!<The width of video */
	VO_U32		nVideoHeight;		/*!<The height of video */
	VO_U32		nProfileLevel;		/*!<The profile's level, 0:baseline,1:main profile,2:high profile*/
	VO_U32		nFPS;				/*!<The frame rate*/
}VO_SOURCE2_CAP_DATA;
	
	
/**
 * CPU info
 */
typedef struct
{
	VO_U32			nCoreCount;		/*!<The codec you want to query performance data */
	VO_U32			nCPUType;		/*!<The CPU type,0:neon not supoort, 1:support neon */
	VO_U32			nFrequency;		/*!<The frequency of CPU */
	long long		llReserved;		/*!<The reserved value */
}VO_SOURCE2_CPU_INFO;

typedef struct 
{
	VO_PTR pUserData;				/*!<The userdata recved the VO_SOURCE2_IO_HTTP_VERIFYCALLBACK*/
	VO_PTR pData;					/*!<The data that need to do verification*/
	VO_U32 uDataSize;				/*!<The data size*/
	VO_U32 uDataFlag;				/*!<The data flag, if it is 0, it is the request string, other value to not support currently */
	VO_CHAR * szResponse;			/*!<The response string*/
	VO_U32 uResponseSize;			/*!<The response string size*/
}VO_SOURCE2_VERIFICATIONINFO;

typedef struct 
{
	VO_U32 uBufferTime;				/*!<The internal buffer time of RTSP, count by milli seconds*/
	VO_U32 uRTPTimeOut;				/*!<S> default 10s, count by second*/
	VO_S32 uRTPChan;				/*!<The default channel of RTP, 0 for RTP/UDP, 1 for RTP/TCP*/
	VO_U32 uVideoConnectionPort;	/*<Indicate video session port, (1024,65535) with Even number*/
	VO_U32 uAudioConnectionPort;	/*<Indicate audio session port, (1024,65535) with Even number*/
}VO_SOURCE2_RTSP_INIT_PARAM;

typedef struct  
{
	VO_U64		ullTimeStamp;				/*!<Time stamp*/

	VO_U32		uSize;						/*!<Size */
	VO_VOID *	pData;						/*!<Data pointor */

	VO_U32		uFlag;						/*!<The pData flag, please refer to VO_SOURCE2_CUSTOMERTAG_TIMEDTAG_FLAG */
	VO_VOID *	pReserved;					/*!<Reserved */
}VO_SOURCE2_CUSTOMERTAG_TIMEDTAG;

typedef struct 
{
	VO_SOURCE2_TRACK_TYPE   nTrackType;						/*!< [in] track type */
	VO_U32					uPacketRecved;					/*!< [out] the cumulative number of RTP media packets received in this media track during this session */
	VO_U32					uPacketDuplicated;				/*!< [out] cumulative number of RTP media packets previously received in this media track during this session */
	VO_U32					uPacketLost;					/*!< [out] the cumulative number of RTP media packets lost for this media track type */
	VO_U32					uPacketSent;					/*!< [out] the cumulative number of RTP media packets sent in this media track during this session */
	VO_U32					uAverageJitter;					/*!< [out] the mean relative transit time between each two RTP packets of this media track type throughout the duration of the media session */
	VO_U32					uAverageLatency;				/*!< [out] average accumulating latency values of all RTP media packets transmitted from the server to the client, !!now only reserved!! */
}VO_SOURCE2_RTSP_STATS;

typedef struct  
{
	VO_PCHAR 	szProxyHost;	//proxy server host name or ip address, must not be null	
	VO_PCHAR 	szProxyPort;	//proxy server port number,must not be null

	VO_U32  	uFlag;			//indicates some property of this structure
	VO_PTR		pFlagData;		//with some special Flag, this field may use

	VO_PCHAR	szProxyBlackList;

}VO_SOURCE2_HTTPPROXY;

typedef struct  
{
	VO_PCHAR 	szHeaderName;	//name of the header, must not be null	
	VO_PCHAR 	szHeaderValue;	//value of the header,must not be null

	VO_U32  	uFlag;			//indicates some property of this structure
	VO_PTR		pFlagData;		//with some special Flag, this field may use
}VO_SOURCE2_HTTPHEADER;

typedef struct  
{
	VO_SOURCE2_THUMBNAILMODE		uFlag;			/*!< [in] Should use the value defined in VO_SOURCE2_THUMBNAILMODE */

	VO_PBYTE						pSampleData;	/*!< [out] Sample data */
	VO_U32							uSampleSize;	/*!< [out] Sample size */

	VO_S64							ullTimeStamp;	/*!< [out] Sample time stamp */

	VO_U64							ullFilePos;		/*!< [out] Sample file pos in the media file */
	VO_U32							uPrivateData;	/*!< [out] Private Data for source use internally only */
}VO_SOURCE2_THUMBNAILINFO;

typedef struct  
{
	VO_U32 uPeriodSequenceNumber;
	VO_U64 ullTimeStamp;
}VO_SOURCE2_PERIODTIMEINFO;

/**
 * Threshold of bitrate adaptation.
 */
typedef struct
{
	VO_U32 nUpper;
	VO_U32 nLower;
}VO_SOURCE2_BA_THRESHOLD;

typedef struct
{
	int     nCount;
    char**   pLanguage;
}VO_SOURCE2_PREFERRED_LANGUAGE;


typedef struct
{
/**
 * Initial a source session and Set the Source( url etc. ) into the module
 * For this function please only copy the pSource param, and cache the callback, do not do anything else
 * This function should be called first for a session.
 * \param phHandle [out] source handle.
 * \param pSource [in] source file description, should be an url or fd etc.
 * \param nFlag [in]. It can indicate the source type..
 * \param pCallback [in] notify call back function pointer.
 */
VO_U32 (VO_API * Init) (VO_HANDLE * phHandle, VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam );

/**
 * Uninitial a source session
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * Uninit) (VO_HANDLE hHandle);

/**
 * Check and analyze the stream, get information
 * For this function, please do not start to parser the media data
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * Open) ( VO_HANDLE hHandle );

/**
 * Destroy the information of the stream
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * Close) ( VO_HANDLE hHandle );

/**
 * Start to parse the media data
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * Start) (VO_HANDLE hHandle);

/**
 * Pause
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * Pause) (VO_HANDLE hHandle);

/**
 * Stop
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * Stop) (VO_HANDLE hHandle);

/**
 * Seek
 * \param hHandle [in] The source handle. Opened by Init().
 * \param pTimeStamp [in/out] The seek pos and return the available pos after seek
 */
VO_U32 (VO_API * Seek) (VO_HANDLE hHandle, VO_U64* pTimeStamp);

/**
 * Get Duration of the stream
 * \param hHandle [in] The source handle. Opened by Init().
 * \param pDuration [out] The duration of the stream
 */
VO_U32 (VO_API * GetDuration) (VO_PTR hHandle, VO_U64 * pDuration);

/**
 * Get sample from the source
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nOutPutType [in] The sample type of the stream, it will be audio/video/closed caption
 * \param pSample [out] The sample of the stream, all buffer in the sample( include reserve field ) will be mantained by source, they will be available until you call GetSample again
 *                       the param type is depended on the nOutputType, for VO_SOURCE2_TT_VIDEO & VO_SOURCE2_TT_AUDIO please use VO_SOURCE2_SAMPLE
 *                                                                      for VO_SOURCE2_TT_SUBTITLE please use voSubtitleInfo
 */
VO_U32 (VO_API * GetSample) ( VO_HANDLE hHandle , VO_SOURCE2_TRACK_TYPE nTrackType , VO_PTR pSample );

/**
 * It retrieve how many streams in the live stream
 * For Stream, it should be defined as: all sub streams that share the same angle for video and same language for audio
 * \param hHandle [in] The source handle. Opened by Init().
 * \param pProgramCount [out] The number of the streams
 */
VO_U32 (VO_API * GetProgramCount) (VO_HANDLE hHandle, VO_U32 *pProgramCount);

/**
 * Enum all the stream info based on stream count
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nProgram  [in] The Stream sequence based on the stream counts
 * \param pProgramInfo [out] return the info of the Stream, source will keep the info structure available until you call close
 */
VO_U32 (VO_API * GetProgramInfo) (VO_HANDLE hHandle, VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **ppProgramInfo);

/**
 * Get the track info we selected
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nTrackType [in] The sample type of the stream, it will be audio/video/closed caption
 * \param pTrackInfo [out] The trackinfo of the selected track. The trackinfo memory will be maintained in source until you call close
 */
VO_U32 (VO_API * GetCurTrackInfo) ( VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo );

/**
 * Select the Program
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nProgram [in] Program ID
 */
VO_U32 (VO_API * SelectProgram) (VO_HANDLE hHandle, VO_U32 nProgram);

/**
 * Select the Stream
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nStream [in] Stream ID
 */
VO_U32 (VO_API * SelectStream) (VO_HANDLE hHandle, VO_U32 nStream);

/**
 * Select the Track
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nTrack [in] Track ID
 */
VO_U32 (VO_API * SelectTrack) (VO_HANDLE hHandle, VO_U32 nTrack);

/**
 * Get the DRM info
 * \param hHandle [in] The source handle. Opened by Init().
 * \param VO_SOURCE2_DRM_INFO [out] return the drm info, source will keep the info structure available until you call close
 */
VO_U32 (VO_API * GetDRMInfo) (VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO **ppDRMInfo);

/**
* Send the buffer into the source, if you want to use this function, you should use flag VO_SOURCE2_FLAG_OPEN_SENDBUFFER
* \param hHandle [in] The source handle. Opened by Init().
* \param buffer [in] The buffer send to the source
*/
VO_U32 (VO_API * SendBuffer) (VO_HANDLE hHandle, const VO_SOURCE2_SAMPLE& buffer );

/**
 * Get the special value from param ID
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nParamID [in] The param ID
 * \
 */
VO_U32 (VO_API * GetParam) (VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam);

/**
 * Set the special value from param ID
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nParamID [in] The param ID
 * \param pParam [in] The set value depend on the param ID.
 */
VO_U32 (VO_API * SetParam) (VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam);

} VO_SOURCE2_API;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
