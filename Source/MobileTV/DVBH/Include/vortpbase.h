#ifndef __VORTPBASE_H__
#define __VORTPBASE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voIndex.h"

typedef enum
{
	PACKET_DATA = 0x00000000,  /*!< Unknown, or multiplex */
	PACKET_VIDEO = 0x01000000, /*!< Video data */
	PACKET_AUDIO = 0x02000000, /*!< Audio data */
	PACKET_TEXT = 0x04000000,  /*!< Text data */
	PACKET_MEDIA = 0x80000000, /*!< Video or audio data */
	PACKET_CTRL = 0x40000000,  /*!< Control information */
	PACKET_SDP = 0x08000000, /*!< Status information */
} VOPACKETTYPE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__VORTPBASE_H__