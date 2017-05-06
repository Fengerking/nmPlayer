	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCCRR.h

	Contains:	voCCRR header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __voCCRR_H__
#define __voCCRR_H__

#include "voCCRRR.h"

namespace android {

class voCCRR
{
public:
	voCCRR (void);
	virtual ~voCCRR (void);

	int		SetInputSize (int width, int height, int left, int top, int right, int bottom);
	int		SetOutputSize (int width, int height, int left, int top, int right, int bottom);

//	int		ProcessRGB565 (int format, unsigned char * source, unsigned char * target, int angle);

	int		SetSize(int nInWidth, int nInHeight, int nOutWidth, int nOutHeight, int angle);
	int		ProcessRGB565 (int format, VO_VIDEO_BUFFER* pIn, VO_VIDEO_BUFFER* pOut, int angle);
	void  setWorkingPath(const char* path);

protected:
	VO_U32				LoadLib (void);
	int					UpdateSize (void);

protected:
	VO_PTR				m_hModule;
	VO_VIDEO_CCRRRAPI	m_ccrrFunc;
	VO_HANDLE			m_hCCRR;
	VO_CCRRR_PROPERTY	m_prop;

	int					m_nInWidth;
	int					m_nInHeight;
	int					m_nInLeft;
	int					m_nInTop;
	int					m_nInRight;
	int					m_nInBottom;

	int					m_nOutWidth;
	int					m_nOutHeight;
	int					m_nOutLeft;
	int					m_nOutTop;
	int					m_nOutRight;
	int					m_nOutBottom;

	VO_IV_COLORTYPE		m_nOutColor;

	int					m_nAngle;

	VO_VIDEO_BUFFER		m_inBuffer;
	VO_VIDEO_BUFFER		m_outBuffer;

	VO_IV_RTTYPE		m_nRotate;
	char				m_szworkingpath[128];
};

// ---------------------------------------------------------------------------
}; // namespace android

#endif // __voCCRR_H__
