	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2013				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CvoTiOmap4HwRenderer.h

	Contains:	TI OMAP4 Hardware Renderer header file, refer to \hardware\ti\omap3\omap3\libstagefrighthw\TIHardwareRenderer.h

	Written by:	East Zhou

	Change History (most recent first):
	2013-02-20	East		Create file

*******************************************************************************/
#ifndef __CvoTiOmap4HwRenderer_H__
#define __CvoTiOmap4HwRenderer_H__

#include "binder/MemoryHeapBase.h"
#include "binder/MemoryBase.h"
#include <utils/RefBase.h>
#include <utils/Vector.h>

#include <OMX_Component.h>

namespace android {

// copy from \hardware\ti\omap3\omap3\liboverlay\overlay_common.h
#ifndef NUM_OVERLAY_BUFFERS_MAX
#define NUM_OVERLAY_BUFFERS_MAX (32)
#endif	// NUM_OVERLAY_BUFFERS_MAX

// copy from \frameworks\base\include\media\stagefright\VideoRenderer.h
typedef void (*release_rendered_buffer_callback)(const sp<IMemory>& mem, void *cookie);
typedef struct {
	uint32_t decoded_width;
	uint32_t decoded_height;
	uint32_t buffercount;
	uint32_t display_width;
	uint32_t display_height;
} render_resize_params;

// copy from \hardware\ti\omap3\omap3\liboverlay\v4l2_utils.h
typedef struct
{
	int fd;
	size_t length;
	uint32_t offset;
	void *ptr;
} mapping_data_t;

class Surface;
class ISurface;
class Overlay;

class Test {
public:
	static sp<ISurface> getISurface(const sp<Surface> &surface)
	{
		return surface->mSurface;
	}
};

class TIHardwareRenderer
{
public:
    TIHardwareRenderer(const sp<Surface> &surface, 
		size_t displayWidth, size_t displayHeight, size_t decodedWidth, size_t decodedHeight, OMX_COLOR_FORMATTYPE colorFormat, 
		int32_t rotationDegrees = 0, int isS3D = 0, int numOfOpBuffers = -1);
    virtual ~TIHardwareRenderer();

public:
    virtual void render(
            const void *data, size_t size, void *platformPrivate);

    Vector< sp<IMemory> > getBuffers() { return mOverlayAddresses; }
    bool setCallback(release_rendered_buffer_callback cb, void *c);

private:
    sp<ISurface> mISurface;
    size_t mDisplayWidth, mDisplayHeight;
    size_t mDecodedWidth, mDecodedHeight;
    OMX_COLOR_FORMATTYPE mColorFormat;
    sp<Overlay> mOverlay;
    Vector< sp<IMemory> > mOverlayAddresses;
    int nOverlayBuffersQueued;
    size_t mIndex;
    sp<MemoryHeapBase> mVideoHeaps[NUM_OVERLAY_BUFFERS_MAX];
    int buffers_queued_to_dss[NUM_OVERLAY_BUFFERS_MAX];
    release_rendered_buffer_callback release_frame_cb;
    void  *cookie;

    TIHardwareRenderer(const TIHardwareRenderer &);
    TIHardwareRenderer &operator=(const TIHardwareRenderer &);

    int mCropX;
    int mCropY;
};

}  // namespace android

#endif  // __CvoTiOmap4HwRenderer_H__
