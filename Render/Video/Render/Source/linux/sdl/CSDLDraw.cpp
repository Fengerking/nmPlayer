#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "cmnMemory.h"
#include "voIVCommon.h"
#include "voOSFunc.h"
#include "CSDLDraw.h"

CSDLDraw::CSDLDraw(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
                               :CBaseVideoRender(hInst, hView, pMemOP)
							   ,mIsHWSurface(false)
							   ,mScreen(NULL)
#if defined(_RGB_OUTPUT)
							   ,mSurface(NULL)
#elif defined(_YUV_OUTPUT)
							   ,mOverlay(NULL)	
#endif

{
	if (hView != NULL)
		pMemOP->Copy(VO_INDEX_SNK_VIDEO, &m_rcDisplay, (VO_RECT*)hView, sizeof(VO_RECT));
    InitDevice();
    
}

CSDLDraw::~CSDLDraw()
{
	CloseDevice();

}



bool CSDLDraw::prepareFrame(VO_VIDEO_BUFFER* lpdata)
{

	bool bOk = true;
#if defined(_RGB_OUTPUT)

	// 32 bit: a:ff000000 r:ff000 g:ff00 b:ff
	// 24 bit: r:ff0000 g:ff00 b:ff
	// 16 bit: r:1111100000000000b g:0000011111100000b b:0000000000011111b
	// 15 bit: r:111110000000000b g:000001111100000b b:000000000011111b

	if(!mSurface)
	{
		mSurface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, m_nDrawWidth, m_nDrawHeight, 16, 63488, 2016, 31, 0);
	}
	assert (mSurface != NULL);

	SDL_LockSurface(mSurface);

    VO_VIDEO_BUFFER out;
	out.Buffer[0] = (unsigned char*)mSurface->pixels;
	out.Stride[0]= m_nDrawWidth * 2;

	if (!ConvertData(lpdata,&out, 0, VO_FALSE ))
	{
#  if defined(_DEBUG)
		printf("can not conver yuv data into rgb data\n");
#  endif
		bOk = false;
	}
	SDL_UnlockSurface(mSurface);
#elif defined(_YUV_OUTPUT)
	// since we use the I420 format, but it is the same format with IYUV format
	if(!mOverlay)
	{
		mOverlay = SDL_CreateYUVOverlay(m_nVideoWidth, m_nVideoHeight, SDL_IYUV_OVERLAY, mScreen);
	}
	assert(mOverlay != NULL);

	SDL_LockYUVOverlay(mOverlay);
	
	Uint16 strides[3];
	for(int j = 0; j < 3; j++)
	{
		strides[j]=lpdata->Stride[j];
	}
	mOverlay->pitches = strides;
	mOverlay->pixels = lpdata->Buffer;
    mOverlay->planes = 3;
	SDL_UnlockYUVOverlay(mOverlay);
#endif

   return bOk;
}


VO_U32 CSDLDraw::Render(VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait) 
{
   onDrawImage(pVideoBuffer);
   return VO_ERR_NONE;
}
bool CSDLDraw::onDrawImage(VO_VIDEO_BUFFER* pBuffer)
{

	if(!mScreen)
	{
		int flags = SDL_DOUBLEBUF|SDL_ANYFORMAT;
		if(mIsHWSurface)
			flags |= SDL_HWSURFACE;
		else
			flags |= SDL_SWSURFACE;
		if(m_nVideoWidth && m_nVideoHeight)
#if defined(_ARM_VER)
			mScreen = SDL_SetVideoMode(m_nScreenWidth, m_nScreenHeight, 16, flags);
#else
			mScreen = SDL_SetVideoMode(m_nDrawWidth, m_nDrawHeight, 16, flags);
#endif
		assert(mScreen != NULL);
	}

	if(prepareFrame(pBuffer))
	{
		
#if defined(_RGB_OUTPUT)
		SDL_BlitSurface(mSurface, NULL, mScreen, NULL);
		SDL_Flip(mScreen);
#elif defined(_YUV_OUTPUT)
		SDL_Rect rect = {0, 0, m_nVideoWidth, m_nVideoHeight};
		
		SDL_DisplayYUVOverlay(mOverlay, &rect);
		//SDL_Flip(mScreen);
#endif
		return true;
	}
   return false;
}

 
bool CSDLDraw::InitDevice()
{
   bool bOk = false;
   
	
   if(SDL_Init(SDL_INIT_VIDEO) < 0)
   {
#if defined(_DEBUG)
      printf("can not call sdl init the reason is %s\n", SDL_GetError());
#endif
      return bOk;
   }
   getSDLVideoInfo();
   bOk = true;
	
   return bOk;
}

bool CSDLDraw::CloseDevice()
{
#if defined(_RGB_OUTPUT)
	if(mSurface)
	{
		SDL_FreeSurface(mSurface);
		mSurface = NULL;
	}
#elif defined(_YUV_OUTPUT)
	if(mOverlay)
	{
		SDL_FreeYUVOverlay(mOverlay);
		mOverlay = NULL;
	}
#endif
	
	
    if(mScreen)
    {
       SDL_FreeSurface(mScreen);
       mScreen = NULL;
    }
    SDL_Quit();
	
	return true;
}

void CSDLDraw::getSDLVideoInfo()
{
	const SDL_VideoInfo* pInfo = SDL_GetVideoInfo();
	if(pInfo->hw_available)
		mIsHWSurface = true;

}




