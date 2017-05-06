	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		VOJAVAVideoRender.cpp

	Contains:	VOJAVAVideoRender class file

	Written by:	Tom Yu Wei

	Change History (most recent first):
	2010-09-09		Tom			Create file

*******************************************************************************/

#define LOG_TAG "VOJAVAVideoRender"

#include <android_runtime/AndroidRuntime.h>

#include "VOJAVAVideoRender.h"
#include "voLog.h"
#include "voOMXOSFun.h"

namespace android {

VOJAVAVideoRender::VOJAVAVideoRender(void)
	: VOBaseVideoRender ()	
	, mClass(NULL)		
	, mMethodInit(NULL)
    , mMethodRender(NULL)
	, mVD(NULL)
	, m_pCCRR(NULL)
	, m_bThreadEventStop(false)
	, m_bThreadExit(false)
{
	Init();   
}

VOJAVAVideoRender::~VOJAVAVideoRender(void)
{
	voCOMXAutoLock lock (&m_tmTrack);
	VOLOGI();
	m_bThreadEventStop = true;	
	m_sEvent.msg	   = JAVAVR_EXIT;
	m_tsEventStart.Up ();

	int nTimes = 500;
	while (!m_bThreadExit && nTimes > 0)
	{
		nTimes --;
		voOMXOS_Sleep(2);
	}

	JNIEnv * pEnv = AndroidRuntime::getJNIEnv();
	if (mClass != NULL)
	    pEnv->DeleteGlobalRef(mClass);
	    
	if (m_pCCRR)
		delete m_pCCRR;	
}

int VOJAVAVideoRender::SetVideoSurface (const sp<Surface>& surface)
{	
	return 0;
}

int VOJAVAVideoRender::SetVideoSize (int nWidth, int nHeight)
{
	voCOMXAutoLock lock (&m_tmTrack);

	m_nVideoWidth = nWidth;
	m_nVideoHeight = nHeight;
	m_nVideoWidth = (m_nVideoWidth + 3) & ~0X03;
	m_nVideoHeight = (m_nVideoHeight + 1) & ~0X01;

	m_sEvent.msg = JAVAVR_SETVIDEOSIZE;
	m_tsEventStart.Up ();
	m_tsEventFinish.Down ();

	return 0;
}


bool VOJAVAVideoRender::CheckColorType (VO_ANDROID_COLORTYPE nColorType)
{
	voCOMXAutoLock lock (&m_tmTrack);

	if (nColorType == VO_ANDROID_COLOR_RGB565 || nColorType == VO_ANDROID_COLOR_YUV420)
		return true;

	return false;
}


bool VOJAVAVideoRender::Render (VO_ANDROID_VIDEO_BUFFERTYPE * pVideoBuffer)
{		
	voCOMXAutoLock lock (&m_tmTrack);

	m_sEvent.msg =  JAVAVR_RENDER;
	m_sEvent.ext1 = (int)pVideoBuffer;
	
	m_tsEventStart.Up ();
	m_tsEventFinish.Down ();

	return true;
}


void VOJAVAVideoRender::Init()
{
	if (mClass)
		return;

	VOLOGI("Init JAVA Video Render3");

	JNIEnv* env = AndroidRuntime::getJNIEnv();
	if (env == NULL)
	{
		VOLOGE("Env is NULL");
		return;
	}

	jclass clzVideoRender = env->FindClass("com/mobitv/vome/VOVideoRender");
	if (clzVideoRender != NULL) 
	{
		mClass = (jclass)env->NewGlobalRef(clzVideoRender);	

		mMethodInit = env->GetStaticMethodID(mClass, "vrinit", "(II)V");
		if (mMethodInit == NULL)
		{
			VOLOGE("Can't find vrinit");
			return;
		}	

		mMethodRender = env->GetStaticMethodID(mClass, "vrrender", "([B)V");
		if (mMethodRender == NULL)
		{
			VOLOGE("Can't find vrrender");
			return;
		}		

		

		m_bThreadEventStop = false;
		char name[128];
		sprintf(name, "java_vr_thread-%s-%s", __DATE__, __TIME__);
		AndroidRuntime::createJavaThread(name, (void(*)(void*))vomeVRThreadProc, (void*)this); 

		m_bThreadExit = false;
		VOLOGI("Finished JAVAVideoRender init");
	}
	else
		VOLOGE("clzVideoRender is NULL");
}

void VOJAVAVideoRender::vomeVRThreadProc (void* pparam)
{
	VOLOGF();

	VOJAVAVideoRender* pJAVAVideoRender = (VOJAVAVideoRender*)pparam;

	JNIEnv* env = AndroidRuntime::getJNIEnv();
	while (!pJAVAVideoRender->m_bThreadEventStop)
		pJAVAVideoRender->RenderVideoLoop(env);	

	pJAVAVideoRender->m_bThreadExit = true;	
}


void VOJAVAVideoRender::RenderVideoLoop(JNIEnv * env)
{
	VOLOGF();

	m_tsEventStart.Down ();

	if (m_bThreadEventStop)
		return;

	switch(m_sEvent.msg) 
	{
		case JAVAVR_SETSURFACE:
			 break;

		case JAVAVR_SETVIDEOSIZE:
			{
				//VOLOGI("JAVAVR_SETVIDEOSIZE,%d,%d",m_nVideoWidth,m_nVideoHeight);
				if (mVD != NULL)
				{
					env->DeleteLocalRef(mVD);
					mVD = NULL;
				}
				//VOLOGI("JAVAVR_SETVIDEOSIZE0");
				mVD = env->NewByteArray(m_nVideoWidth*m_nVideoHeight*2);			
				//VOLOGI("JAVAVR_SETVIDEOSIZE1");	
				env->CallStaticVoidMethod(mClass, mMethodInit, m_nVideoWidth, m_nVideoHeight);
				//VOLOGI("JAVAVR_SETVIDEOSIZE2");
			}
			break;

		case JAVAVR_RENDER:		
			{	
				//VOLOGI("JAVAVR_RENDER0");
				if (m_pCCRR == NULL)
				{
					VOLOGI("JAVAVR_RENDER1");
					m_pCCRR = new voCCRR();

					char szPackageName[2048];
					FILE *cmdline = fopen("/proc/self/cmdline", "rb");
					char *arg = 0;
					size_t size = 0;
					if (cmdline != NULL)
					{		
						fgets(szPackageName, 2048, cmdline);
						sprintf(m_szWorkingPath, "/data/data/%s/", szPackageName);
						fclose(cmdline);
					}
					else
						strcpy(m_szWorkingPath, "/data/data/com.visualon.vome");
					VOLOGI("JAVAVR_RENDER,%s",m_szWorkingPath);
					m_pCCRR->setWorkingPath(m_szWorkingPath);
				}		

				VO_ANDROID_VIDEO_BUFFERTYPE * pVideoBuffer = (VO_ANDROID_VIDEO_BUFFERTYPE*)m_sEvent.ext1;
				jbyte* b =  env->GetByteArrayElements(mVD, NULL);

				m_inBuf.Buffer[0] = pVideoBuffer->virBuffer[0];
				m_inBuf.Buffer[1] = pVideoBuffer->virBuffer[1];
				m_inBuf.Buffer[2] = pVideoBuffer->virBuffer[2];

				m_inBuf.Stride[0] = pVideoBuffer->nStride[0];
				m_inBuf.Stride[1] = pVideoBuffer->nStride[1];
				m_inBuf.Stride[2] = pVideoBuffer->nStride[2];

				m_outBuf.ColorType = VO_COLOR_RGB565_PACKED;	
				m_outBuf.Buffer[0] = (VO_PBYTE)b;
				m_outBuf.Stride[0] = m_nVideoWidth*2; 
				//VOLOGI("JAVAVR_RENDER1");
				if (m_pCCRR->SetInputSize (m_nVideoWidth, m_nVideoHeight, 0, 0, m_nVideoWidth, m_nVideoHeight) >= 0)
				    if (m_pCCRR->SetOutputSize (m_nVideoWidth, m_nVideoHeight, 0, 0, m_nVideoWidth, m_nVideoHeight) >= 0)
						 m_pCCRR->ProcessRGB565 (VO_COLOR_YUV_PLANAR420, &m_inBuf, &m_outBuf, 0);
			
				env->ReleaseByteArrayElements(mVD, b, 0);
				//VOLOGI("JAVAVR_RENDER2,%X",mMethodRender);
				env->CallStaticVoidMethod(mClass, mMethodRender, mVD);
				//VOLOGI("JAVAVR_RENDER3");
			}	
			break;

		case JAVAVR_EXIT:
			break;

		default:
			break;
	}

	m_tsEventFinish.Up ();
}

} // end namespace android

