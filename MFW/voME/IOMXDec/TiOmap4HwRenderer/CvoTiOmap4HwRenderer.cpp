	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2013				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CvoTiOmap4HwRenderer.cpp

	Contains:	TI OMAP4 Hardware Renderer source file, refer to \hardware\ti\omap3\omap3\libstagefrighthw\TIHardwareRenderer.cpp

	Written by:	East Zhou

	Change History (most recent first):
	2013-02-20	East		Create file

*******************************************************************************/
#include <surfaceflinger/ISurface.h>
#include <surfaceflinger/Surface.h>
#include <ui/Overlay.h>
#include "voOMXOSFun.h"
#include "CvoTiOmap4HwRenderer.h"

#define LOG_TAG "CvoTiOmap4HwRenderer"
#include "voLog.h"

namespace android {

#define ARMPAGESIZE	4096

// copy from \hardware\ti\omap3\omap3\liboverlay\overlay_common.h
#define NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE	2
#define OPTIMAL_QBUF_CNT	0x4
#define SET_CLONE_FD		0x8

// copy from \hardware\ti\omx\omx\ducati\domx\system\omx_core\inc\OMX_TI_IVCommon.h
typedef enum OMX_TI_COLOR_FORMATTYPE {
	OMX_TI_COLOR_FormatYUV420PackedSemiPlanarInterlaced = (OMX_COLOR_FORMATTYPE) OMX_COLOR_FormatVendorStartUnused + 1,
	OMX_TI_COLOR_FormatYUV420PackedSemiPlanar_Sequential_TopBottom = OMX_TI_COLOR_FormatYUV420PackedSemiPlanarInterlaced,
	OMX_TI_COLOR_FormatRawBayer10bitStereo = OMX_COLOR_FormatVendorStartUnused + 2 /**< 10 bit raw for stereo */
} OMX_TI_COLOR_FORMATTYPE;

////////////////////////////////////////////////////////////////////////////////

static int Calculate_TotalRefFrames(int nWidth, int nHeight) {
    LOGD("Calculate_TotalRefFrames");
    uint32_t ref_frames = 0;
    uint32_t MaxDpbMbs;
    uint32_t PicWidthInMbs;
    uint32_t FrameHeightInMbs;

    MaxDpbMbs = 32768; //Maximum value for upto level 4.1

    PicWidthInMbs = nWidth / 16;

    FrameHeightInMbs = nHeight / 16;

    ref_frames =  (uint32_t)(MaxDpbMbs / (PicWidthInMbs * FrameHeightInMbs));

    LOGD("nWidth [%d] PicWidthInMbs [%d] nHeight [%d] FrameHeightInMbs [%d] ref_frames [%d]",
        nWidth, PicWidthInMbs, nHeight, FrameHeightInMbs, ref_frames);

    ref_frames = (ref_frames > 16) ? 16 : ref_frames;

    LOGD("Final ref_frames [%d]", ref_frames);

    return (ref_frames + 3 + 2*NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE);
}

//S3D
TIHardwareRenderer::TIHardwareRenderer(
        const sp<Surface> &surface,
        size_t displayWidth, size_t displayHeight,
        size_t decodedWidth, size_t decodedHeight,
        OMX_COLOR_FORMATTYPE colorFormat,
        int32_t rotationDegrees,
        int isS3D, int numOfOpBuffers)
	: mISurface(Test::getISurface(surface)),
      mDisplayWidth(displayWidth),
      mDisplayHeight(displayHeight),
      mDecodedWidth(decodedWidth),
      mDecodedHeight(decodedHeight),
      mColorFormat(colorFormat),
      nOverlayBuffersQueued(0),
      mIndex(0),
      release_frame_cb(0),
      mCropX(-1),
      mCropY(-1)
{
	VOLOGR("disp width %d, height %d, decode width %d, height %d ISurface 0x%08X", mDisplayWidth, mDisplayHeight, mDecodedWidth, mDecodedHeight, mISurface.get());

	int videoFormat = OVERLAY_FORMAT_CbYCrY_422_I;
	if ((colorFormat == OMX_COLOR_FormatYUV420PackedSemiPlanar)||(colorFormat == OMX_COLOR_FormatYUV420SemiPlanar)) 
		videoFormat = OVERLAY_FORMAT_YCbCr_420_SP;
	else if ((colorFormat == (OMX_COLOR_FORMATTYPE)OMX_TI_COLOR_FormatYUV420PackedSemiPlanar_Sequential_TopBottom))
		videoFormat = OVERLAY_FORMAT_YCbCr_420_SP_SEQ_TB;
	else if(colorFormat == OMX_COLOR_FormatCbYCrY)
		videoFormat =  OVERLAY_FORMAT_CbYCrY_422_I;
	else if (colorFormat == OMX_COLOR_FormatYCbYCr)
		videoFormat = OVERLAY_FORMAT_YCbYCr_422_I;
	else if (colorFormat == OMX_COLOR_FormatYUV420Planar)
	{
		videoFormat = OVERLAY_FORMAT_YCbCr_420_SP;
		VOLOGW("Use YUV420_PLANAR -> YUV422_INTERLEAVED_UYVY converter or NV-12 converter needed!!");
    }
	else
	{
		VOLOGE("Not Supported format, and no coverter available");
		return;
    }

    uint32_t orientation;
    switch (rotationDegrees)
	{
        case 0: orientation = ISurface::BufferHeap::ROT_0; break;
        case 90: orientation = ISurface::BufferHeap::ROT_90; break;
        case 180: orientation = ISurface::BufferHeap::ROT_180; break;
        case 270: orientation = ISurface::BufferHeap::ROT_270; break;
        default: orientation = ISurface::BufferHeap::ROT_0; break;
    }

	VOLOGR("video format %d, orientation %d, isS3D %d", videoFormat, orientation, isS3D);
	sp<OverlayRef> ref = mISurface->createOverlay(mDecodedWidth, mDecodedHeight, videoFormat, orientation, isS3D);
	if (ref.get() == NULL)
		return;
    mOverlay = new Overlay(ref);

    if(numOfOpBuffers == -1)
	{
        /* Calculate the number of overlay buffers required, based on the video resolution
        * and resize the overlay for the new number of buffers
        */
        int overlaybuffcnt = Calculate_TotalRefFrames(mDisplayWidth, mDisplayHeight);
        int initialcnt = mOverlay->getBufferCount();
        if (overlaybuffcnt != initialcnt)
		{
            mOverlay->setParameter(OVERLAY_NUM_BUFFERS, overlaybuffcnt);
            mOverlay->resizeInput(mDecodedWidth, mDecodedHeight);
        }
        mOverlay->setParameter(OPTIMAL_QBUF_CNT, 2*NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE);
    }
	else
	{
       /* Number of buffers will be set as recommended by user or codec */
       VOLOGR("Overlay Buffer Count [as recommneded] %d", numOfOpBuffers);
       mOverlay->setParameter(OVERLAY_NUM_BUFFERS, numOfOpBuffers);
       mOverlay->resizeInput(mDecodedWidth, mDecodedHeight);
       mOverlay->setParameter(OPTIMAL_QBUF_CNT, NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE * 2);
    }

	sp<IMemory> mem;
    for (size_t i = 0; i < (size_t)mOverlay->getBufferCount(); ++i)
	{
		mapping_data_t * data = (mapping_data_t *)mOverlay->getBufferAddress((void *)i);
		VOLOGR("i %d, fd %d, length %d, offset %d ptr %p", i, data->fd, data->length, data->offset, data->ptr);

        mVideoHeaps[i] = new MemoryHeapBase(data->fd, data->length, 0, data->offset);
        mem = new MemoryBase(mVideoHeaps[i], 0, data->length);

        VOLOGR("mem->pointer[%d] = %p", i, mem->pointer());

        mOverlayAddresses.push(mem);
        buffers_queued_to_dss[i] = 0;
    }
}

TIHardwareRenderer::~TIHardwareRenderer()
{
	if (mOverlay.get() != NULL)
	{
		sp<IMemory> mem;
		unsigned int sz = mOverlayAddresses.size();
		for (size_t i = 0; i < sz; ++i)
		{
			mem = mOverlayAddresses[i];
			mem.clear();
			//dispose the memory allocated on heap explicitly
			(mVideoHeaps[i].get())->dispose();
			mVideoHeaps[i].clear();
		}
		mOverlayAddresses.clear();
		mOverlay->destroy();
		mOverlay.clear();
	}
}

void TIHardwareRenderer::render(const void *data, size_t size, void *platformPrivate)
{
	if (mOverlay.get() == NULL)
		return;

    overlay_buffer_t overlay_buffer;
    size_t i = 0;
    int cropX = 0;
    int cropY = 0;

	for (; i < mOverlayAddresses.size(); ++i)
	{
		unsigned int offsetinPixels = (char *)data - (char *)mOverlayAddresses[i]->pointer();
		if(offsetinPixels < size)
		{
			cropY = (offsetinPixels) / ARMPAGESIZE;
			cropX = (offsetinPixels) % ARMPAGESIZE;
			if((cropY != mCropY) || (cropX != mCropX))
			{
				mCropY = cropY;
				mCropX = cropX;
				mOverlay->setCrop((uint32_t)cropX, (uint32_t)cropY, mDisplayWidth, mDisplayHeight);
			}
			break;
		}
	}

	if (i == mOverlayAddresses.size())
	{
		VOLOGE("Doing a memcpy. Report this issue.");
		memcpy(mOverlayAddresses[mIndex]->pointer(), data, size);
	}
	else
		mIndex = i;

    int nBuffers_queued_to_dss = mOverlay->queueBuffer((void *)mIndex);
	VOLOGR("index %d, queued buffers %d", mIndex, nBuffers_queued_to_dss);
    if (release_frame_cb)
	{
        if (nBuffers_queued_to_dss < 0)
            release_frame_cb(mOverlayAddresses[mIndex], cookie);
        else
        {
            nOverlayBuffersQueued++;
            buffers_queued_to_dss[mIndex] = 1; 
            if (nBuffers_queued_to_dss != nOverlayBuffersQueued) // STREAM OFF occurred
            {
                LOGD("nBuffers_queued_to_dss = %d, nOverlayBuffersQueued = %d", nBuffers_queued_to_dss, nOverlayBuffersQueued);
                LOGD("buffers in DSS \n %d %d %d  %d %d %d", buffers_queued_to_dss[0], buffers_queued_to_dss[1], buffers_queued_to_dss[2], buffers_queued_to_dss[3], buffers_queued_to_dss[4], buffers_queued_to_dss[5]);
                //Release all the buffers that were discarded by DSS upon STREAM OFF.
                for(unsigned int k = 0; k < (unsigned int)mOverlay->getBufferCount(); k++)
                {
                    if ((buffers_queued_to_dss[k] == 1) && (k != mIndex))
                    {
                        buffers_queued_to_dss[k] = 0;
                        nOverlayBuffersQueued--;
                        release_frame_cb(mOverlayAddresses[k], cookie);
                        VOLOGR("Reclaiming the buffer [%d] from Overlay", k);
                    }
                }
            }
        }
    }

    int err = mOverlay->dequeueBuffer(&overlay_buffer);
	VOLOGR("err %d overlay buffer %d", err, (int)overlay_buffer);
    if (err == 0)
	{
        nOverlayBuffersQueued--;
        buffers_queued_to_dss[(int)overlay_buffer] = 0;
        if (release_frame_cb)
            release_frame_cb(mOverlayAddresses[(int)overlay_buffer], cookie);
    }

    if (++mIndex == mOverlayAddresses.size())
		mIndex = 0;
}

bool TIHardwareRenderer::setCallback(release_rendered_buffer_callback cb, void *c)
{
    release_frame_cb = cb;
    cookie = c;
    return true;
}

}  // namespace android
