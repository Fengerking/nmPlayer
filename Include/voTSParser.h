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


#ifndef __VO_TS_PARSE_H_
#define __VO_TS_PARSE_H_

#include "voVideo.h"
#include "voParser.h"
#include "voString.h"


/**
* TS Parser output type, refer to VO_PARSER_OUTPUT_TYPE
*/
typedef enum
{
	// tag: 20100417, handle VO_PARSER_OT_STREAMINFO
	VO_PARSER_OT_TS_PROGRAM_INFO		= (VO_PARSER_OT_BASE_TS | 0x0001),// see VOSTREAMPARSEPROGRAMINFO	
	VO_PARSER_OT_TS_PACKET_LOSS			= (VO_PARSER_OT_BASE_TS | 0x0002)

}VO_TS_PARSER_OUTPUT_TYPE;


/**
* EPG Item
*/
typedef struct
{
	VO_U32 id;
	VO_TCHAR name[128];
	VO_TCHAR description[256];
	unsigned long long start_time;
	int duration;
}
VOSTREAMPARSEEPGITEM;

/**
* EPG info
*/
typedef struct
{
	VO_U16 count; /*!< Count of EPG items */
	VOSTREAMPARSEEPGITEM* items;
}
VOSTREAMPARSEEPGINFO;

/**
* Program info
*/
typedef struct
{
	VO_U32 id;  /*!< program id */
	VO_U32 pid_count; /*!< How many TS pid belongs to this program */
	VO_U32 pids[16];  /*!< The TS pids belong to this program */
	VO_TCHAR name[64];
	VOSTREAMPARSEEPGINFO EPG;
}
VOSTREAMPARSEPROGRAMINFO;


/**
* Return code for stream parse, see _VOCOMMONRETURNCODE for general return code
*/
enum _VOSTREAMPARSERETURNCODE
{
	VORC_STREAMPARSE_OK				= VO_ERR_PARSER_OK,
	VORC_STREAMPARSE_ERROR			= VO_ERR_PARSER_BASE_TS | 0x0001,	/*!< General parse error */
	VORC_STREAMPARSE_INVALID_ARG    = VO_ERR_PARSER_BASE_TS | 0x0002,
	VORC_STREAMPARSE_OUT_OF_MEMORY  = VO_ERR_PARSER_BASE_TS | 0x0003,
	VORC_STREAMPARSE_NOT_IMPLEMENT	= VO_ERR_PARSER_BASE_TS | 0x0004,
	VORC_STREAMPARSE_NOT_HANDLE     = VO_ERR_PARSER_BASE_TS | 0x0005,
	VORC_STREAMPARSE_PACKET_LOSS    = VO_ERR_PARSER_BASE_TS | 0x0006,
};

/**
* TS parser parameter ID
*/
enum
{
	VO_PID_PARSE_SET_PARSER_TYPE	= VO_PID_PARSER_TS_BASE | 0x0001,	/*!< Refer to VOTSPARSETYPE*/
    VO_PID_DO_TS_FORCE_PLAY         = VO_PID_PARSER_TS_BASE | 0x0002,
    VO_PID_SET_EIT_CAPTION_CALLBACK = VO_PID_PARSER_TS_BASE | 0x0003,   /*!Set the Callback func for Caption*/
};

typedef enum
{
	PARSE_PAT		= 1,
	PARSE_PMT		= 2,
	PARSE_SDT		= 4,
	PARSE_NIT		= 8,
	PARSE_EIT		= 0x10,
	PARSE_PES		= 0x20,
	PARSE_PLAYBACK	= PARSE_PAT | PARSE_PMT | PARSE_PES,
	PARSE_PLAYBACK2 = PARSE_PLAYBACK | PARSE_SDT,
	PARSE_BASEEPG	= PARSE_PAT | PARSE_PMT | PARSE_SDT,
	PARSE_FULLEPG	= PARSE_BASEEPG | PARSE_EIT,
	PARSE_ALL		= 0xffffffff
}VO_TSPARSETYPE;

///<add packet lost info
typedef struct  
{
	VO_U32 nStreamId;
	VO_U32 nErrorCode;
}VO_TS_PACKET_LOST_INFO;


#endif /* __VO_TS_PARSE_H_ */


