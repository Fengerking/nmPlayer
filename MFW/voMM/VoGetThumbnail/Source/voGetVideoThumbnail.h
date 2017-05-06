#pragma once

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voIndex.h"
#include "voMem.h"
#include "voString.h"
#include "voIVCommon.h"
#include "voAudio.h"
#include "voVideo.h"
#include "voFile.h"
#include "voStreaming.h"

typedef struct
{
	VO_U32 (VO_API * Init) (VO_HANDLE * phGetThumbnail);
	VO_U32 (VO_API * Uninit) (VO_HANDLE hGetThumbnail);
	VO_U32 (VO_API * GetThumbnail) (VO_HANDLE hGetThumbnail, VO_PTR pSource, VO_U32 nWidth, VO_U32 nHeight, VO_PTR pBitmapHandle);
} VO_THUMBNAILAPI;
/**
 * Get Thumbnail Play API interface
 * \param hGetThumbnail [IN/OUT] Return the API handle.
 * \retval VO_ERR_None Succeeded.
 */
VO_S32 VO_API voGetThumbnailAPI (VO_THUMBNAILAPI * pGetThumbnail);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
