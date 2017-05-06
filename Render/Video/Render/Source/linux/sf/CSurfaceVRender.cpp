	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CSurfaceVRender.cpp

	Contains:	CSurfaceVRender class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-08-31		XGT			Create file

*******************************************************************************/
#include "cmnMemory.h"
#include "voIVCommon.h"
#include "voOSFunc.h"

#include "CSurfaceVRender.h"

#include <fcntl.h>

namespace android 
{
class Test 
{
public:
    static const sp<ISurface>& getISurface(const sp<Surface>& s) 
      {
        return s->getISurface();
      }
};
};


CSurfaceVRender::CSurfaceVRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
   : CBaseVideoRender (hInst, hView, pMemOP),
     mClient(NULL),
     mSurface(NULL),
     mISurface(NULL),
     mFrameHeap(NULL),
     mOutStride(0),
	 mOffset (0)
{
	if (hView != NULL)
		pMemOP->Copy(VO_INDEX_SNK_VIDEO, &m_rcDisplay, (VO_RECT*)hView, sizeof(VO_RECT));

    memset (&mOutBuffer, 0, sizeof (VO_VIDEO_BUFFER));

	m_bRotateType =  VO_RT_90R;
}

CSurfaceVRender::~CSurfaceVRender() 
{
   ReleaseSurface ();
}

VO_U32 CSurfaceVRender::Render(VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait) 
{
	voCAutoLock lock (&m_csDraw);
	if(mClient == NULL)
		CreateSurface ();

	if(mISurface != NULL)
	{
		if (!ConvertData (pVideoBuffer, &mOutBuffer, nStart, bWait))
			return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;

		mISurface->postBuffer(0);

		mClient->closeTransaction();
	}

	return VO_ERR_NONE;
}

VO_U32 CSurfaceVRender::UpdateSize (void) 
{
	CBaseVideoRender::UpdateSize ();

	voCAutoLock lock (&m_csDraw);
	if (m_bRotate)
        mOutStride = 2 * m_nDrawHeight;
	else
        mOutStride = 2 * m_nDrawWidth;

	return 0;
}

bool CSurfaceVRender::CreateSurface()
{
	if(mClient == NULL)
	{
		mClient = new SurfaceComposerClient();
		assert(mClient != NULL);

		int pid = getpid();
		mSurface = mClient->createSurface(pid, 0, m_nScreenWidth, m_nScreenHeight, PIXEL_FORMAT_UNKNOWN, ISurfaceComposer::eFXSurfaceNormal|ISurfaceComposer::ePushBuffers);

		mISurface = Test::getISurface(mSurface);
		assert(mISurface != NULL);

		int nFrameSize;

		// create frame buffer heap and register with surfaceflinger
		nFrameSize = m_nScreenWidth * m_nScreenHeight * 2;
		mFrameHeap = new MemoryHeapBase(nFrameSize);
		if (mFrameHeap->heapID() < 0) 
		{
			printf("Error creating frame buffer heap\n");
			return false;
		}

		ISurface::BufferHeap buffers(m_nScreenWidth, m_nScreenHeight, m_nScreenWidth, m_nScreenHeight, PIXEL_FORMAT_RGB_565, mFrameHeap);
		mISurface->registerBuffers(buffers);

		mClient->setOrientation(0, ISurfaceComposer::eOrientationDefault, ISurfaceComposer::eOrientationDefault);
		mClient->openTransaction();
		mSurface->show();
		mSurface->setLayer(INT_MAX);

		mOutStride = 2 * m_nScreenWidth;
		mOffset = m_nDrawLeft * 2 + m_nDrawTop * mOutStride;

		mOutBuffer.Buffer[0] = static_cast<unsigned char*>(mFrameHeap->base()) + mOffset;
		mOutBuffer.Stride[0] = mOutStride;
	}

	return true;
}
                      
bool CSurfaceVRender::ReleaseSurface()
{
   if (mISurface != NULL && mISurface.get()) 
   {
        mISurface->unregisterBuffers();
   }
    // free heaps
    mFrameHeap.clear();

   return true;
}

