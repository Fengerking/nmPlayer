/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003 -     		*
 *																		*
 ************************************************************************/
/*******************************************************************************
 *	File:		voOSSourceWrapper.h
 *	
 *	Contains:	voOSSourceWrapper header file
 *	
 *	Written by:	Jeff huang
 *	
 *	Change History (most recent first):
 *	2012-03-20		Jeff			Create file
 *	
 ******************************************************************************/

#ifndef _CONSTREAM_SOURCE_WRAPPER_
#define _CONSTREAM_SOURCE_WRAPPER_

#include "CDllLoad.h"
#include "voOnStreamSource.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

class voOSSourceWrapper : public CDllLoad
{
public:
	voOSSourceWrapper();
	virtual ~voOSSourceWrapper (void);
    
    virtual int         Create();
    virtual int         Destroy();
    
	virtual int         Init(void* pSource, int nSourceFlag, int nSourceType, void* pInitParam, int nInitParamFlag);
	virtual int			Uninit();
	virtual int 		Open();
    virtual int			Close();
    
	virtual int 		Run(void);
	virtual int 		Pause(void);
	virtual int 		Stop(void);
    
	virtual int			SetPos(long long* pTimeStamp);
    virtual int			GetDuration (long long* pDuration);
    
    virtual int			GetSample (int nOutPutType, void* pSample);
    virtual int			GetProgramCount (int* pStreamCount);
    virtual int			GetProgramInfo (int nStream, VOOSMP_SRC_PROGRAM_INFO** ppStreamInfo);
    virtual int         GetCurTrackInfo (int nOutPutType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo);
    
    virtual int         SelectProgram (int nProgram);
	virtual int         SelectStream (int nStream);
    virtual int         SelectTrack (int nTrack);
    
    virtual int         SelectLanguage (int nIndex);
	virtual int         GetLanguage (VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo);
    
    virtual int         SendBuffer (const VOOSMP_BUFFERTYPE& buffer);
    
	virtual int			GetParam(int nParamID, void* pParam);
	virtual int			SetParam(int nParamID, void* pParam);
    
    virtual int			GetVideoCount();
    virtual int			GetAudioCount();
    virtual int			GetSubtitleCount();
    virtual int			SelectVideo(int nIndex);
    virtual int			SelectAudio(int nIndex);
    virtual int			SelectSubtitle(int nIndex);
    virtual bool        IsVideoAvailable(int nIndex);
    virtual bool        IsAudioAvailable(int nIndex);
    virtual bool        IsSubtitleAvailable(int nIndex);
    
    virtual int			CommitSelection();
    virtual int			ClearSelection();
    
    virtual int			GetVideoProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    virtual int			GetAudioProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    virtual int			GetSubtitleProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    virtual int			GetCurrTrackSelection(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
    virtual int			GetCurrPlayingTrackIndex(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
    virtual int			GetCurrSelectedTrackIndex(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
    
    virtual bool        IsBeInit();
private:
    voOSMPSourceAPI     m_cOSSourceFuncSet;
    void *				m_pOSHandle;
    void *              m_pLogCB;
    
    bool                m_bInit;
};

#endif //_CONSTREAM_SOURCE_WRAPPER_