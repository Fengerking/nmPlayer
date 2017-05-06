#ifndef __COSWinVomePlayer_H__
#define __COSWinVomePlayer_H__

#include "COSVomePlayer.h"
#define TRY_TO_HOLD_VIDEO_BUF 0

class COSWinAudioRender;
class COSWinVideoRender;

class COSWinVomePlayer : public COSVomePlayer
{
public:
	// Used to control the image drawing
	COSWinVomePlayer ();
	virtual ~COSWinVomePlayer (void);

	virtual int			Init();
	virtual int			Uninit();

	virtual int			SetDataSource (void * pSource, int nFlag);

	virtual int			SetView(void* pView);

	virtual int 		Run (void);
	virtual int 		Pause (void);
	virtual int 		Stop (void);
	virtual int 		Close (void);
	virtual int 		Flush (void);
	virtual int 		SetParam (int nID, void * pValue);
	virtual int			SetPos (int nCurPos);
	
	virtual int			HandleEvent (int nID, void * pParam1, void * pParam2);

protected:
	void *				m_hWnd;
	VOOSMP_RENDER_TYPE	m_nVideoRenderType;
	VO_BOOL				m_bVideoRenderCallback;

	voCMutex			m_VideoLock;

	int					m_nAspectRatio;

	COSWinAudioRender *	m_pAudioRender;
	COSWinVideoRender *	m_pVideoRender;
	VOOSMP_ASPECT_RATIO	m_nVideoAspectRatio;
	bool				m_bWindowlessMode;
};

#endif // __COSWinVomePlayer_H__
