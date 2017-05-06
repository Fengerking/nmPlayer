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

#ifndef __VOSOURCE2_METADATA_H__

#define __VOSOURCE2_METADATA_H__

#define VO_PID_SOURCE2_METADATA_BASE					0x57000000	/*!< <s> Meta data param id offset */

#define VO_PID_SOURCE2_METADATA_TITLE					( VO_PID_SOURCE2_METADATA_BASE | 0x0001 )	/*!< <g> Param to get title info, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_ARTIST					( VO_PID_SOURCE2_METADATA_BASE | 0x0002 )	/*!< <g> Param to get artist info, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_ALBUM					( VO_PID_SOURCE2_METADATA_BASE | 0x0003 )	/*!< <g> Param to get album name, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_GENRE					( VO_PID_SOURCE2_METADATA_BASE | 0x0004 )	/*!< <g> Param to get genre, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_COMPOSER				( VO_PID_SOURCE2_METADATA_BASE | 0x0005 )	/*!< <g> Param to get composer, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_TRACK					( VO_PID_SOURCE2_METADATA_BASE | 0x0006 )	/*!< <g> Param to get track, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_FRONTCOVER				( VO_PID_SOURCE2_METADATA_BASE | 0x0007 )	/*!< <g> Param to get front cover, VO_SOURCE2_METADATA_IMAGE* */
#define VO_PID_SOURCE2_METADATA_GRACENOTETAGID			( VO_PID_SOURCE2_METADATA_BASE | 0x0008 )	/*!< <g> Param to get gracenote id, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_COMMENT					( VO_PID_SOURCE2_METADATA_BASE | 0x0009 )	/*!< <g> Param to get comment, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_DISK					( VO_PID_SOURCE2_METADATA_BASE | 0x000a )	/*!< <g> Param to get disk, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_PUBLISHER				( VO_PID_SOURCE2_METADATA_BASE | 0x000b )	/*!< <g> Param to get publisher, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_ISRC					( VO_PID_SOURCE2_METADATA_BASE | 0x000c )	/*!< <g> Param to get isrc, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_YEAR					( VO_PID_SOURCE2_METADATA_BASE | 0x000d )	/*!< <g> Param to get year, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_BAND					( VO_PID_SOURCE2_METADATA_BASE | 0x000e )	/*!< <g> Param to get band, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_CONDUCTOR				( VO_PID_SOURCE2_METADATA_BASE | 0x000f )	/*!< <g> Param to get conductor, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_REMIXER					( VO_PID_SOURCE2_METADATA_BASE | 0x0010 )	/*!< <g> Param to get remixer, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_LYRICIST				( VO_PID_SOURCE2_METADATA_BASE | 0x0011 )	/*!< <g> Param to get lyrcist, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_RADIOSTATION			( VO_PID_SOURCE2_METADATA_BASE | 0x0012 )	/*!< <g> Param to get rediostation, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_ORIGRELEASEYEAR			( VO_PID_SOURCE2_METADATA_BASE | 0x0013 )	/*!< <g> Param to get org release year, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_OWNER					( VO_PID_SOURCE2_METADATA_BASE | 0x0014 )	/*!< <g> Param to get owner, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_ENCODER					( VO_PID_SOURCE2_METADATA_BASE | 0x0015 )	/*!< <g> Param to get encoder, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_ENCODESETTINGS			( VO_PID_SOURCE2_METADATA_BASE | 0x0016 )	/*!< <g> Param to get encoder setting, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_COPYRIGHT				( VO_PID_SOURCE2_METADATA_BASE | 0x0017 )	/*!< <g> Param to get copy right, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_ORIGARTIST				( VO_PID_SOURCE2_METADATA_BASE | 0x0018 )	/*!< <g> Param to get org artist, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_ORIGALBUM				( VO_PID_SOURCE2_METADATA_BASE | 0x0019 )	/*!< <g> Param to get org album, VO_SOURCE2_METADATA_STRING* */
#define VO_PID_SOURCE2_METADATA_COMPILATION				( VO_PID_SOURCE2_METADATA_BASE | 0x001a )	/*!< <g> Param to get compilation, VO_SOURCE2_METADATA_STRING* */

/**
 * text encoding
 */
#define VO_SOURCE2_METADATA_TE_UNKNOWN				0x0
#define VO_SOURCE2_METADATA_TE_ANSI					0x1
#define VO_SOURCE2_METADATA_TE_UTF8					0x2
#define VO_SOURCE2_METADATA_TE_UTF16				0x4
#define VO_SOURCE2_METADATA_NOTADDSTRINGTERMINATE	0x80000000
#define VO_SOURCE2_METADATA_DYNAMICSTRING			0x40000000

typedef struct
{
	//0xABRRRRRR RRRRRRRR RRRRRRRR TTTTTTTT
	//A<User Set>
	//if set, user tell voMetaData should not add string terminate automatically

	//B<voMetaData Set>
	//if set, voMetaData tell user the string is dynamic allocated
	//you must copy it, otherwise the buffer maybe be released after you call next GetParam

	//T<voMetaData Set>
	//voMetaData tell user the string type, VO_SOURCE2_METADATA_TE_XXX
	VO_U32						nFlags;
	VO_U32						nBufferSize;
	VO_PBYTE					pBuffer;
} VO_SOURCE2_METADATA_STRING;

/**
* image type
*/
typedef enum
{
	VO_SOURCE2_METADATA_IT_UNKNOWN	= 0, 
	VO_SOURCE2_METADATA_IT_JPEG, 
	VO_SOURCE2_METADATA_IT_BMP,
	VO_SOURCE2_METADATA_IT_PNG, 
	VO_SOURCE2_METADATA_IT_GIF, 
	VO_SOURCE2_METADATA_IT_MAX	= VO_MAX_ENUM_VALUE
} VO_SOURCE2_METADATA_IMAGETYPE;

typedef struct
{
	VO_SOURCE2_METADATA_IMAGETYPE		nImageType;
	VO_U32								nBufferSize;
	VO_PBYTE							pBuffer;
} VO_SOURCE2_METADATA_IMAGE;

#endif //__VOSOURCE2_METADATA_H__