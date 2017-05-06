/****************************************************************************
*
*   Module Title :     Codec_common.h
*
*   Description  :     Common codec definitions header file.
*
****************************************************************************/
#ifndef __INC_COMCODEC_H
#define __INC_COMCODEC_H

/****************************************************************************
*  Include Files
****************************************************************************/
//#include <string.h>
#include "type_aliases.h"
#include "voVP6DecID.h"

/****************************************************************************
*  Macros
****************************************************************************/

//elif _X86_

#if __GNUC__
#	define ALIGN16 __attribute__ (( __aligned__ (16) ))
#elif __ICC
#	define ALIGN16 __declspec(align(16))
#else
#	define ALIGN16 __declspec(align(16))
#endif



// Platform Specific Inlines
#ifndef RVDS
#define FORCEINLINE __forceinline
#ifndef INLINE
#define INLINE      __inline
#endif
//#define FORCEINLINE
//#define INLINE
#else
#define FORCEINLINE
#undef INLINE
#define INLINE
#endif


// Platform specific math function defines
#define LIMIT(x)    ( (x)<0 ? 0: (x)>255 ? 255: (x) )
/****************************************************************************
*  Module constants.
****************************************************************************/
#define BASE_FRAME              0
#define NORMAL_FRAME            1
#define Q_TABLE_SIZE            64
#define BLOCK_HEIGHT_WIDTH      8
#define BLOCK_SIZE              (BLOCK_HEIGHT_WIDTH * BLOCK_HEIGHT_WIDTH)


/****************************************************************************
*  Types
****************************************************************************/

/* Type defining YUV data elements. */
typedef UINT8 YUV_BUFFER_ENTRY;
typedef UINT8 *YUV_BUFFER_ENTRY_PTR;

typedef struct CONFIG_TYPE
{
    // The size of the surface we want to draw to
    UINT32 VideoFrameWidth;
    UINT32 VideoFrameHeight;

    INT32 YStride;
    INT32 UVStride;

    // The number of horizontal and vertical blocks encoded
    UINT32 HFragPixels;
    UINT32 VFragPixels;

    // The Intended Horizontal Scale
    UINT32 HScale;
    UINT32 HRatio;

    // The Intended Vertical Scale
    UINT32 VScale;
    UINT32 VRatio;

    // The way in which we intended
    UINT32 ScalingMode;

    UINT32 reserved;

	UINT32 ExpandedFrameWidth;
	UINT32 ExpandedFrameHeight;

} CONFIG_TYPE;

typedef struct
{
    INT16   x;
    INT16   y;
} MOTION_VECTOR;

typedef union {
	MOTION_VECTOR vect;
	INT32         n;
} MOTION_U;

typedef MOTION_VECTOR COORDINATE;
typedef INT16           Q_LIST_ENTRY;
typedef Q_LIST_ENTRY    Q_LIST[64];

#endif
