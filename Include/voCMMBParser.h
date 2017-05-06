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



#ifndef __VO_CMMB_PARSER_H_
#define __VO_CMMB_PARSER_H_

#include "voString.h"
#include "voParser.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)

#define MAX_SERVICE_COUNT		40
#define MAX_CA_SERVICE_COUNT	8
#define MAX_CA_SYSTEM_COUNT		8
#define MAX_ESG_SERVICE_COUNT	8
#define MAX_PROGRAM_THEME_LEN	256


#define	VO_PID_PARSER_CMMB_CA_TYPE			(VO_PID_PARSER_CMMB_BASE | 0x0001)		/*!< param ID for CA system type */


/**
* CMMB Parser output type, refer to VO_PARSER_OUTPUT_TYPE
*/
typedef enum
{
	//TS0
	VO_PARSER_OT_CMMB_TS0_INFO		= (VO_PARSER_OT_BASE_CMMB | 0x0001),// see VOTS0INFO			

	//ESG
	VO_PARSER_OT_CMMB_SERVICE_INFO,			// see VOSERVICEINFO
	VO_PARSER_OT_CMMB_PROGRAM_GUIDE,		// see VOPROGRAMGUIDE
	VO_PARSER_OT_CMMB_CONTENT_INFO,			// see VOCONTENTINFO
	VO_PARSER_OT_CMMB_SCHEDULE_INFO,		// see VOSCHEDULEINFO
	VO_PARSER_OT_CMMB_SERVICE_AUX_INFO,		// see VOSERVICEAUDIXINFO
	VO_PARSER_OT_CMMB_SERVICE_PARAM_INFO,	// see VOSERVICEPARAMINFO
	VO_PARSER_OT_CMMB_ESG_PARSE_FINISHED,

	//Descrambling
	VO_PARSER_OT_CMMB_DESCRAMBLING,			// see VODESCRAMBLING

	// MF header
	VO_PARSER_OT_CMMB_MF_HEADER_INFO,		// see VOMFHEADERINFO

	// MSF header
	VO_PARSER_OT_CMMB_MSF_HEADER_INFO,		// see VOMSFHEADERINFO

	//Error
	VO_PARSER_OT_CMMB_FAILED,

	// Data
	VO_PARSER_OT_CMMB_PACKET_DATA			// see VODATABUFFER

}VO_CMMB_PARSER_OUTPUT_TYPE;

typedef enum
{
	CLASS_RESERVED				= 0,
	CLASS_TV					= 1,
	CLASS_AUDIO_BROADCAST		= 2,
	CLASS_VIDEO_FRAGMENT		= 3,
	CLASS_FILE_DOWNLOAD			= 4,
	CLASS_SOFTWARE_MANAGMENT	= 5,
	CLASS_ESG					= 6,
	CLASS_STREAM_DATA_SERVICE	= 7
}VOSERVICECLASS;


typedef struct  
{
	VO_BYTE				esg_service_count;
	VO_U16				esg_service_id[MAX_ESG_SERVICE_COUNT];

}VOESGBASICDESCRIPTION;


typedef struct  
{
	VO_U16				mf_id;
	VO_U16				service_id;
	VO_U32				freq;
	VO_BYTE				time_slot_count;
	VO_BYTE				time_slot_start;

	VO_BYTE				rs;
	VO_BYTE				byte_interlace;
	VO_BYTE				ldpc;
	VO_BYTE				demod;

	VO_BYTE				scramble_code;
}VOCONTINUESERVICEINFO;


typedef struct  
{
	VO_BYTE			count;
	struct  
	{
		VO_TCHAR	theme[MAX_PROGRAM_THEME_LEN];
		VO_U32		theme_len;
		VODATETIME	start_time;
		VO_U32		duration;
	}guide[4];

}VOPROGRAMGUIDE;


typedef enum
{
	CONTENT_TEXT		= 0,	//"text"
	CONTENT_IMAGE		= 1,	//"image"
	CONTENT_AUDIO		= 2,	//"audio"
	CONTENT_VIDEO		= 3,	//"video"
	CONTENT_APPLICATION	= 4,	//"application"
	CONTENT_MULTIPART	= 5		//"multipart"
}VOCONTENTCLASS;


typedef enum
{
}VOCONTENTGENRE;


typedef enum
{
	LANGUAGE_SC		= 0,
	LANGUAGE_EN		= 1,
	LANGUAGE_TIB	= 2
}VOLANGUAGE;

typedef struct
{
	VO_BYTE	broadcast_sn;
	//VO_U16	broadcast_data_len;
	//VO_BYTE*	broadcast_data_ptr;

	struct EBDATASEGMENT 
	{
		VO_U16	net_level;		// 4 bits
		VO_U16	net_id;			// 12 bits;
		VO_U16	msg_id;

		//VO_U16	reseved;
		VO_U16	eb_data_len;
		//VO_BYTE*	eb_data;

		VO_BYTE	trigger_flag;			// 1 bit

		// trigger_flag == 0
		struct EBMSG0 
		{	
			VO_BYTE	trigger_type;			// 7 bits
			//VO_BYTE	eb_level;
			VO_BYTE	eb_charset;
			//VO_BYTE	reserved1;

			VO_U64	eb_start_time	:40;
			VO_U64	eb_duration		:20;
			VO_U64	text_data_count	:4;		// language 

			struct TEXTDATA
			{
				VO_U32	language_type;	// 24 bits
				
				VO_U16	organization_flag;
				VO_U16	ref_service_flag;
				//VO_U16	reserved1;
				VO_U16	text_data_len;

				VO_BYTE*	text_data_ptr;

				VO_BYTE	organization_name_len;	// 8 bits
				VO_BYTE*	organization_name_ptr;	// 0 ~ 255

				VO_U16	ref_service_id;			// 16 bits

				//VO_BYTE	reserved2	:4;
				VO_BYTE	aux_data_idx;
			}text_data[15];

			//
			//VO_BYTE	reserved2		:4;
			VO_BYTE	aux_data_count;
			
			struct AUXDATA
			{
				VO_BYTE	aux_data_type;	// 8 bits
				VO_U16	aux_data_len;	// 16 bits
				VO_BYTE*	aux_data_ptr;
			}aux_data[15];
		}eb_msg0;

		// trigger_flag == 1
		struct EBMSG1
		{
			VO_BYTE	trigger_type;			// 7 bits

			VO_BYTE	trigger_msg_level	:3;
			VO_BYTE	reserved1			:5;

			VO_U16	trigger_net_level	:4;
			VO_U16	trigger_network_sn	:12;

			VO_BYTE	trigger_freq_point;
			VO_U32	trigger_center_freq;

			VO_BYTE	trigger_band_width	:4;
			VO_BYTE	reserved2			:4;
		}eb_msg1;
		
	}eb_data_segment;

}VOEBINFO;

typedef struct
{
	VO_BYTE					ca_update_sn;

	VO_BYTE					ca_service_count;
	VO_U16					ca_service_id[MAX_CA_SERVICE_COUNT];

	VO_BYTE					ca_system_count;
	VO_U16					ca_system_id[MAX_CA_SYSTEM_COUNT];

	VO_BYTE*				data_ptr;
	VO_BYTE					data_len;
}VOCAINFO;

typedef struct  
{
	VO_U16			service_id;
	VO_U16			service_param_id;
	VO_TCHAR*		service_name;
	VOSERVICECLASS	service_class;
	VO_BOOL			service_free;
	VOCAINFO		ca_info;
}VOSERVICEINFO;


typedef struct  
{
	VO_BYTE			usage;
	VO_TCHAR*		content_class;
	VO_VOID*		media_data;
	VO_TCHAR*		media_uri;
}VOMEDIATYPE;


typedef struct  
{
	VO_TCHAR*		title;
	VO_U16			content_id;
	VOCONTENTGENRE	content_genre;
	VO_TCHAR*		content_class;
	VO_TCHAR*		digest_info;
	VO_U16			duration;
	VOLANGUAGE		audio_lang;
	VOLANGUAGE		subtitle_lang;
}VOCONTENTINFO;


typedef struct  
{
	VO_TCHAR*		title;
	VO_TCHAR*		ca_description;
	VOLANGUAGE		lang;
	VO_U16			schedule_id;
	VO_U16			service_id;
	VO_U16			content_id;
	VO_TCHAR*		date;
	VO_TCHAR*		time;
	VO_BOOL			free;
	VO_BOOL			live;
	VO_BOOL			repeat;
}VOSCHEDULEINFO;


typedef struct  
{
	VO_TCHAR*		lang_special;
	VO_U16			service_id;
	VO_TCHAR*		lang;
	VO_TCHAR*		service_description;
	VO_TCHAR*		service_provider;
	VOMEDIATYPE		media_type;
}VOSERVICEAUDIXINFO;


typedef struct  
{
	VO_U16			service_param_id;
	VO_VOID*		para;				//reserved
}VOSERVICEPARAMINFO;

typedef struct
{
	VO_BYTE	nit_update_sn;
	VO_U16	net_id;
}VONITINFO;

typedef struct  
{
	//CA service id
	VOCAINFO				ca_info;

	//NIT
	VONITINFO				nit_info;

	//
	VOESGBASICDESCRIPTION	esg_desc;

	VO_BYTE					continue_service_count;
	VOCONTINUESERVICEINFO	continue_service[MAX_SERVICE_COUNT];

	//EB
	VOEBINFO				eb;
}VOTS0INFO;

//CA
typedef enum
{
	WHOLE_MSF		= 0,
	MULTIPLEX_SEGMENT_AUDIO,
	MULTIPLEX_SEGMENT_VIDEO,
	MULTIPLEX_SEGMENT_DATA,
	MULTIPLEX_SEGMENT_ECM,
	MULTIPLEX_SEGMENT_EMM
}VODESCRAMBLETYPE;

typedef struct
{
	VO_BYTE		type;				/*!< [in]Refer to VODESCRAMBLETYPE */
	VO_U16		mf_id;				/*!< [in]MF id */
	VO_BYTE*	encrypt_data_ptr;	/*!< [in]Encrypt data pointer */
	VO_U16		encrypt_data_len;	/*!< [in]Encrypt data length */
	VO_BYTE		ecm_emm_type;		/*!< [in]ECM EMM type */

	VO_BYTE*	decrypt_data_ptr;	/*!< [out]Decrypt data pointer */
	VO_U16		decrypt_data_len;	/*!< [out]Decrypt data length */
	VO_BOOL		encrypt;			/*!< [out]Set true if describling successed, default is false */

}VODESCRAMBLING;

typedef struct
{
	VO_U16		mf_id;
	VO_BYTE		eb_flag;			/*!< Emergency broadcast flag, "0" indicate no message, ">0" indicate has message*/
}VOMFHEADERINFO;

typedef struct
{
	VO_BYTE	encrypt_flag;		/*!< Encrypt flag, "0" indicate no encryption, "1" indicate has been decrypted, "2" indicate encrypted, "3" reserved*/
}VOMSFHEADERINFO;

typedef enum
{
	CARD_NAGRA		= 0,
	CARD_KTOUCH
}VOCACARDTYPE;

typedef enum
{
	XPE_STREAM		= 0,
	XPE_FILE		= 1,
	XPE_FEC_STREAM	= 2,
	XPE_FEC_FILE	= 3
}VOXPETYPE;

typedef struct
{
	VO_BYTE*	data;
	VO_U32	data_len;
	VO_BYTE	data_type;	// refer to VOXPETYPE

}VODATABUFFER;


#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* __VO_CMMB_PARSER_H_ */



