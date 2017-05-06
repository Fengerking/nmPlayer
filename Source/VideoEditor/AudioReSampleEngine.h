
#ifndef __AUDIORESAMPLEENGINE_H__
#define __AUDIORESAMPLEENGINE_H__

#ifdef _WIN32
	#include <windows.h>
	#include <tchar.h>
#endif

#include "voResample.h"
#include "voString.h"
#include "videoEditorType.h"
#include "CDllLoad.h"

//VO_S32 VO_API voGetResampleAPI (VO_AUDIO_CODECAPI * pResHandle);
typedef VO_S32 (VO_API * VOGETRESAMPLEAPI) (VO_AUDIO_CODECAPI * pResHandle);

class CAudioReSampleEngine : public CDllLoad
{
public:
	CAudioReSampleEngine ();
	virtual ~CAudioReSampleEngine (void);

	virtual VO_U32		Init ();
	virtual VO_U32		Uninit (void);

	virtual void	SetLibOperator (VO_LIB_OPERATOR * pLibOP) {m_pLibOP = pLibOP;CDllLoad::SetLibOperator(m_pLibOP);}

	virtual int 		GetParam (int nID, void * pValue);
	virtual int 		SetParam (int nID, void * pValue);
	VO_U32	Process(VOMP_BUFFERTYPE *inputData);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:
	VO_VOID RelaeseBuffer();
	int			LoadDll (void);

	VO_AUDIO_CODECAPI m_AudioResampleAPI;
	VO_AUDIO_OUTPUTINFO m_OutInfo;
	VO_CODECBUFFER m_OutData;

	VO_TCHAR       m_cTitle[255];
	VO_HANDLE	m_pResHandle;
	VOMP_VIDEO_FORMAT m_srcFormat;

	VO_VIDEO_BUFFER ImgBuffer;
	VOMP_BUFFERTYPE m_sourceImgBuffer;
	VOMP_BUFFERTYPE m_curImgBuffer;
	VOMP_BUFFERTYPE m_tmpImgBuffer;
	VOMP_VIDEO_FORMAT m_outFormat;
	
};

#endif // __AUDIORESAMPLEENGINE_H__