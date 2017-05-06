	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXThirdPartyDef.h

	Contains:	

	Written by:	

	Change History (most recent first):
	2011-09-26		Eric			Create file

*******************************************************************************/

#ifndef _voCOMXThirdPartyDef_H_
#define _voCOMXThirdPartyDef_H_

//////////////////////////////////////////////////////////////////////////
//Common definition

// For HoneyComb HW codec integration
struct EnableAndroidNativeBuffersParams
{
	OMX_U32	nSize;
	OMX_VERSIONTYPE nVersion;
	OMX_U32 nPortIndex;
	OMX_BOOL enable;
};

// A pointer to this struct is passed to OMX_GetParameter when the extension
// index for the 'OMX.google.android.index.getAndroidNativeBufferUsage'
// extension is given.  The usage bits returned from this query will be used to
// allocate the Gralloc buffers that get passed to the useAndroidNativeBuffer
// command.
struct GetAndroidNativeBufferUsageParams {
    OMX_U32 nSize;              // IN
    OMX_VERSIONTYPE nVersion;   // IN
    OMX_U32 nPortIndex;         // IN
    OMX_U32 nUsage;             // OUT
};

//end common

//////////////////////////////////////////////////////////////////////////
// QCM Definitions
//////////////////////////////////////////////////////////////////////////
typedef enum OMX_QCOMFramePackingFormat
{
	OMX_QCOM_FramePacking_Unspecified,
	OMX_QCOM_FramePacking_Arbitrary,
	OMX_QCOM_FramePacking_CompleteFrames,
	OMX_QCOM_FramePacking_OnlyOneCompleteFrame,
	OMX_QCOM_FramePacking_OnlyOneCompleteSubFrame,
	OMX_QCOM_FramePacking_MAX = 0X7FFFFFFF
} OMX_QCOMFramePackingFormat;

typedef enum OMX_QCOMMemoryRegion
{
	OMX_QCOM_MemRegionInvalid,
	OMX_QCOM_MemRegionEBI1,
	OMX_QCOM_MemRegionSMI,
	OMX_QCOM_MemRegionMax = 0X7FFFFFFF
} OMX_QCOMMemoryRegion;

typedef enum OMX_QCOMCacheAttr
{
	OMX_QCOM_CacheAttrNone,
	OMX_QCOM_CacheAttrWriteBack,
	OMX_QCOM_CacheAttrWriteThrough,
	OMX_QCOM_CacheAttrMAX = 0X7FFFFFFF
} OMX_QCOMCacheAttr;

typedef struct OMX_QCOM_PARAM_PORTDEFINITIONTYPE {
	OMX_U32 nSize;						/** Size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;			/** OMX specification version information */
	OMX_U32 nPortIndex;					/** Port index which is extended by this structure */
	OMX_QCOMMemoryRegion nMemRegion;	/** Platform specific memory region EBI1, SMI, etc.,*/
	OMX_QCOMCacheAttr nCacheAttr;		/** Cache attributes */
	OMX_U32 nFramePackingFormat;		/** Input or output buffer format */
} OMX_QCOM_PARAM_PORTDEFINITIONTYPE;

enum OMX_QCOM_EXTN_INDEXTYPE
{
	OMX_QcomIndexRegmmap = 0x7F000000,
	OMX_QcomIndexPlatformPvt = 0x7F000001,
	OMX_QcomIndexPortDefn = 0x7F000002,
	OMX_QcomIndexPortFramePackFmt = 0x7F000003,
	OMX_QcomIndexParamInterlaced = 0x7F000004,
	OMX_QcomIndexConfigInterlaced = 0x7F000005,
	QOMX_IndexParamVideoSyntaxHdr = 0x7F000006,
	QOMX_IndexConfigVideoIntraperiod = 0x7F000007,
	QOMX_IndexConfigVideoIntraRefresh = 0x7F000008,
	QOMX_IndexConfigVideoTemporalSpatialTradeOff = 0x7F000009,
	QOMX_IndexParamVideoEncoderMode = 0x7F00000A,
	OMX_QcomIndexParamVideoDivx = 0x7F00000B,
	OMX_QcomIndexParamVideoSpark = 0x7F00000C,
	OMX_QcomIndexParamVideoVp = 0x7F00000D,
	OMX_QcomIndexQueryNumberOfVideoDecInstance = 0x7F00000E,
	OMX_QcomIndexParamVideoSyncFrameDecodingMode = 0x7F00000F,
	OMX_QcomIndexParamVideoDecoderPictureOrder = 0x7F000010,
	OMX_QcomIndexConfigVideoFramePackingArrangement = 0x7F000011,
	OMX_QcomIndexParamConcealMBMapExtraData = 0x7F000012,
	OMX_QcomIndexParamFrameInfoExtraData = 0x7F000013,
	OMX_QcomIndexParamInterlaceExtraData = 0x7F000014,
	OMX_QcomIndexParamH264TimeInfo = 0x7F000015
};

// here no official confirmed, only guessed by East from omx_vdec.cpp, 20110417
typedef struct OMX_QCOMVC1WMV9_CONFIGDATA
{
	VO_U32		start_code;		// always be 0xc5ffffff
	VO_U32		head_data_size;	// always be 4 for WMV9
	VO_BYTE		head_data[4];
	VO_U32		height;			// why height is ahead??
	VO_U32		width;
	VO_U32		c_data_size;	// always be 12
	VO_BYTE		c_data[12];		// I only set to 0 all, it can works, but don't know the mean of this data
} OMX_QCOMVC1WMV9_CONFIGDATA;

typedef struct OMX_QCOM_FRAME_PACK_ARRANGEMENT
{
	OMX_U32 nSize;
	OMX_VERSIONTYPE nVersion;
	OMX_U32 nPortIndex;
	OMX_U32 id;
	OMX_U32 cancel_flag;
	OMX_U32 type;
	OMX_U32 quincunx_sampling_flag;
	OMX_U32 content_interpretation_type;
	OMX_U32 spatial_flipping_flag;
	OMX_U32 frame0_flipped_flag;
	OMX_U32 field_views_flag;
	OMX_U32 current_frame_is_frame0_flag;
	OMX_U32 frame0_self_contained_flag;
	OMX_U32 frame1_self_contained_flag;
	OMX_U32 frame0_grid_position_x;
	OMX_U32 frame0_grid_position_y;
	OMX_U32 frame1_grid_position_x;
	OMX_U32 frame1_grid_position_y;
	OMX_U32 reserved_byte;
	OMX_U32 repetition_period;
	OMX_U32 extension_flag;
} OMX_QCOM_FRAME_PACK_ARRANGEMENT;

typedef struct QOMX_ENABLETYPE {
	OMX_BOOL bEnable;
} QOMX_ENABLETYPE;

// end of QCM definition

//////////////////////////////////////////////////////////////////////////
// STE special OMX config
//////////////////////////////////////////////////////////////////////////

// 1. DTS handling in OMX component
typedef struct VFM_PARAM_TIMESTAMPDISPLAYORDERTYPE {
	/// @brief Size of the structure
	OMX_U32 nSize;
	/// @brief Version of this structure
	OMX_VERSIONTYPE nVersion;
	/// @brief TRUE to indicate timestamps are in display order in a decoder
	/// Default value is FALSE
	OMX_BOOL bDisplayOrder;
} VFM_PARAM_TIMESTAMPDISPLAYORDERTYPE;
// 2. Buffer recycling delay in OMX decoder output
typedef struct VFM_PARAM_RECYCLINGDELAYTYPE {
	/// @brief Size of the structure
	OMX_U32 nSize;
	/// @brief Version of this structure
	OMX_VERSIONTYPE nVersion;
	/// @brief Recycling delay, in number of frames in the output, to retain some frames.
	/// Default value is 0 (no delay)
	OMX_U32 nRecyclingDelay;
} VFM_PARAM_RECYCLINGDELAYTYPE;
// 3. MMHwBuffer handle in pPlatformPrivate
typedef struct VFM_PARAM_SHAREDCHUNKINPLATFORMPRIVATETYPE {
	/// @brief Size of the structure
	OMX_U32 nSize;
	/// @brief Version of this structure
	OMX_VERSIONTYPE nVersion;
	/// @brief TRUE to indicate that the pPlatformPrivate must contain the shared chunk metadata on FillBufferDone
	/// Default value is FALSE
	/// Can be used in Android to provide data to the renderer
	OMX_BOOL bSharedchunkInPlatformprivate;
} VFM_PARAM_SHAREDCHUNKINPLATFORMPRIVATETYPE;

/// @brief Used to set static dpb size
typedef struct VFM_PARAM_STATICDPBSIZE_TYPE
{
	/// @brief Size of the structure
	OMX_U32 nSize;
	/// @brief Version of this structure
	OMX_VERSIONTYPE nVersion;
	/// @brief OMX_TRUE if it static dpb size is required
	OMX_BOOL bStaticDpbSize;
} VFM_PARAM_STATICDPBSIZE_TYPE;

// For STE platform
#define STE_H264DEC_MAX_1080P_BUFFERS 21 
// end of ste

//
#endif
