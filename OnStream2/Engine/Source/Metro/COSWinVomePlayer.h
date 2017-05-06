#ifndef __COSWinVomePlayer_H__
#define __COSWinVomePlayer_H__

#include "COSVomePlayer.h"


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
	
protected:
	Windows::UI::Core::CoreWindow^	m_Window;

	COSWinAudioRender *m_pAudioRender;
	COSWinVideoRender *m_pVideoRender;


};

#endif // __COSWinVomePlayer_H__
