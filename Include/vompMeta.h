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

#ifndef __vompMeta_H__
#define __vompMeta_H__

#include "vompType.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
* Parameter ID
* S - set; G - get; S/G - both set and get.
*/
#define	VOMP_PID_METADATA_BASE				0x600000								/*!< the base param ID for MetaData module */
#define	VOMP_PID_METADATA_TITLE				(VOMP_PID_METADATA_BASE | 0x0001)		/*!< <G>Title, PMetaDataString */
#define	VOMP_PID_METADATA_ARTIST			(VOMP_PID_METADATA_BASE | 0x0002)		/*!< <G>Artist, PMetaDataString */
#define	VOMP_PID_METADATA_ALBUM				(VOMP_PID_METADATA_BASE | 0x0003)		/*!< <G>Album, PMetaDataString */
#define	VOMP_PID_METADATA_GENRE				(VOMP_PID_METADATA_BASE | 0x0004)		/*!< <G>Genre, PMetaDataString */
#define	VOMP_PID_METADATA_LYRICLINE			(VOMP_PID_METADATA_BASE | 0x0005)		/*!< <G>Lyric lines, VO_U32* */
#define	VOMP_PID_METADATA_LYRICBYLINE		(VOMP_PID_METADATA_BASE | 0x0006)		/*!< <G>Lyric by line, PMetaDataLyricInfo, dwTime<Input> - Line */
#define	VOMP_PID_METADATA_LYRICBYTIME		(VOMP_PID_METADATA_BASE | 0x0007)		/*!< <G>Lyric by time, PMetaDataLyricInfo, dwTime<Input> - Time */
#define	VOMP_PID_METADATA_COMPOSER			(VOMP_PID_METADATA_BASE | 0x0008)		/*!< <G>Composer, PMetaDataString */
#define	VOMP_PID_METADATA_TRACK				(VOMP_PID_METADATA_BASE | 0x0009)		/*!< <G>Track, PMetaDataString */
#define	VOMP_PID_METADATA_FRONTCOVER		(VOMP_PID_METADATA_BASE | 0x000A)		/*!< <G>Front Cover, PMetaDataImage */
#define	VOMP_PID_METADATA_GRACENOTETAGID	(VOMP_PID_METADATA_BASE | 0x000B)		/*!< <G>Grace note tag ID, PMetaDataString */
#define	VOMP_PID_METADATA_COMMENT			(VOMP_PID_METADATA_BASE | 0x000C)		/*!< <G>Comments, PMetaDataString */
#define	VOMP_PID_METADATA_DISK				(VOMP_PID_METADATA_BASE | 0x000D)		/*!< <G>Disk, PMetaDataString */
#define	VOMP_PID_METADATA_PUBLISHER			(VOMP_PID_METADATA_BASE | 0x000E)		/*!< <G>Publisher, PMetaDataString */
#define	VOMP_PID_METADATA_ISRC				(VOMP_PID_METADATA_BASE | 0x000F)		/*!< <G>ISRC, PMetaDataString */
#define	VOMP_PID_METADATA_YEAR				(VOMP_PID_METADATA_BASE | 0x0010)		/*!< <G>Year, PMetaDataString */
#define	VOMP_PID_METADATA_BAND				(VOMP_PID_METADATA_BASE | 0x0011)		/*!< <G>Band, PMetaDataString */
#define	VOMP_PID_METADATA_CONDUCTOR			(VOMP_PID_METADATA_BASE | 0x0012)		/*!< <G>Conductor, PMetaDataString */
#define	VOMP_PID_METADATA_REMIXER			(VOMP_PID_METADATA_BASE | 0x0013)		/*!< <G>Remixer, PMetaDataString */
#define	VOMP_PID_METADATA_LYRICIST			(VOMP_PID_METADATA_BASE | 0x0014)		/*!< <G>Lyricist, PMetaDataString */
#define	VOMP_PID_METADATA_RADIOSTATION		(VOMP_PID_METADATA_BASE | 0x0015)		/*!< <G>Radio Station Name, PMetaDataString */
#define	VOMP_PID_METADATA_ORIGRELEASEYEAR	(VOMP_PID_METADATA_BASE | 0x0016)		/*!< <G>Original Release Year, PMetaDataString */
#define	VOMP_PID_METADATA_OWNER				(VOMP_PID_METADATA_BASE | 0x0017)		/*!< <G>Owner/Licensee, PMetaDataString */
#define	VOMP_PID_METADATA_ENCODER			(VOMP_PID_METADATA_BASE | 0x0018)		/*!< <G>Encoder, PMetaDataString */
#define	VOMP_PID_METADATA_ENCODESETTINGS	(VOMP_PID_METADATA_BASE | 0x0019)		/*!< <G>Encodeing PMetaDataString */
#define	VOMP_PID_METADATA_COPYRIGHT			(VOMP_PID_METADATA_BASE | 0x001A)		/*!< <G>Copyright, PMetaDataString */
#define	VOMP_PID_METADATA_ORIGARTIST		(VOMP_PID_METADATA_BASE | 0x001B)		/*!< <G>Original Artist, PMetaDataString */
#define	VOMP_PID_METADATA_ORIGALBUM			(VOMP_PID_METADATA_BASE | 0x001C)		/*!< <G>Original Album, PMetaDataString */


#define	VOMP_PID_METADATA_DURATION			(VOMP_PID_METADATA_BASE | 0x1001)		/*!< <G>Duration of the file */
#define	VOMP_PID_METADATA_CODEC				(VOMP_PID_METADATA_BASE | 0x1002)		/*!< <G>Codec of the file */
#define	VOMP_PID_METADATA_SAMPLERATE		(VOMP_PID_METADATA_BASE | 0x1003)		/*!< <G>Sample Rate of the file */
#define	VOMP_PID_METADATA_BITRATE			(VOMP_PID_METADATA_BASE | 0x1004)		/*!< <G>Bit Rate of the file */
#define	VOMP_PID_METADATA_FRAMERATE			(VOMP_PID_METADATA_BASE | 0x1005)		/*!< <G>Frame Rate of the file */
#define	VOMP_PID_METADATA_FILEFORMAT		(VOMP_PID_METADATA_BASE | 0x1006)		/*!< <G>File Format of the file */
#define	VOMP_PID_METADATA_VIDEOWIDTH		(VOMP_PID_METADATA_BASE | 0x1007)		/*!< <G>Video Width of the file */
#define	VOMP_PID_METADATA_VIDEOHEIGHT		(VOMP_PID_METADATA_BASE | 0x1008)		/*!< <G>Video height of the file */
#define	VOMP_PID_METADATA_DRMCRIPPLED		(VOMP_PID_METADATA_BASE | 0x1009)		/*!< <G>Video height of the file */
#define	VOMP_PID_METADATA_MIME				(VOMP_PID_METADATA_BASE | 0x100A)		/*!< <G>Video height of the file */

#define	VOMP_PID_METADATA_MAX				(VOMP_PID_METADATA_BASE | 0x1FFF)		/*!< <G>The max value of meta data */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __vompMeta_H__
