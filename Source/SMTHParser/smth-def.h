
//create 2010.11.4 by hc


#ifndef _SMTH_DEF_H
#define	_SMTH_DEF_H


#include "voToolUtility.h"
#define __NEW_SWAPENDIAN
#ifndef __NEW_SWAPENDIAN
#define be16toh(x) \
(	  \
	(x &(FF <<8))>> 8 | (x & FF)<<8 \
)
#define be24toh(x) \
(	  \
	(x &(FF <<16))>> 16 | (x & (FF<<8))  | (x & FF)<<16 \
)
#define be32toh(x) \
(	  \
	 (x &(FF<<24))>> 24 | (x & (FF<<16)) >>16 << 8 | (x &(FF<<8)) >>8 <<16 | (x & FF)<<24 \
)

/* #define be64toh(x) \
 (	\
 	(x &(FF<<56))>>56 | (x & (FF<<48)) >>48 << 8 | (x &(FF<<40)) >>40 <<16 \
 		| (x & (FF<<32))>>32<<24 | (x & (FF<<24))>>24<<32 | (x & (FF<<16))>>16<<40 | (x & (FF<<8))>>8<<48 | (x & FF)>>0<<56  \
 )*/

#define be64toh(x) \
(	\
	(x >>56 &0xFF) | (x >>48& 0xFF)<< 8 | (x >>40 &0xFF)<<16 	| (x >>32&0xFF)<<24 \
	| (x >>24 & 0xFF)<<32 | (x >>16& 0xFF)<<40 | (x >>8 & 0xFF)<<48 | (x  & 0xFF)<<56  \
	)
#endif

#define CHECK_FAIL(nRC) \
	if(nRC != VO_ERR_NONE)\
	{ \
		VOLOGE("CHECK FAIL");\
		return nRC;\
	}

/** BOX TYPE */
typedef enum
{  
				VO_MOOF = 0,
				VO_MFHD, 
				VO_TRAF,    
				VO_UUID, 
				VO_TFHD,    
				VO_TRUN,   
				VO_MDAT,  
				VO_SDTP,    
				VO_UNKNOWN	
} VO_SMTH_BOXTYPE;

/** DEFINED IN TFHD BOX **/
typedef struct 
{	
	VO_U64 base_data_offset;
	VO_U32 sample_description_index;
	VO_U32 default_sample_duration;
	VO_U32 default_sample_size;
	VO_U32 default_sample_flags;
} VO_SMTH_DEFAULTSAMPLE_INFO;
/**DEFINED IN TRUN BOX**/
typedef struct
{
	VO_U32 sample_duration;
	VO_U32 sample_size;
	VO_U32 sample_flags;
	VO_S32 sample_composition_time_offset;

	VO_U8  nKey;
} VO_SMTH_SAMPLE_INFO;

typedef struct 
{  
	VO_U32 index;
	/**track_ID**/
	VO_U32 track_id;
	VO_U32 sampleno;
	VO_S32 data_offset;
	VO_U32 settings;
	VO_U64 timestamp;
	VO_U64 duration;
	VO_SMTH_DEFAULTSAMPLE_INFO defaults;
	VO_SMTH_SAMPLE_INFO *samples;
	VO_U64 size;
	VO_PBYTE data;
} VO_SMTH_FRAGMENT_INFO;


/**add by leon**/
/** Define input stream type,0 for Video ,1 for Audio*/
typedef VO_PARSER_OUTPUT_TYPE VO_PARSER_INPUT_TYPE;
typedef struct 
{	
	VO_U64 length_t;  /* size of buffer*/
	VO_PARSER_INPUT_TYPE type;   /* incoming Stream type Audio or video*/
	VO_PBYTE pData;/* buffer memory */
}VO_SMTH_FRAGMENT;

typedef struct 
{   
	VO_S64 curSize; /**< size of the incoming block body */
	VO_SMTH_BOXTYPE type;	      /**box type	*/
	VO_SMTH_FRAGMENT_INFO *fragmentInfo;          /** fragment info */ 
} VO_SMTH_BOX;


#define VO_NULL			0

#define VO_SMTH_TFHD_BASE_DATA_OFFSET_PRESENT								0x000001
#define VO_SMTH_TFHD_SAMPLE_DESCRIPTION_INDEX_PRESENT				0x000002
#define VO_SMTH_TFHD_DEFAULT_SAMPLE_DURATION_PRESENT				0x000008
#define VO_SMTH_TFHD_DEFAULT_SAMPLE_SIZE_PRESENT						0x000010
#define VO_SMTH_TFHD_DEFAULT_SAMPLE_FLAGS_PRESENT					0x000020
#define VO_SMTH_TFHD_DURATION_IS_EMPTY											0x010000

#define VO_SMTH_TRUN_DATA_OFFSET_PRESENT										0x000001																										
#define VO_SMTH_TRUN_FIRST_SAMPLE_FLAGS_PRESENT							0x000004
#define VO_SMTH_TRUN_SAMPLE_DURATION_PRESENT							0x000100
#define VO_SMTH_TRUN_SAMPLE_SIZE_PRESENT										0x000200
#define VO_SMTH_TRUN_SAMPLE_FLAGS_PRESENT									0x000400
#define VO_SMTH_TRUN_SAMPLE_COMPOSITION_TIME_OFFSET_PRESENT			0x000800


//PIFF: Protected Interoperable File Format
#define VO_PIFF_OVERRIDE_TRACKENCRYPTIONBOX									0X000001
#define VO_PIFF_USE_SUBSAMPLE_ENCRYPTION										0X000002


typedef char uuid[16];
/** The uuid of a SampleEncryptionBox, namely a specific UUIDBox. */
static const uuid SAMPLE_ENCRYTION_UUID = {  0xa2, 0x39, 0x4f, 0x52,
0x5a, 0x9b, 0x4f, 0x14,
0xa2, 0x44, 0x6c, 0x42,
0x7c, 0x64, 0x8d, 0xf4 };
/** The uuid of a ProtectionSystemSpecificHeaderBox, namely a specific UUIDBox. */
static const uuid PROTECTION_HEADER_UUID = {  0xd0, 0x8a, 0x4f, 0x18,
0x10, 0xf3, 0x4a, 0x82,
0xb6, 0xc8, 0x32, 0xd8,
0xab, 0xa1, 0x83, 0xd3 };

static const uuid PLAYREADY_SYSTEM_ID = {  0x9A, 0x04, 0xF0, 0x79,
0x98, 0x40, 0x42, 0x86,
0xAB, 0x92, 0xE6, 0x5B,
0xE0, 0x88, 0x5F, 0x95 };

#endif