	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOWEdit.cpp

	Contains:	 CVOWEdit class file

	Written by:	Yu Wei (Tom)

	Change History (most recent first):
	2010-05-05		YW			Create file

*******************************************************************************/

#include <stdio.h>
#include <string.h>

#ifdef LINUX
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif // _LINUX

#define LOG_TAG "CVOWEditor"

#include "CVOWEditor.h"

#include "voOMXOSFun.h"

#include "voLog.h"
#include "vojnilog.h"

// ----------------------------------------------------------------------------
typedef VO_S32 (VO_API * VOWGETAPI) (VO_PTR pFunc, VO_U32 uFlag);

CVOWEditor::CVOWEditor(void)
	: CVOWPlayer ()
{
 	JNILOGE("V7");
}

CVOWEditor::~CVOWEditor()
{
	 
}


int CVOWEditor::Init (void)
{
	LoadModule ();
      if (m_hVOWModule == NULL)
	{
		JNILOGE ("It could not load libvoedit module.");
    	       return -1;
	}

 	JNILOGI("after init");
 	
	VO_U32 nRC = 0;

	VO_VOMM_INITPARAM initParam;
	memset (&initParam, 0, sizeof (VO_VOMM_INITPARAM));
	initParam.nFlag = 0;
	initParam.pFileOP = (VO_FILE_OPERATOR *)m_pFilePipe;
	initParam.pMemOP = NULL;
	nRC = m_fVOEditorAPI.Init(&m_hVOWPlay, &initParam);
	if (m_hVOWPlay == NULL)
	{
		JNILOGE ("It could not init voMMEdit.");
		return -1;
	}

	nRC = m_fVOEditorAPI.SetCallBack (m_hVOWPlay, vowMessageHandlerProc, this);

	VO_VOMM_CB_VIDEORENDER vrParam;
	vrParam.pCallBack = vowVideoRenderProc;
	vrParam.pUserData = this;
	m_fVOEditorAPI.SetParam (m_hVOWPlay, VO_VOMMP_PMID_VideoRender, &vrParam);

	VO_VOMM_CB_AUDIORENDER arParam;
	arParam.pCallBack = vowAudioRenderProc;
	arParam.pUserData = this;
	m_fVOEditorAPI.SetParam (m_hVOWPlay, VO_VOMMP_PMID_AudioRender, &arParam);

	return 0;
}


int CVOWEditor::SetSource (const char* pSource)
{
	JNILOGI2 ("Editor Set Source: %s  ", pSource);

	if (m_hLoadThread != NULL)
		return -1;

	strcpy (m_szFile, pSource);

	m_nSourceType = 0;

	m_nOffset = 0;
	m_nLength = 0;

	m_nStatus = -1;
	m_bPrepareAsync = false;
	m_bUninit = false;
	
	JNILOGI2("setsource thread id: 0x%08x", pthread_self());

	if (Init () < 0)
		return -1;	
	
      JNILOGI("after init");
	
	return 0;
}

int CVOWEditor::ExportFile (const char* pSource)
{	
	JNILOGI("ExportFile");
	Run(); 
	JNILOGI("after run");
	return 0;
}


 int    CVOWEditor::GetExportPercent()
 {
     
      return 0;
 }

 int CVOWEditor::LoadModule (void)
{
#ifdef _LINUX
	
 	 
	m_hVOWModule = dlopen ("/data/data/com.visualon.vome/lib/libvoedit.so", RTLD_NOW);

	if (m_hVOWModule == NULL) {
		m_hVOWModule = dlopen ("/data/data/com.visualon.voeditor/lib/libvoedit.so", RTLD_NOW);
	}
	else 
		JNILOGI("/data/data/com.visualon.vome/lib/libvoedit.so loaded");

	if (m_hVOWModule == NULL) {
		m_hVOWModule = dlopen ("libvoedit.so", RTLD_NOW);
	}
	else 
		JNILOGI("/data/data/com.visualon.voeditor/lib/libvoedit.so loaded");

	

	if (m_hVOWModule == NULL)
	{
		JNILOGE2("@@@@@@ Load Module Error %s \n", dlerror ());
		return -1;
	}

	

	VOWGETAPI pAPI = (VOWGETAPI) dlsym(m_hVOWModule, "vommGetEditAPI");
	if (pAPI == NULL)
	{
		JNILOGE2 ("@@@@@@ Get function address Error %s \n", dlerror ());
		return -1;
	}

	pAPI (&m_fVOEditorAPI, 0);

	JNILOGI("after load api");
#endif //_LINUX

	return 0;
}

 int CVOWEditor::Run (void)
{
      JNILOGI("Run");
     
   	if (m_hVOWPlay == NULL)
       	return -1;

 	JNILOGI("before run");
	if (m_fVOEditorAPI.Run (m_hVOWPlay)== VO_ERR_NONE)
	    {
	    	JNILOGI("after run");
	    	/*
			if (m_nStartPos > 0)
			{
				m_fVOEditorAPI.SetCurPos (m_hVOWPlay, m_nStartPos);
				m_nStartPos = 0;
			}

		    	m_nStatus = 1;
		    	return 0;
		    	*/
		    	return 0;
	   }
	else
		 JNILOGE("failed to run");

	return -1;

/*
	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioStart, NULL, NULL);

	if (m_bAutoStart)
		m_bAutoStart = false;

    if (m_fVOWAPI.Run (m_hVOWPlay)== VO_ERR_NONE)
    {
		if (m_nStartPos > 0)
		{
			m_fVOWAPI.SetCurPos (m_hVOWPlay, m_nStartPos);
			m_nStartPos = 0;
		}

    	m_nStatus = 1;
    	return 0;
    }

	return -1;
	*/
 }


  
