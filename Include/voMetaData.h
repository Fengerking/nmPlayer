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

#ifndef __voMetaData_H__
#define __voMetaData_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voSource.h"

/**
* Parameter ID
* S - set; G - get; S/G - both set and get.
*/
#define	VO_PID_METADATA_BASE			(VO_PID_SOURCE_BASE | 0x600000)		/*!< the base param ID for MetaData module */
#define	VO_PID_METADATA_TITLE			(VO_PID_METADATA_BASE | 0x0001)		/*!< <G>Title, PMetaDataString */
#define	VO_PID_METADATA_ARTIST			(VO_PID_METADATA_BASE | 0x0002)		/*!< <G>Artist, PMetaDataString */
#define	VO_PID_METADATA_ALBUM			(VO_PID_METADATA_BASE | 0x0003)		/*!< <G>Album, PMetaDataString */
#define	VO_PID_METADATA_GENRE			(VO_PID_METADATA_BASE | 0x0004)		/*!< <G>Genre, PMetaDataString */
#define	VO_PID_METADATA_LYRICLINE		(VO_PID_METADATA_BASE | 0x0005)		/*!< <G>Lyric lines, VO_U32* */
#define	VO_PID_METADATA_LYRICBYLINE		(VO_PID_METADATA_BASE | 0x0006)		/*!< <G>Lyric by line, PMetaDataLyricInfo, dwTime<Input> - Line */
#define	VO_PID_METADATA_LYRICBYTIME		(VO_PID_METADATA_BASE | 0x0007)		/*!< <G>Lyric by time, PMetaDataLyricInfo, dwTime<Input> - Time */
#define	VO_PID_METADATA_COMPOSER		(VO_PID_METADATA_BASE | 0x0008)		/*!< <G>Composer, PMetaDataString */
#define	VO_PID_METADATA_TRACK			(VO_PID_METADATA_BASE | 0x0009)		/*!< <G>Track, PMetaDataString */
#define	VO_PID_METADATA_FRONTCOVER		(VO_PID_METADATA_BASE | 0x000A)		/*!< <G>Front Cover, PMetaDataImage */
#define	VO_PID_METADATA_GRACENOTETAGID	(VO_PID_METADATA_BASE | 0x000B)		/*!< <G>Grace note tag ID, PMetaDataString */
#define	VO_PID_METADATA_COMMENT			(VO_PID_METADATA_BASE | 0x000C)		/*!< <G>Comments, PMetaDataString */
#define	VO_PID_METADATA_DISK			(VO_PID_METADATA_BASE | 0x000D)		/*!< <G>Disk, PMetaDataString */
#define	VO_PID_METADATA_PUBLISHER		(VO_PID_METADATA_BASE | 0x000E)		/*!< <G>Publisher, PMetaDataString */
#define	VO_PID_METADATA_ISRC			(VO_PID_METADATA_BASE | 0x000F)		/*!< <G>ISRC, PMetaDataString */
#define	VO_PID_METADATA_YEAR			(VO_PID_METADATA_BASE | 0x0010)		/*!< <G>Year, PMetaDataString */
#define	VO_PID_METADATA_BAND			(VO_PID_METADATA_BASE | 0x0011)		/*!< <G>Band(Album Artist), PMetaDataString */
#define	VO_PID_METADATA_CONDUCTOR		(VO_PID_METADATA_BASE | 0x0012)		/*!< <G>Conductor, PMetaDataString */
#define	VO_PID_METADATA_REMIXER			(VO_PID_METADATA_BASE | 0x0013)		/*!< <G>Remixer, PMetaDataString */
#define	VO_PID_METADATA_LYRICIST		(VO_PID_METADATA_BASE | 0x0014)		/*!< <G>Lyricist, PMetaDataString */
#define	VO_PID_METADATA_RADIOSTATION	(VO_PID_METADATA_BASE | 0x0015)		/*!< <G>Radio Station Name, PMetaDataString */
#define	VO_PID_METADATA_ORIGRELEASEYEAR	(VO_PID_METADATA_BASE | 0x0016)		/*!< <G>Original Release Year, PMetaDataString */
#define	VO_PID_METADATA_OWNER			(VO_PID_METADATA_BASE | 0x0017)		/*!< <G>Owner/Licensee, PMetaDataString */
#define	VO_PID_METADATA_ENCODER			(VO_PID_METADATA_BASE | 0x0018)		/*!< <G>Encoder, PMetaDataString */
#define	VO_PID_METADATA_ENCODESETTINGS	(VO_PID_METADATA_BASE | 0x0019)		/*!< <G>Encodeing PMetaDataString */
#define	VO_PID_METADATA_COPYRIGHT		(VO_PID_METADATA_BASE | 0x001A)		/*!< <G>Copyright, PMetaDataString */
#define	VO_PID_METADATA_ORIGARTIST		(VO_PID_METADATA_BASE | 0x001B)		/*!< <G>Original Artist, PMetaDataString */
#define	VO_PID_METADATA_ORIGALBUM		(VO_PID_METADATA_BASE | 0x001C)		/*!< <G>Original Album, PMetaDataString */
#define VO_PID_METADATA_COMPILATION		(VO_PID_METADATA_BASE | 0x001D)		/*!< <G>Compilation, PMetaDataString */

/**
* Error code
*/
#define VO_ERR_METADATA_OK				VO_ERR_NONE
#define VO_ERR_METADATA_BASE			(VO_ERR_SOURCE_BASE | 0x600000)
#define VO_ERR_METADATA_OPENFAIL		(VO_ERR_METADATA_BASE | 0x0001)		/*!< open fail */
#define VO_ERR_METADATA_NOMETADATA		(VO_ERR_METADATA_BASE | 0x0002)		/*!< file has no meta data */
#define VO_ERR_METADATA_NOSUCHFRAME		(VO_ERR_METADATA_BASE | 0x0003)		/*!< file has no such frame */
#define VO_ERR_METADATA_ERRORDATA		(VO_ERR_METADATA_BASE | 0x0004)		/*!< this frame has error data */

/**
 * text encoding
 */
#define VO_METADATA_TE_UNKNOWN				0x0
#define VO_METADATA_TE_ANSI					0x1
#define VO_METADATA_TE_UTF8					0x2
#define VO_METADATA_TE_UTF16				0x4
#define VO_METADATA_NOTADDSTRINGTERMINATE	0x80000000
#define VO_METADATA_DYNAMICSTRING			0x40000000
typedef struct tagMetaDataString
{
	//0xABRRRRRR RRRRRRRR RRRRRRRR TTTTTTTT
	//A<User Set>
	//if set, user tell voMetaData should not add string terminate automatically

	//B<voMetaData Set>
	//if set, voMetaData tell user the string is dynamic allocated
	//you must copy it, otherwise the buffer maybe be released after you call next GetParam

	//T<voMetaData Set>
	//voMetaData tell user the string type, VO_METADATA_TE_XXX
	VO_U32						dwFlags;
	VO_U32						dwBufferSize;
	VO_PBYTE					pBuffer;
} MetaDataString, *PMetaDataString;

/**
* image type
*/
typedef enum
{
	VO_METADATA_IT_UNKNOWN	= 0, 
	VO_METADATA_IT_JPEG, 
	VO_METADATA_IT_BMP,
	VO_METADATA_IT_PNG, 
	VO_METADATA_IT_GIF, 
	VO_METADATA_IT_MAX	= VO_MAX_ENUM_VALUE
} VO_METADATA_IMAGETYPE;

typedef struct tagMetaDataImage 
{
	VO_METADATA_IMAGETYPE		nImageType;
	VO_U32						dwBufferSize;
	VO_PBYTE					pBuffer;
} MetaDataImage, *PMetaDataImage;

/**
* Lyric Structure
*/
typedef struct tagMetaDataLyricInfo 
{
	VO_U32			dwTime;			/*!< the start time of this line */
	MetaDataString	strLyric;		/*!< lyric text  (UNICODE)*/
} MetaDataLyricInfo, *PMetaDataLyricInfo;

/**
 * Source reader function set
 */
typedef struct
{
	VO_U32 (VO_API * Open)(VO_PTR * ppHandle, VO_SOURCE_OPENPARAM * pParam);
	VO_U32 (VO_API * Close)(VO_PTR pHandle);
	VO_U32 (VO_API * SetParam)(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
	VO_U32 (VO_API * GetParam)(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
} VO_METADATA_READAPI;

/**
* Get MetaData Read API interface
* \param pReadHandle [out] Return the MetaData File Read handle.
* \retval VO_ERR_OK Succeeded.
*/
VO_S32 VO_API voGetMetaDataReadAPI(VO_METADATA_READAPI* pReadHandle, VO_U32 uFlag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voMetaData_H__
