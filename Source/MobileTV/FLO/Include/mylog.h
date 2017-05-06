#ifndef _FLO_LOG_H_
#define _FLO_LOG_H_

#include "dbgdump.h"


#define TXTLN "\r\n"
#ifdef WIN32
#define DEFAULT_LOG_DIR "C:\\Log"
#define DEFAULT_LOG_MASK 0xffffffff
#else //WIN32
#define DEFAULT_LOG_DIR "/sdcard/log"
#define DEFAULT_LOG_MASK 0
#endif //WIN32

#define LOG_DX_SUMMARY 0x1
#define LOG_DX_DATA    0x2

#define LOG_EVENT      0x08

#define LOG_RTP_VIDEO  0x10
#define LOG_RTP_AUDIO  0x20
#define LOG_RTP_LOST   0x40

#define LOG_RTSP_CMD	0x80
#define LOG_RTCP_REPORT	0x100

#if ENABLE_LOG

#define LOGFILE_RTP_LOST "rtp_lost.csv"
#define LOGTITLE_RTP_LOST "SysTime,RTPPayloadType,RTPSeqNum,NextExpSeqNum,LostNum" TXTLN

#define LOGFILE_RTP_AUDIO "rtp_audio.csv"
#define LOGFILE_RTP_VIDEO "rtp_video.csv"
#define LOGTITLE_RTP_AUDIO "SysTime,PayloadType,SSRC,TimeStamp,SeqNum,PacketSize" TXTLN
#define LOGTITLE_RTP_VIDEO "SysTime,PayloadType,SSRC,TimeStamp,SeqNum,PacketSize" TXTLN
//#define LOGTITLE_RTP_VIDEO LOGTITLE_RTP_AUDIO 

#define LOGFILE_RTSP	"rtsp.txt"

#define LOGFILE_EVENT "event.txt"
#define LOGTITLE_EVENT "SysTime    Code   Param" TXTLN
#define LOGFMT_EVENT   "%7d %7d %7d" TXTLN

extern class CTextDumper TheLostRTPLogger;
extern class CTextDumper TheAudioRTPLogger;
extern class CTextDumper TheVideoRTPLogger;
extern class CTextDumper TheRTSPLogger;
extern class CTextDumper TheEventLogger;

#define MYLOG_DEF \
class CTextDumper TheLostRTPLogger; \
class CTextDumper TheAudioRTPLogger; \
class CTextDumper TheVideoRTPLogger; \
class CTextDumper TheRTSPLogger; \
class CTextDumper TheEventLogger; 

inline void MYLOG_INIT()
{
	TheLostRTPLogger.Open(LOGFILE_RTP_LOST, LOG_RTP_LOST);
	TheLostRTPLogger.Dump(LOGTITLE_RTP_LOST);

    TheVideoRTPLogger.Open(LOGFILE_RTP_VIDEO, LOG_RTP_VIDEO);
    TheVideoRTPLogger.Dump(LOGTITLE_RTP_VIDEO);

    TheAudioRTPLogger.Open(LOGFILE_RTP_AUDIO, LOG_RTP_AUDIO);
    TheAudioRTPLogger.Dump(LOGTITLE_RTP_AUDIO);

	TheRTSPLogger.Open(LOGFILE_RTSP, LOG_RTSP_CMD);

    TheEventLogger.Open(LOGFILE_EVENT, LOG_EVENT);
    TheEventLogger.Dump(LOGTITLE_EVENT);
}


inline void MYLOG_DONE()
{
	TheLostRTPLogger.Close();
    TheVideoRTPLogger.Close();
    TheAudioRTPLogger.Close();
	TheRTSPLogger.Close();
    TheEventLogger.Close();
}

#else //ENABLE_LOG

#define MYLOG_DEF
#define MYLOG_INIT()

#endif //ENABLE_LOG


#endif //_FLO_LOG_H_
