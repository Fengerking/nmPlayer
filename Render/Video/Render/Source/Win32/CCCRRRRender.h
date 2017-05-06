#pragma once
#include "cbasevideorender.h"

class CCCRRRRender : public CBaseVideoRender
{
public:
	CCCRRRRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~CCCRRRRender(void);

	virtual VO_U32 Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);
	virtual VO_U32 SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor = VO_COLOR_YUV_PLANAR420);
	

protected:
	virtual VO_U32	UpdateSize (void);
	virtual void	EraseBackGround();

private:
	HDC					m_hWinDC;
};
