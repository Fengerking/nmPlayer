/*
**
** Copyright 2008, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef ANDROID_voVideoRenderType_H
#define ANDROID_voVideoRenderType_H

namespace android {

typedef void (* VOPACKUVAPI) (unsigned char* dstUV, unsigned char* srcU, unsigned char* srcV,
								int strideU, int strideV, int rows, int width, int stridedest);

#define	PLATFORM_PRIVATE_PMEM	1
#define OMX_COLOR_Format_QcomHW	0X7FA30C00

// data structures for tunneling buffers
typedef struct PLATFORM_PRIVATE_PMEM_INFO
{
    /* pmem file descriptor */
    int		pmem_fd;
    int		offset;
} PLATFORM_PRIVATE_PMEM_INFO;

typedef struct PLATFORM_PRIVATE_ENTRY
{
    /* Entry type */
	int	 type;

    /* Pointer to platform specific entry */
    void * entry;
} PLATFORM_PRIVATE_ENTRY;

typedef struct PLATFORM_PRIVATE_LIST
{
    /* Number of entries */
	int nEntries;

    /* Pointer to array of platform specific entries *
     * Contiguous block of PLATFORM_PRIVATE_ENTRY elements */
    PLATFORM_PRIVATE_ENTRY* entryList;
} PLATFORM_PRIVATE_LIST;

/**
 * Defination of color format
 */
typedef enum
{
	VO_ANDROID_COLOR_YUV420				= 0,		/*!< YUV planar mode:420  */
	VO_ANDROID_COLOR_NV12				= 1,		/*!< YUV Y planar UV interlace */
	VO_ANDROID_COLOR_RGB565				= 10,		/*!< RGB565  */
	VO_ANDROID_COLOR_RGB888				= 11,		/*!< RGB888  */
	VO_ANDROID_COLOR_TYPE_MAX			= 0X7FFFFFFF
} VO_ANDROID_COLORTYPE;

/**
 * Video data buffer, usually used as iutput video information.
 */
typedef struct
{
	unsigned char *			virBuffer[3];			/*!< virtual Buffer pointer */
	unsigned char *			phyBuffer[3];			/*!< physical Buffer pointer */
	int						nStride[3];				/*!< Buffer stride */
	VO_ANDROID_COLORTYPE	nColorType;				/*!< color type  */
} VO_ANDROID_VIDEO_BUFFERTYPE;

}; // namespace android

#endif // ANDROID_voVideoRenderType_H

