#include <assert.h>
#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// for __android_log_print(ANDROID_LOG_INFO, "YourApp", "formatted message");
#include <android/log.h>
#define TAG "NativeMedia"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)

#include <dlfcn.h>
#include <android/native_window_jni.h>


#include "OnStreamReadSrc.h"
#include "COMXALPlayer.h"

#if defined __cplusplus 
extern "C" {
#endif
	
bool					g_bWantSeek = false;
COMXALPlayer			g_Engn;
OnStreamReadSrc*		g_pSrc = NULL;
VOOSMP_LIB_FUNC			g_LibOP;
char					g_szPathLib[1024];
	
static void * vomtGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag)
{
	void* ptr = dlsym (hLib, pFuncName);
	
	LOGV( "VOLOG Func Name: %s  %p" , pFuncName , ptr );
	
	return ptr;
}

static int vomtFreeLib (void * pUserData, void * hLib, int nFlag)
{
	LOGV("VOLOG Free Lib: %0xd", (unsigned int) hLib);
	dlclose (hLib);
	return 0;
}

static void * vomtLoadLib (void * pUserData, char * pLibName, int nFlag)
{
	char szFile[256];
	
	uint64_t nCpuFeature;
	
	memset(szFile, 0, sizeof(szFile));
	
	{
		strcpy (szFile, g_szPathLib);
	}
	strcat (szFile, "lib/lib");
	
	{
		strcat(szFile, pLibName); // use non-NEON routines and call v6 lib
	}
	
	if( strstr( szFile , ".so" ) == 0 )
	{
		strcat( szFile , ".so" );
	}
	
	LOGV("VOLOG Load lib name: %s.",szFile);
	
	void * hDll = NULL;
	
	{
		hDll = dlopen (szFile, RTLD_NOW);
	}
	
	if (hDll == NULL)
	{
		strcpy (szFile, g_szPathLib);
		strcat (szFile, pLibName);
		
		if( strstr( szFile , ".so" ) == 0 )
			strcat( szFile , ".so" );
		
		{
			hDll = dlopen (szFile, RTLD_NOW);
		}
		
		LOGV ("VOLOG Load Library File %s, Handle %p", szFile, hDll);
	}else
	{
		LOGV ("VOLOG Load Library File %s, Handle %p", szFile, hDll);
	}
	
	return hDll;
}
	

int Init()
{
	LOGV("[Jim] +Init()");
	
	memset(g_szPathLib, 0, 1024*sizeof(char));
	char	szPackageName[1024];
	FILE * hFile = fopen("/proc/self/cmdline", "rb");
	if (hFile != NULL)
	{  
		fgets(szPackageName, 1024, hFile);
		fclose(hFile);
		if (strstr (szPackageName, "com.") != NULL)
			sprintf(g_szPathLib, "/data/data/%s/", szPackageName);
	}
	
	
	memset(&g_LibOP, 0, sizeof(VOOMXAL_LIB_FUNC));
	g_LibOP.pUserData	= NULL;
	g_LibOP.GetAddress	= vomtGetAddress;
	g_LibOP.FreeLib		= vomtFreeLib;
	g_LibOP.LoadLib		= vomtLoadLib;
	
	LOGV("Set lib op");
	// it must set lib op before Init
	g_Engn.SetParam(VOOSMP_PID_FUNC_LIB, &g_LibOP);
	int nRet = g_Engn.Init();
	
	g_pSrc = new OnStreamReadSrc;
	return nRet;
}

int Uninit()
{
	LOGV("[Jim] +Uninit()");

	g_Engn.Uninit();
		
	if(g_pSrc)
	{
		g_pSrc->Stop();
		g_pSrc->Uninit();
		delete g_pSrc;
		g_pSrc = NULL;
	}
	
	return 0;
}

int SetView(void* pView)
{
	LOGV("[Jim] +SetView()");
	
	return g_Engn.SetView(pView);
}

int Open(void* pSource, int nFlag)
{
	LOGV("[Jim] +Open()");

	if(g_pSrc)
	{
		g_pSrc->SetParam(AV_FILE_PATH, (void*)"/sdcard/");
		g_pSrc->Init(NULL, 0);
		g_pSrc->Run();
	}
	
	LOGV("[Jim] Set data source");
	return g_Engn.SetDataSource(pSource, nFlag);
}


int Run()
{
	LOGV("[Jim] +Run()");
	
	return g_Engn.Run();
}

int Pause()
{
	LOGV("[Jim] +Pause()");
	
	return g_Engn.Pause();
}
	
int Stop()
{
	LOGV("[Jim] +Stop()");

	if(g_pSrc)
	{
		g_pSrc->Stop();
	}	
	
	return g_Engn.Stop();
}
	

int Seek(int nPos)
{
	LOGV("[Jim] +Seek(%d)", nPos);
	
	if(g_pSrc)
	{
		g_pSrc->SetCurPos(nPos);
	}
	
	g_Engn.SetPos(nPos);
}















// create the engine and output mix objects
void Java_com_example_nativemedia_NativeMedia_createEngine(JNIEnv* env, jclass clazz)
{	
	Init();
	return;
}


// create streaming media player
jboolean Java_com_example_nativemedia_NativeMedia_createStreamingMediaPlayer(JNIEnv* env,
        jclass clazz, jstring filename)
{
    // convert Java string to UTF-8
    //const char *utf8 = (*env)->GetStringUTFChars(env, filename, NULL);
    //assert(NULL != utf8);
	
	Open(g_pSrc->GetReadBufPtr(), VOOSMP_FLAG_SOURCE_READBUFFER);
	return JNI_TRUE;
}


// set the playing state for the streaming media player
void Java_com_example_nativemedia_NativeMedia_setPlayingStreamingMediaPlayer(JNIEnv* env,
        jclass clazz, jboolean isPlaying)
{
	{
		if(g_bWantSeek)
		{
			//Seek(1000*(rand()%30));
			Seek(1000*20);
			return;
		}
		
		if(isPlaying)
		{
			Run();
			g_bWantSeek = true;
		}
		else
		{
			Pause();
			g_bWantSeek = true;
		}
	}
	return;
}


// shut down the native media system
void Java_com_example_nativemedia_NativeMedia_shutdown(JNIEnv* env, jclass clazz)
{
	g_bWantSeek = false;
	Stop();
	Uninit();
	return;
}


// set the surface
void Java_com_example_nativemedia_NativeMedia_setSurface(JNIEnv *env, jclass clazz, jobject surface)
{
    // obtain a native window from a Java surface
    //theNativeWindow = ANativeWindow_fromSurface(env, surface);
	
	//jun
	SetView(ANativeWindow_fromSurface(env, surface));
}


// rewind the streaming media player
void Java_com_example_nativemedia_NativeMedia_rewindStreamingMediaPlayer(JNIEnv *env, jclass clazz)
{	
	/*
    XAresult res;

    // make sure the streaming media player was created
    if (NULL != playerBQItf && NULL != file) {
        // first wait for buffers currently in queue to be drained
        int ok;
        ok = pthread_mutex_lock(&mutex);
        assert(0 == ok);
        discontinuity = JNI_TRUE;
        // wait for discontinuity request to be observed by buffer queue callback
        // Note: can't rewind after EOS, which we send when reaching EOF
        // (don't send EOS if you plan to play more content through the same player)
        while (discontinuity && !reachedEof) {
            ok = pthread_cond_wait(&cond, &mutex);
            assert(0 == ok);
        }
        ok = pthread_mutex_unlock(&mutex);
        assert(0 == ok);
    }
*/
}

#if defined __cplusplus
}
#endif
