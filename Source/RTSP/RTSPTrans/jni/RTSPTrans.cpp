/*VisualOn Thomas Liang 10/23/2012*/
#include <android/log.h> 
#include "RTSPTrans.h"
#include <arpa/inet.h>

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define LOG_TAG "MYJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)  

void DumpRTSPPacket(unsigned char* msg)
{ 
	char* pDirPath = NULL;
  char szLogPath[128] = { 0 };

  pDirPath = "/sdcard/rtspTransLog";
  snprintf(szLogPath, 127, "%s/RTSPPacket.data", pDirPath);
  if(access(pDirPath,0)!=0)
  	if(mkdir(pDirPath, 0755)!=0) return;
   
  FILE* pLoger=fopen(szLogPath, "a");
  if(pLoger==NULL) return;
  
  fprintf(pLoger,"%x",msg);
  fprintf(pLoger, "\n");
  fclose(pLoger);
}
/*
void SetRTSPPacket(unsigned char flag, unsigned int size, unsigned char *buf)
{
	LOGI("SetRTSPPacket: flag = %d, size = %d, buf =%x", flag, size, buf);
	DumpRTSPPacket(buf);
}
*/

void 	SetRTSPPacket(unsigned char isTCP, unsigned char flag, unsigned int size, unsigned char *buf, unsigned short rtpSeqNum, unsigned int rtpTimestamp)
{
	LOGI("SetRTSPPacket: isTCP = %d, flag = %d, size = %d, buf =%x, rtpSeqNum = %d, rtpTimestamp = %d", isTCP, flag, size, buf, rtpSeqNum, rtpTimestamp);

	unsigned int rtpHeader = ntohl(*(unsigned int *)(buf));
	LOGI("SetRTSPPacket: rtpHeader = %x", rtpHeader);
	
	bool m_rtpMarker = ((rtpHeader & 0x00800000) >> 23) != 0;       // marker: 1 bit
	unsigned int m_rtpPayloadType = (rtpHeader & 0x007F0000) >> 16;  // payload: 7 bits
	unsigned short m_rtpSeqNum = (unsigned short)(rtpHeader & 0xFFFF); // sequence number: 16 bits

	LOGI("SetRTSPPacket: m_rtpMarker = %d", m_rtpMarker);
	LOGI("SetRTSPPacket: m_rtpPayloadType = %d", m_rtpPayloadType);
	LOGI("SetRTSPPacket: m_rtpSeqNum = %d", m_rtpSeqNum);


	DumpRTSPPacket(buf);
}