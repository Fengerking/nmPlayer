#ifndef __RTSPTRANS__H_
#define __RTSPTRANS__H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//void 	SetRTSPPacket(unsigned char flag, unsigned int size, unsigned char *buf);
void 	SetRTSPPacket(unsigned char isTCP, unsigned char flag, unsigned int size, unsigned char *buf, unsigned short rtpSeqNum, unsigned int rtpTimestamp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__RTSPTRANS__H_
